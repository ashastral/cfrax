#ifndef VECTOR_H
#define VECTOR_H

#include <stdbool.h>

typedef struct {
    double x;
    double y;
    double z;
} vector_point_t;

typedef struct {
    vector_point_t a;
    vector_point_t b;
} vector_line_t;

typedef struct {
    vector_point_t a;
    vector_point_t b;
    vector_point_t c;
} vector_face_t;

typedef struct {
    vector_point_t p;
    double r;
} vector_circle_t;

typedef enum {
    VECTOR_INVALID,
    VECTOR_POINT,
    VECTOR_LINE,
    VECTOR_FACE,
    VECTOR_CIRCLE,
} vector_type_t;

typedef struct {
    vector_type_t type;
    union {
        vector_point_t point;
        vector_line_t line;
        vector_face_t face;
        vector_circle_t circle;
    };
} vector_t;

vector_type_t vector_typeof(vector_t v);
bool vector_is_point(vector_t v);
bool vector_is_line(vector_t v);
bool vector_is_face(vector_t v);
bool vector_is_circle(vector_t v);
vector_t vector_point(double x, double y, double z);
vector_t vector_line(vector_t a, vector_t b);
vector_t vector_face(vector_t a, vector_t b, vector_t c);
vector_t vector_circle(vector_t p, double r);
vector_t vector_scale(vector_t v, vector_t scale);
vector_t vector_translate(vector_t v, vector_t translate);
vector_t vector_rotate(vector_t v, vector_t rotate);
vector_t vector_negate(vector_t v);
vector_t vector_rotate_around(vector_t v, vector_t rotate, vector_t center);

#ifdef USING_NAMESPACE_VECTOR
#define vector_point point
#define vector_line line
#define vector_face face
#define vector_circle circle
#define vector_scale scale
#define vector_translate translate
#define vector_rotate rotate
#define vector_negate negate
#define vector_rotate_around rotate_around
#endif

#endif // VECTOR_H
