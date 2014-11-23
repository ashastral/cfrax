#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "canvas.h"
#include "model.h"
#include "vector.h"

#define pow2(X) ((X) * (X))
#define dist2(VX, VY, WX, WY) (pow2((VX) - (WX)) + pow2((VY) - (WY)))
#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

typedef struct {
    double a;
    double b;
    double g;
} barycenter_t;

canvas_t canvas(pixel_t size)
{
    int pixels = size.x * size.y;
    int pixel, layer;

    canvas_t c = {
        .size = size,
        .canvas = malloc(pixels * sizeof(layer_t))
    };

    for (pixel = 0; pixel < pixels; pixel++) {
        for (layer = 0; layer < CANVAS_LAYERS; layer++) {
            c.canvas[pixel].layers[layer].color.a = 0;
            c.canvas[pixel].layers[layer].z = INFINITY;
        }
    }

    return c;
}

static color_t color_merge(color_t top, color_t bottom)
{
    // Shortcuts:
    if (top.a == UCHAR_MAX || bottom.a == 0) {
        return top;
    } else if (top.a == 0) {
        return bottom;
    }

    double top_alpha = (double)top.a / UCHAR_MAX;
    double bottom_alpha = (double)bottom.a / UCHAR_MAX;
    double top_weight = top_alpha;
    double bottom_weight = (1 - top_alpha) * bottom_alpha;
    double total_weight = top_weight + bottom_weight;

    color_t result;
    result.r = (top.r * top_weight + bottom.r * bottom_weight) / total_weight;
    result.g = (top.g * top_weight + bottom.g * bottom_weight) / total_weight;
    result.b = (top.b * top_weight + bottom.b * bottom_weight) / total_weight;
    result.a = 255 * (bottom_alpha + (1 - bottom_alpha) * top_alpha);
    return result;
}

static vector_point_t flatten_point(pixel_t size, vector_point_t p)
{
    // Technically this should return something like pixel_t with doubles
    // instead of ints, but I'm too lazy to create a new type, so we'll just
    // reuse vector_point_t and ignore the z field
    vector_point_t flat = {
        .x = (size.x >> 1) + (p.x - (size.x >> 1)) / (1 + p.z / size.x),
        .y = (size.y >> 1) + (p.y - (size.y >> 1)) / (1 + p.z / size.x),
        .z = 0
    };
    return flat;
}

void canvas_draw_pixel(canvas_t c, pixel_t px, double z, color_t co)
{
    // Don't go out-of-bounds
    if (px.x >= c.size.x || px.y >= c.size.y || px.x < 0 || px.y < 0) {
        return;
    }

    paint_t new = { .color = co, .z = z };

    paint_t *layers = c.canvas[px.y*c.size.x + px.x].layers;
    bool shifting = false;
    int layer;

    for (layer = 0; layer < CANVAS_LAYERS; layer++) {
        if (layers[layer].z > z) {
            shifting = true;
        }
        if (shifting) {
            paint_t tmp = layers[layer];
            layers[layer] = new;
            new = tmp;
        }
    }

    // Merge bottom two colors
    layers[CANVAS_LAYERS - 1].color = color_merge(new.color,
        layers[CANVAS_LAYERS - 1].color);
}

static void canvas_draw_point(canvas_t c, vector_point_t p, color_t co)
{
    vector_point_t flat = flatten_point(c.size, p);
    // Start with the given color as a base and decrease the alpha channel for
    // each pixel covered by the point (depending on how covered it is)
    color_t co_draw = co;
    pixel_t px_draw = {.x = flat.x, .y = flat.y};
    double x_fraction = flat.x - ((int)flat.x);
    double y_fraction = flat.y - ((int)flat.y);
    // Draw upper-left pixel
    co_draw.a = co.a * (1 - x_fraction) * (1 - y_fraction);
    canvas_draw_pixel(c, px_draw, p.z, co_draw);
    // Draw upper-right pixel
    co_draw.a = co.a * x_fraction * (1 - y_fraction);
    px_draw.x++;
    canvas_draw_pixel(c, px_draw, p.z, co_draw);
    // Draw lower-right pixel
    co_draw.a = co.a * x_fraction * y_fraction;
    px_draw.y++;
    canvas_draw_pixel(c, px_draw, p.z, co_draw);
    // Draw lower-left pixel
    co_draw.a = co.a * (1 - x_fraction) * y_fraction;
    px_draw.x--;
    canvas_draw_pixel(c, px_draw, p.z, co_draw);
}

#define fpart(x) fmod(x, 1)
#define rfpart(x) (1 - fpart(x))
static void canvas_draw_line(canvas_t c, vector_line_t l, color_t co)
{
    vector_point_t p1 = flatten_point(c.size, l.a);
    vector_point_t p2 = flatten_point(c.size, l.b);
    color_t co_draw = co;
    pixel_t px_draw;

    double _swap;
    bool steep = fabs(p1.y - p2.y) > fabs(p1.x - p2.x);

    if (steep) {
        _swap = p1.x;
        p1.x = p1.y;
        p1.y = _swap;
        _swap = p2.x;
        p2.x = p2.y;
        p2.y = _swap;
    }
    if (p1.x > p2.x) {
        _swap = p1.x;
        p1.x = p2.x;
        p2.x = _swap;
        _swap = p1.y;
        p1.y = p2.y;
        p2.y = _swap;
        _swap = l.a.z;
        l.a.z = l.b.z;
        l.b.z = _swap;
    }

    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;
    double gradient = dy / dx;

    // handle first endpoint
    double xend = round(p1.x);
    double yend = p1.y + gradient * (xend - p1.x);
    double xgap = rfpart(p1.x + 0.5);
    int xpxl1 = (int)xend;
    int ypxl1 = (int)yend;
    if (steep) {
        px_draw = (pixel_t){ypxl1, xpxl1};
        co_draw.a = co.a * (rfpart(yend) * xgap);
        canvas_draw_pixel(c, px_draw, l.a.z, co_draw);
        px_draw = (pixel_t){ypxl1 + 1, xpxl1};
        co_draw.a = co.a * (fpart(yend) * xgap);
        canvas_draw_pixel(c, px_draw, l.a.z, co_draw);
    } else {
        px_draw = (pixel_t){xpxl1, ypxl1};
        co_draw.a = co.a * (rfpart(yend) * xgap);
        canvas_draw_pixel(c, px_draw, l.a.z, co_draw);
        px_draw = (pixel_t){xpxl1, ypxl1 + 1};
        co_draw.a = co.a * (fpart(yend) * xgap);
        canvas_draw_pixel(c, px_draw, l.a.z, co_draw);
    }
    double intery = yend + gradient;

    // handle second endpoint

    xend = round(p2.x);
    yend = p2.y + gradient * (xend - p2.x);
    xgap = fpart(p2.x + 0.5);
    int xpxl2 = (int)xend;
    int ypxl2 = (int)yend;
    if (steep) {
        px_draw = (pixel_t){ypxl2, xpxl2};
        co_draw.a = co.a * (rfpart(yend) * xgap);
        canvas_draw_pixel(c, px_draw, l.b.z, co_draw);
        px_draw = (pixel_t){ypxl2 + 1, xpxl2};
        co_draw.a = co.a * (fpart(yend) * xgap);
        canvas_draw_pixel(c, px_draw, l.b.z, co_draw);
    } else {
        px_draw = (pixel_t){xpxl2, ypxl2};
        co_draw.a = co.a * (rfpart(yend) * xgap);
        canvas_draw_pixel(c, px_draw, l.b.z, co_draw);
        px_draw = (pixel_t){xpxl2, ypxl2 + 1};
        co_draw.a = co.a * (fpart(yend) * xgap);
        canvas_draw_pixel(c, px_draw, l.b.z, co_draw);
    }

    // main loop
    int x;

    for (x = xpxl1 + 1; x < xpxl2; x++) {
        double z_ratio = ((xpxl2 == xpxl1) ? 0 : (x - xpxl1) / (xpxl2 - xpxl1));
        double z = (1-z_ratio)*l.a.z + z_ratio*l.b.z;
        if (steep) {
            px_draw = (pixel_t){(int)intery, x};
            co_draw.a = co.a * rfpart(intery);
            canvas_draw_pixel(c, px_draw, z, co_draw);
            px_draw = (pixel_t){(int)intery + 1, x};
            co_draw.a = co.a * fpart(intery);
            canvas_draw_pixel(c, px_draw, z, co_draw);
        } else {
            px_draw = (pixel_t){x, (int)intery};
            co_draw.a = co.a * rfpart(intery);
            canvas_draw_pixel(c, px_draw, z, co_draw);
            px_draw = (pixel_t){x, (int)intery + 1};
            co_draw.a = co.a * fpart(intery);
            canvas_draw_pixel(c, px_draw, z, co_draw);
        }
        intery += gradient;
    }

}

static barycenter_t barycenter(
        vector_point_t p1,
        vector_point_t p2,
        vector_point_t p3,
        pixel_t p)
{
    barycenter_t bc;
    bc.a = ((p2.y - p3.y)*(p.x - p3.x) + (p3.x - p2.x)*(p.y - p3.y)) /
        ((p2.y - p3.y)*(p1.x - p3.x) + (p3.x - p2.x)*(p1.y - p3.y));
    bc.b = ((p3.y - p1.y)*(p.x - p3.x) + (p1.x - p3.x)*(p.y - p3.y)) /
        ((p2.y - p3.y)*(p1.x - p3.x) + (p3.x - p2.x)*(p1.y - p3.y));
    bc.g = 1.0 - bc.a - bc.b;
    return bc;
}

static double interpolated(pixel_t size, vector_face_t f, barycenter_t bc)
{
    f.a.z = 1.0 / (f.a.z + size.x);
    f.b.z = 1.0 / (f.b.z + size.x);
    f.c.z = 1.0 / (f.c.z + size.x);
    return 1.0 / (f.a.z + bc.b*(f.b.z-f.a.z) + bc.g*(f.c.z-f.a.z)) - size.x;

}

static void canvas_draw_face(canvas_t c, vector_face_t f, color_t co)
{
    canvas_draw_line(c, (vector_line_t) { f.a, f.b }, co);
    canvas_draw_line(c, (vector_line_t) { f.b, f.c }, co);
    canvas_draw_line(c, (vector_line_t) { f.a, f.c }, co);

    vector_point_t p1 = flatten_point(c.size, f.a);
    vector_point_t p2 = flatten_point(c.size, f.b);
    vector_point_t p3 = flatten_point(c.size, f.c);
    
    pixel_t top_left = {
        .x = min(min(p1.x, p2.x), p3.x),
        .y = min(min(p1.y, p2.y), p3.y),
    };
    pixel_t bottom_right = {
        .x = max(max(p1.x, p2.x), p3.x),
        .y = max(max(p1.y, p2.y), p3.y),
    };
    pixel_t px_draw;
    barycenter_t bc;

    for (px_draw.x = top_left.x; px_draw.x <= bottom_right.x; px_draw.x++) {
        for (px_draw.y = top_left.y; px_draw.y <= bottom_right.y; px_draw.y++) {
            
            bc = barycenter(p1, p2, p3, px_draw);
            if (bc.a < 0 || bc.b < 0 || bc.g < 0) {
                continue;
            }
            canvas_draw_pixel(c, px_draw, interpolated(c.size, f, bc), co);

        }
    }

}

static void canvas_draw_circle(canvas_t c, vector_circle_t ci, color_t co)
{
    vector_point_t p = flatten_point(c.size, ci.p);
    double r = ci.r / (1 + (ci.p.z / c.size.x));
    pixel_t cursor;
    pixel_t cursor_start = {
        .x = (int)(p.x - r),            // Start at the upper-left corner of the
        .y = (int)(p.y - r)             // square enclosing the circle
    };
    pixel_t cursor_end = {
        .x = (int)(p.x + r + 1),        // End at the lower-right corner, naturally
        .y = (int)(p.y + r + 1)
    };
    color_t co_draw = co;               // The given color, but with a modified alpha

    for (cursor.x = cursor_start.x; cursor.x <= cursor_end.x; cursor.x++) {
        for (cursor.y = cursor_start.y; cursor.y <= cursor_end.y; cursor.y++) {
            double a_scale = r - sqrt(dist2(cursor.x, cursor.y, p.x, p.y));
            co_draw.a = (unsigned char)((double)co.a *
                (a_scale > 1 ? 1 : (a_scale < 0 ? 0 : a_scale)));
            canvas_draw_pixel(c, cursor, ci.p.z, co_draw);
        }
    }
}

void canvas_draw_vector(canvas_t c, vector_t v, color_t co)
{
    switch (vector_typeof(v)) {
        case VECTOR_INVALID:
            break;
        case VECTOR_POINT:
            canvas_draw_point(c, v.point, co);
            break;
        case VECTOR_LINE:
            canvas_draw_line(c, v.line, co);
            break;
        case VECTOR_FACE:
            canvas_draw_face(c, v.face, co);
            break;
        case VECTOR_CIRCLE:
            canvas_draw_circle(c, v.circle, co);
            break;
    }
}

void canvas_draw_model(canvas_t c, model_t m, color_t co)
{
    int v;
    for (v = 0; v < m.count; v++) {
        canvas_draw_vector(c, m.vectors[v], co);
    }
}

void canvas_write(canvas_t c, FILE *file)
{
    int pixel, layer, bitmap_index = 0;
    int total_pixels = c.size.x * c.size.y;
    int total_bytes = 4 * total_pixels;
    unsigned char *bitmap = malloc(total_bytes);

    for (pixel = 0; pixel < total_pixels; pixel++) {
        for (layer = CANVAS_LAYERS - 1; layer > 0; layer--) {
            c.canvas[pixel].layers[layer - 1].color = color_merge(
                c.canvas[pixel].layers[layer - 1].color,
                c.canvas[pixel].layers[layer].color);
        }
        bitmap[bitmap_index++] = c.canvas[pixel].layers[0].color.r;
        bitmap[bitmap_index++] = c.canvas[pixel].layers[0].color.g;
        bitmap[bitmap_index++] = c.canvas[pixel].layers[0].color.b;
        bitmap[bitmap_index++] = c.canvas[pixel].layers[0].color.a;
    }

    fwrite(bitmap, 1, total_bytes, file);
    free(bitmap);
}

void canvas_free(canvas_t *c)
{
    free(c->canvas);
    c->canvas = NULL;
}
