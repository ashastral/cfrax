#define _GNU_SOURCE

#if INTERFACE

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { gray, rgb, rgba } COLOR;

typedef unsigned short Alpha;

typedef struct Canvas_struct {
    unsigned int width;
    unsigned int height;
    unsigned int depth;
    unsigned char *image;
    Alpha *alpha;
    COLOR color;
} Canvas;

#endif

#include "cfrax.h"

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

    c->image = calloc(canvas_size(c), sizeof(unsigned char));
    assert(c->image);

    c->alpha = calloc(c->width * c->height * c->depth, sizeof(Alpha));
    assert(c->alpha);

}

void canvas_pixel(Canvas *c, int x, int y, unsigned char *pixel, Alpha alpha) {
    
    int ps = color2ps(c->color);
    int alpha_index = (x * c->width + y) * c->depth;
    int pixel_index = alpha_index * ps;

    int pixel_under_size = sizeof(unsigned char) * ps * (c->depth - 1);
    unsigned char *pixel_under = malloc(pixel_under_size);
    assert(pixel_under);
    
    int alpha_under_size = sizeof(Alpha) * (c->depth - 1);
    unsigned char *alpha_under = malloc(alpha_under_size);
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

}

void canvas_write(Canvas *c, char *filename) {

    int i, d, cc;
    int ps = color2ps(c->color);
    float current_alpha;
    float difference;
    int flat_size = canvas_size(c) / c->depth * sizeof(unsigned char);
    unsigned char *flat = calloc(flat_size, sizeof(unsigned char));
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
    fwrite(flat, sizeof(char), flat_size, convert);
    fclose(convert);
}
