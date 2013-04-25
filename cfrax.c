#define _GNU_SOURCE

#include "cfrax.h"

#if INTERFACE

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { gray, rgb, rgba } COLOR;

typedef unsigned char Pixel;
typedef unsigned short Alpha;

typedef struct Canvas_struct {
    unsigned int width;
    unsigned int height;
    unsigned int depth;
    Pixel *image;
    Alpha *alpha;
    COLOR color;
} Canvas;

typedef struct Point_struct {
    float x;
    float y;
    float z;
} Point;

typedef struct FlatPoint_struct {
    int x;
    int y;
} FlatPoint;

typedef struct DrawnList_struct {
    int x;
    int y;
    DrawnList_struct *next;
} DrawnList;

#define SOLID 0xffff

#endif

Point *point(float x, float y, float z) {
    Point *p = malloc(sizeof(Point));
    assert(p);
    p->x = x;
    p->y = y;
    p->z = z;
    return p;
}

FlatPoint *flatpoint(int x, int y) {
    FlatPoint *fp = malloc(sizeof(FlatPoint));
    assert(fp);
    fp->x = x;
    fp->y = y;
    return fp;
}

int max(int a, int b) { return (a < b) ? b : a; }
int min(int a, int b) { return (a > b) ? b : a; }

int color2ps(COLOR color) {
    switch ((int)color) {
        case gray:  return 1;
        case rgb:   return 3;
        case rgba:  return 4;
        default:    return -1;
    }
}

char *color2str(COLOR color) {
    switch ((int)color) {
        case gray:  return "gray";
        case rgb:   return "rgb";
        case rgba:  return "rgba";
        default:    return NULL;
    }
}

int canvas_size(Canvas *c) {
    return c->width * c->height * color2ps(c->color) * c->depth;
}

void canvas_mkimage(Canvas *c) {

    c->image = calloc(canvas_size(c), sizeof(Pixel));
    assert(c->image);

    c->alpha = calloc(c->width * c->height * c->depth, sizeof(Alpha));
    assert(c->alpha);

}

void canvas_pixel(Canvas *c, FlatPoint *p, Pixel *pixel, Alpha alpha) {
    
    int ps = color2ps(c->color);
    int alpha_index = (p->x * c->width + p->y) * c->depth;
    int pixel_index = alpha_index * ps;

    int pixel_under_size = sizeof(Pixel) * ps * (c->depth - 1);
    Pixel *pixel_under = malloc(pixel_under_size);
    assert(pixel_under);
    
    int alpha_under_size = sizeof(Alpha) * (c->depth - 1);
    Pixel *alpha_under = malloc(alpha_under_size);
    assert(alpha_under);

    if (pixel_index < canvas_size(c) && pixel_index >= 0) {

        // Shift pixels down
        memcpy(pixel_under, &c->image[pixel_index], pixel_under_size);
        memcpy(&c->image[pixel_index + ps], pixel_under, pixel_under_size);
        // Copy new pixel to the top
        memcpy(&c->image[pixel_index], pixel, ps);

        // Shift alpha down
        memcpy(alpha_under, &c->alpha[alpha_index], alpha_under_size);
        memcpy(&c->alpha[alpha_index + 1], alpha_under, alpha_under_size);
        // Copy new alpha to the top
        memcpy(&c->alpha[alpha_index], &alpha, sizeof(Alpha));

    }
    
    free(pixel_under);
    free(alpha_under);

}

static FlatPoint *flatten_point(Canvas *c, Point p) {
    FlatPoint *fp = malloc(sizeof(FlatPoint));
    assert(fp);
    fp->x = c->width / 2 + (p.x - c->width / 2) / (1 + p.z / c->width);
    fp->y = c->height / 2 + (p.y - c->height / 2) / (1 + p.z / c->width);
    return fp;
}

static float dpl(FlatPoint a, FlatPoint b, FlatPoint p) {
    
    float ix, iy;

    float magnitude = hypotf(b.x - a.x, b.y - a.y);
    if (magnitude < 0.000000001) {
        return 1.0 / 0.0;
    }

    float u1 = (((p.x - a.x) * (b.x - a.x)) + ((p.y - a.y) * (b.y - a.y)));
    float u = u1 / (magnitude * magnitude);

    if (u < 0.000000001 || u > 1) {
        // Closest point does not fall within the line segment.
        ix = hypotf(a.x - p.x, a.y - p.y);
        iy = hypotf(b.x - p.x, b.y - p.y);
        return min(ix, iy);
    }

    // Intersecting point is on the line segment.
    ix = a.x + u * (b.x - a.x);
    iy = a.y + u * (b.y - a.y);
    return hypotf(ix - p.x, iy - p.y);

}

DrawnList *already_drawn = NULL;
FlatPoint *line_cap_a;
FlatPoint *line_cap_b;

static void line_builder(Canvas *c, Point *a, Point *b, Pixel *pixel, Alpha alpha) {
    
    if (fabs(a->x - b->x) <= 1 && fabs(a->y - b->y) <= 1 && fabs(a->z - b->z) <= 1) {
        return;
    }

    Point midpoint = {(a->x + b->x) / 2, (a->y + b->y) / 2, (a->z + b->z) / 2};
    int dx, dy;
    FlatPoint *fp;
    DrawnList *check_drawn;
    int was_drawn = 0;
    float dist;

    for (dx = -2; dx <= 2; dx++) {
        for (dy = -2; dy <= 2; dy++) {
            Point p = {midpoint.x + dx, midpoint.y + dy, midpoint.z};
            fp = flatten_point(c, p);

            // Don't draw FlatPoints twice
            for (check_drawn = already_drawn; check_drawn; check_drawn = check_drawn->next) {
                if ((int)fp->x == check_drawn->x && (int)fp->y == check_drawn->y) {
                    was_drawn = 1;
                    break;
                }
            }
            if (was_drawn) {
                was_drawn = 0;
                free(fp);
                continue;
            }
            
            dist = dpl(*line_cap_a, *line_cap_b, *fp);
            if (dist > 1.0) {
                free(fp);
                continue;
            }

            canvas_pixel(c, fp, pixel, alpha * (1.0 - dist));
            
            // Add to list of drawn FlatPoints
            check_drawn = malloc(sizeof(DrawnList));
            check_drawn->x = fp->x;
            check_drawn->y = fp->y;
            check_drawn->next = already_drawn;
            already_drawn = check_drawn;

            free(fp);
        }
    }

    line_builder(c, a, &midpoint, pixel, alpha);
    line_builder(c, &midpoint, b, pixel, alpha);
}

void canvas_line(Canvas *c, Point *a, Point *b, Pixel *pixel, Alpha alpha) {
    line_cap_a = flatten_point(c, *a);
    line_cap_b = flatten_point(c, *b);
    line_builder(c, a, b, pixel, alpha);
    
    // Free / clear list of drawn FlatPoints
    DrawnList *tmp;
    while (already_drawn) {
        tmp = already_drawn->next;
        free(already_drawn);
        already_drawn = tmp;
    }
    free(line_cap_a);
    free(line_cap_b);
}

void canvas_write(Canvas *c, char *filename) {

    int i, d, cc;
    int ps = color2ps(c->color);
    float current_alpha;
    float difference;
    int flat_size = canvas_size(c) / c->depth * sizeof(Pixel);
    Pixel *flat = calloc(flat_size, sizeof(Pixel));
    assert(flat);
    
    // Iterate over pixels
    for (i = 0; i < c->width * c->height; i++) {
        
        // Iterate over alpha layers (backwards)
        for (d = c->depth - 1; d >= 0; d--) {
            
            // Skip transparent pixel layers
            if (!c->alpha[i * c->depth + d]) {
                continue;
            }
            
            current_alpha = (float)(c->alpha[i * c->depth + d]) / 65536.;
            difference = 0;

            // Iterate over color channels
            for (cc = 0; cc < ps; cc++) {

                // Add the difference between the current color and the
                // existing color multiplied by the alpha of the current color
                difference = (float)(c->image[i * ps * c->depth + d + cc]
                    - flat[i * ps + cc]);
                flat[i * ps + cc] = max(0, min(255,
                    flat[i * ps + cc] + (int)(difference * current_alpha)));

            }
        }
    }

    char *args;
    int rtn = asprintf(&args, "convert -depth 8 -size %dx%d %s:- ~/fraxme/c/render/%s.png",
             c->width, c->height, color2str(c->color), filename);
    assert(args);
    assert(rtn != EOF);
    FILE *convert = popen(args, "w");
    fwrite(flat, sizeof(Pixel), flat_size, convert);
    fclose(convert);

    free(flat);
    free(args);
}

void canvas_free(Canvas *c) {
    free(c->image);
    free(c->alpha);
    free(c);
}
