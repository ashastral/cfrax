#ifndef CANVAS_H
#define CANVAS_H

#include "model.h"
#include "vector.h"

#define CANVAS_LAYERS 8

typedef struct {
    int x;
    int y;
} pixel_t;

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} color_t;

typedef struct {
    color_t color;
    double z;
} paint_t;

typedef struct {
    paint_t layers[CANVAS_LAYERS];
} layer_t;

typedef struct {
    pixel_t size;
    layer_t *canvas;
} canvas_t;

canvas_t canvas(pixel_t size);
void canvas_draw_pixel(canvas_t c, pixel_t px, double z, color_t co);
void canvas_draw_vector(canvas_t c, vector_t v, color_t co);
void canvas_draw_model(canvas_t c, model_t m, color_t co);
void canvas_write(canvas_t c, FILE *file);
void canvas_free(canvas_t *c);

#endif // CANVAS_H
