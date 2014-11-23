#include <stdio.h>

#include "canvas.h"
#include "vector.h"
#include "verify.h"

int test_canvas_line_bug()
{
    vector_t p1 = vector_point(93.75, 212.42339723910436, 156.25);
    vector_t p2 = vector_point(93.75, 254.29642433524367, 187.5);
    pixel_t size = {500, 500};
    canvas_t c = canvas(size);
    color_t co = {0, 0, 0, 255};
    canvas_draw_vector(c, vector_line(p1, p2), co);
    verify_return();
}

int test_canvas()
{
    verify_call(test_canvas_line_bug());
    verify_return();
}
