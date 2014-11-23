#include <stdio.h>

#include "verify.h"
#include "vector.h"

static int test_vector_typeof()
{
    vector_t p1 = vector_point(1, 2, 3);
    vector_t p2 = vector_point(40, 50, 60);
    vector_t p3 = vector_point(700, 800, 900);
    vector_t l1 = vector_line(p1, p2);
    vector_t f1 = vector_face(p1, p2, p3);
    vector_t i1 = vector_line(l1, f1);
    
    verify_bare(vector_typeof(p1) == VECTOR_POINT);
    verify_bare(vector_typeof(p2) == VECTOR_POINT);
    verify_bare(vector_typeof(p3) == VECTOR_POINT);
    verify_bare(vector_typeof(l1) == VECTOR_LINE);
    verify_bare(vector_typeof(f1) == VECTOR_FACE);
    verify_bare(vector_typeof(i1) == VECTOR_INVALID);

    verify_return();
}

static int test_vector_is_point()
{
    vector_t p1 = vector_point(1, 2, 3);
    vector_t p2 = vector_point(40, 50, 60);
    vector_t p3 = vector_point(700, 800, 900);
    vector_t l1 = vector_line(p1, p2);
    vector_t f1 = vector_face(p1, p2, p3);
    
    verify_bare(vector_is_point(p1));
    verify_bare(vector_is_point(p2));
    verify_bare(vector_is_point(p3));
    verify_bare(!vector_is_point(l1));
    verify_bare(!vector_is_point(f1));
    
    verify_return();
}

static int test_vector_is_line()
{
    vector_t p1 = vector_point(1, 2, 3);
    vector_t p2 = vector_point(40, 50, 60);
    vector_t p3 = vector_point(700, 800, 900);
    vector_t l1 = vector_line(p1, p2);
    vector_t f1 = vector_face(p1, p2, p3);
    
    verify_bare(!vector_is_line(p1));
    verify_bare(!vector_is_line(p2));
    verify_bare(!vector_is_line(p3));
    verify_bare(vector_is_line(l1));
    verify_bare(!vector_is_line(f1));
    
    verify_return();
}

static int test_vector_is_face()
{
    vector_t p1 = vector_point(1, 2, 3);
    vector_t p2 = vector_point(40, 50, 60);
    vector_t p3 = vector_point(700, 800, 900);
    vector_t l1 = vector_line(p1, p2);
    vector_t f1 = vector_face(p1, p2, p3);
    
    verify_bare(!vector_is_face(p1));
    verify_bare(!vector_is_face(p2));
    verify_bare(!vector_is_face(p3));
    verify_bare(!vector_is_face(l1));
    verify_bare(vector_is_face(f1));
    
    verify_return();
}

static int test_vector_point()
{
    double x = 3.14, y = 2.72, z = -1.11111;
    vector_t p1 = vector_point(x, y, z);
    
    verify_bare(vector_is_point(p1));
    verify_bare(p1.point.x == x);
    verify_bare(p1.point.y == y);
    verify_bare(p1.point.z == z);
    
    verify_return();
}

static int test_vector_line()
{
    double x1 = 3.14, y1 = 2.72, z1 = -1.11111;
    double x2 = 123.4, y2 = 567.8, z2 = 999.9; 
    vector_t p1 = vector_point(x1, y1, z1);
    vector_t p2 = vector_point(x2, y2, z2);
    vector_t l1 = vector_line(p1, p2);
    vector_t i1 = vector_line(l1, p1);
    vector_t i2 = vector_line(p1, l1);
    vector_t i3 = vector_line(l1, l1);

    verify_bare(vector_is_line(l1));
    verify_bare(l1.line.a.x == x1);
    verify_bare(l1.line.a.y == y1);
    verify_bare(l1.line.a.z == z1);
    verify_bare(l1.line.b.x == x2);
    verify_bare(l1.line.b.y == y2);
    verify_bare(l1.line.b.z == z2);
    verify_bare(!vector_is_line(i1));
    verify_bare(!vector_is_line(i2));
    verify_bare(!vector_is_line(i3));

    verify_return();
}

static int test_vector_face()
{
    double x1 = 3.14, y1 = 2.72, z1 = -1.11111;
    double x2 = 123.4, y2 = 567.8, z2 = 999.9; 
    double x3 = 98765, y3 = 43210, z3 = 13579; 
    vector_t p1 = vector_point(x1, y1, z1);
    vector_t p2 = vector_point(x2, y2, z2);
    vector_t p3 = vector_point(x3, y3, z3);
    vector_t f1 = vector_face(p1, p2, p3);
    vector_t i1 = vector_face(f1, p2, p3);
    vector_t i2 = vector_face(p1, f1, p3);
    vector_t i3 = vector_face(p1, p2, f1);
    vector_t i4 = vector_face(p1, f1, f1);
    vector_t i5 = vector_face(f1, p2, f1);
    vector_t i6 = vector_face(f1, f1, p3);
    vector_t i7 = vector_face(f1, f1, f1);

    verify_bare(vector_is_face(f1));
    verify_bare(f1.face.a.x == x1);
    verify_bare(f1.face.a.y == y1);
    verify_bare(f1.face.a.z == z1);
    verify_bare(f1.face.b.x == x2);
    verify_bare(f1.face.b.y == y2);
    verify_bare(f1.face.b.z == z2);
    verify_bare(f1.face.c.x == x3);
    verify_bare(f1.face.c.y == y3);
    verify_bare(f1.face.c.z == z3);
    verify_bare(!vector_is_face(i1));
    verify_bare(!vector_is_face(i2));
    verify_bare(!vector_is_face(i3));
    verify_bare(!vector_is_face(i4));
    verify_bare(!vector_is_face(i5));
    verify_bare(!vector_is_face(i6));
    verify_bare(!vector_is_face(i7));

    verify_return();
}

int test_vector_scale()
{
    vector_t p1 = vector_point(1, 2, 3);
    vector_t p2 = vector_point(40, 50, 60);
    vector_t p3 = vector_point(700, 800, 900);
    vector_t l1 = vector_line(p1, p2);
    vector_t f1 = vector_face(p1, p2, p3);
    vector_t i1 = vector_line(l1, f1);
    vector_t scale_double = vector_point(2, 2, 2);
    vector_t scale_half = vector_point(.5, .5, .5);
    vector_t scale_independent = vector_point(1, 2, 3);
    vector_t p1_double = vector_scale(p1, scale_double);
    vector_t p1_half = vector_scale(p1, scale_half);
    vector_t p1_independent = vector_scale(p1, scale_independent);
    vector_t l1_double = vector_scale(l1, scale_double);
    vector_t l1_half = vector_scale(l1, scale_half);
    vector_t l1_independent = vector_scale(l1, scale_independent);
    vector_t f1_double = vector_scale(f1, scale_double);
    vector_t f1_half = vector_scale(f1, scale_half);
    vector_t f1_independent = vector_scale(f1, scale_independent);
    vector_t i1_double = vector_scale(i1, scale_double);
    vector_t i1_half = vector_scale(i1, scale_half);
    vector_t i1_independent = vector_scale(i1, scale_independent);

    verify_bare(vector_is_point(p1_double));
    verify_bare(vector_is_point(p1_half));
    verify_bare(vector_is_point(p1_independent));
    verify_bare(vector_is_line(l1_double));
    verify_bare(vector_is_line(l1_half));
    verify_bare(vector_is_line(l1_independent));
    verify_bare(vector_is_face(f1_double));
    verify_bare(vector_is_face(f1_half));
    verify_bare(vector_is_face(f1_independent));
    verify_bare(vector_typeof(i1_double) == VECTOR_INVALID);
    verify_bare(vector_typeof(i1_half) == VECTOR_INVALID);
    verify_bare(vector_typeof(i1_independent) == VECTOR_INVALID);
    
    verify_bare(p1_double.point.x == p1.point.x * 2);
    verify_bare(p1_double.point.y == p1.point.y * 2);
    verify_bare(p1_double.point.z == p1.point.z * 2);
    verify_bare(p1_half.point.x == p1.point.x / 2);
    verify_bare(p1_half.point.y == p1.point.y / 2);
    verify_bare(p1_half.point.z == p1.point.z / 2);
    verify_bare(p1_independent.point.x == p1.point.x * 1);
    verify_bare(p1_independent.point.y == p1.point.y * 2);
    verify_bare(p1_independent.point.z == p1.point.z * 3);
    
    verify_bare(l1_double.line.a.x == p1.point.x * 2);
    verify_bare(l1_double.line.a.y == p1.point.y * 2);
    verify_bare(l1_double.line.a.z == p1.point.z * 2);
    verify_bare(l1_half.line.a.x == p1.point.x / 2);
    verify_bare(l1_half.line.a.y == p1.point.y / 2);
    verify_bare(l1_half.line.a.z == p1.point.z / 2);
    verify_bare(l1_independent.line.a.x == p1.point.x * 1);
    verify_bare(l1_independent.line.a.y == p1.point.y * 2);
    verify_bare(l1_independent.line.a.z == p1.point.z * 3);
    verify_bare(l1_double.line.b.x == p2.point.x * 2);
    verify_bare(l1_double.line.b.y == p2.point.y * 2);
    verify_bare(l1_double.line.b.z == p2.point.z * 2);
    verify_bare(l1_half.line.b.x == p2.point.x / 2);
    verify_bare(l1_half.line.b.y == p2.point.y / 2);
    verify_bare(l1_half.line.b.z == p2.point.z / 2);
    verify_bare(l1_independent.line.b.x == p2.point.x * 1);
    verify_bare(l1_independent.line.b.y == p2.point.y * 2);
    verify_bare(l1_independent.line.b.z == p2.point.z * 3);
    
    verify_bare(f1_double.face.a.x == p1.point.x * 2);
    verify_bare(f1_double.face.a.y == p1.point.y * 2);
    verify_bare(f1_double.face.a.z == p1.point.z * 2);
    verify_bare(f1_half.face.a.x == p1.point.x / 2);
    verify_bare(f1_half.face.a.y == p1.point.y / 2);
    verify_bare(f1_half.face.a.z == p1.point.z / 2);
    verify_bare(f1_independent.face.a.x == p1.point.x * 1);
    verify_bare(f1_independent.face.a.y == p1.point.y * 2);
    verify_bare(f1_independent.face.a.z == p1.point.z * 3);
    verify_bare(f1_double.face.b.x == p2.point.x * 2);
    verify_bare(f1_double.face.b.y == p2.point.y * 2);
    verify_bare(f1_double.face.b.z == p2.point.z * 2);
    verify_bare(f1_half.face.b.x == p2.point.x / 2);
    verify_bare(f1_half.face.b.y == p2.point.y / 2);
    verify_bare(f1_half.face.b.z == p2.point.z / 2);
    verify_bare(f1_independent.face.b.x == p2.point.x * 1);
    verify_bare(f1_independent.face.b.y == p2.point.y * 2);
    verify_bare(f1_independent.face.b.z == p2.point.z * 3);
    verify_bare(f1_double.face.c.x == p3.point.x * 2);
    verify_bare(f1_double.face.c.y == p3.point.y * 2);
    verify_bare(f1_double.face.c.z == p3.point.z * 2);
    verify_bare(f1_half.face.c.x == p3.point.x / 2);
    verify_bare(f1_half.face.c.y == p3.point.y / 2);
    verify_bare(f1_half.face.c.z == p3.point.z / 2);
    verify_bare(f1_independent.face.c.x == p3.point.x * 1);
    verify_bare(f1_independent.face.c.y == p3.point.y * 2);
    verify_bare(f1_independent.face.c.z == p3.point.z * 3);

    verify_return();
}

int test_vector()
{
    verify_call(test_vector_typeof());
    verify_call(test_vector_is_point());
    verify_call(test_vector_is_line());
    verify_call(test_vector_is_face());
    verify_call(test_vector_point());
    verify_call(test_vector_line());
    verify_call(test_vector_face());
    verify_call(test_vector_scale());
//    verify_call(test_vector_translate());
//    verify_call(test_vector_rotate());
//    verify_call(test_vector_negate());
//    verify_call(test_vector_rotate_around());
    verify_return();
}
