#include <math.h>
#include <stdbool.h>

#include "vector.h"

static vector_t invalid = {VECTOR_INVALID};

vector_type_t vector_typeof(vector_t v) { return v.type; }
bool vector_is_point(vector_t v) { return vector_typeof(v) == VECTOR_POINT; }
bool vector_is_line(vector_t v) { return vector_typeof(v) == VECTOR_LINE; }
bool vector_is_face(vector_t v) { return vector_typeof(v) == VECTOR_FACE; }
bool vector_is_circle(vector_t v) { return vector_typeof(v) == VECTOR_CIRCLE; }

vector_t vector_point(double x, double y, double z)
{
    vector_t v = {VECTOR_POINT, .point={x, y, z}};
    return v;
}

vector_t vector_line(vector_t a, vector_t b)
{
    if (!vector_is_point(a) || !vector_is_point(b)) {
        return invalid;
    }
    vector_t v = {VECTOR_LINE, .line={a.point, b.point}};
    return v;
}

vector_t vector_face(vector_t a, vector_t b, vector_t c)
{
    if (!vector_is_point(a) || !vector_is_point(b) || !vector_is_point(c)) {
        return invalid;
    }
    vector_t v = {VECTOR_FACE, .face={a.point, b.point, c.point}};
    return v;
}

vector_t vector_circle(vector_t p, double r)
{
    if (!vector_is_point(p)) {
        return invalid;
    }
    vector_t v = {VECTOR_CIRCLE, .circle={p.point, r}};
    return v;
}

static void vector_each_point(
        vector_t *v,
        vector_point_t (*func)(vector_point_t, vector_t),
        vector_t other)
{
    switch (vector_typeof(*v)) {
        case VECTOR_INVALID:
            break;
        case VECTOR_POINT:
            v->point = func(v->point, other);
            break;
        case VECTOR_LINE:
            v->line.a = func(v->line.a, other);
            v->line.b = func(v->line.b, other);
            break;
        case VECTOR_FACE:
            v->face.a = func(v->face.a, other);
            v->face.b = func(v->face.b, other);
            v->face.c = func(v->face.c, other);
            break;
        case VECTOR_CIRCLE:
            v->circle.p = func(v->circle.p, other);
            break;
    }
}

static vector_point_t vector_point_scale(vector_point_t p, vector_t scale)
{
    if (vector_is_point(scale)) {
        p.x *= scale.point.x;
        p.y *= scale.point.y;
        p.z *= scale.point.z;
    }
    return p;
}

static vector_point_t vector_point_translate(vector_point_t p, vector_t translate)
{
    if (vector_is_point(translate)) {
        p.x += translate.point.x;
        p.y += translate.point.y;
        p.z += translate.point.z;
    }
    return p;
}

static vector_point_t vector_point_rotate(vector_point_t p, vector_t rotate)
{
    vector_point_t tmp;
    if (vector_is_point(rotate)) {
        // rotate x axis
        tmp.x =  p.x;
        tmp.y =  cos(rotate.point.x) * p.y - sin(rotate.point.x) * p.z;
        tmp.z =  sin(rotate.point.x) * p.y + cos(rotate.point.x) * p.z;
        p = tmp;
        // rotate y axis
        tmp.x =  cos(rotate.point.y) * p.x + sin(rotate.point.y) * p.z;
        tmp.y =  p.y;
        tmp.z = -sin(rotate.point.y) * p.x + cos(rotate.point.y) * p.z;
        p = tmp;
        // rotate z axis
        tmp.x =  cos(rotate.point.z) * p.x - sin(rotate.point.z) * p.y;
        tmp.z =  p.z;
        tmp.y =  sin(rotate.point.z) * p.x + cos(rotate.point.z) * p.y;
        p = tmp;
    }
    return p;
}

static vector_point_t vector_point_negate(vector_point_t p, vector_t unused)
{
    p.x *= -1;
    p.y *= -1;
    p.z *= -1;
    return p;
}

vector_t vector_scale(vector_t v, vector_t scale)
{
    vector_each_point(&v, vector_point_scale, scale);
    return v;
}

vector_t vector_translate(vector_t v, vector_t translate)
{
    vector_each_point(&v, vector_point_translate, translate);
    return v;
}

vector_t vector_rotate(vector_t v, vector_t rotate)
{
    vector_each_point(&v, vector_point_rotate, rotate);
    return v;
}

vector_t vector_negate(vector_t v)
{
    vector_each_point(&v, vector_point_negate, v);
    return v;
}

vector_t vector_rotate_around(vector_t v, vector_t rotate, vector_t center)
{
    // Move center to (0, 0, 0)
    v = vector_translate(v, vector_negate(center));
    // Perform rotation
    v = vector_rotate(v, rotate);
    // Move center back to original position
    v = vector_translate(v, center);
    return v;
}

