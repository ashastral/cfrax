#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "canvas.h"
#include "toolbox.h"
#include "vector.h"

#define pow2(X) ((X) * (X))
#define dist2(VX, VY, WX, WY) (pow2((VX) - (WX)) + pow2((VY) - (WY)))

int noise_min_width;
color_t co = {0, 0, 0, 255};

int noise(
        canvas_t c,
        vector_t front_left,
        vector_t front_right,
        vector_t back_left,
        vector_t back_right)
{
    double width = sqrt(dist2(front_left.point.x, front_left.point.z,
                              back_right.point.x, back_right.point.z));
    
    if (width <= noise_min_width) {
        //canvas_draw_vector(c, vector_line(front_left, front_right), co);
        canvas_draw_vector(c, vector_line(front_left, back_left), co);
        canvas_draw_vector(c, vector_line(back_right, front_right), co);
        canvas_draw_vector(c, vector_line(back_right, back_left), co);
        return 0;
    }

    vector_t midpoint = vector_point(
        (front_left.point.x + back_right.point.x) / 2,
        (front_left.point.y + front_right.point.y + back_left.point.y + back_right.point.y) / 4,
        (front_left.point.z + back_right.point.z) / 2
    );
    midpoint.point.y += (rand() % (int)width) - (width / 2);
    
    canvas_draw_vector(c, vector_circle(midpoint, 2), co);

    vector_t front_center = vector_point(midpoint.point.x, (front_left.point.y + front_right.point.y) / 2, front_left.point.z);
    vector_t back_center = vector_point(midpoint.point.x, (back_left.point.y + back_right.point.y) / 2, back_left.point.z);
    vector_t mid_left = vector_point(front_left.point.x, (front_left.point.y + back_left.point.y) / 2, midpoint.point.z);
    vector_t mid_right = vector_point(front_right.point.x, (front_right.point.y + back_right.point.y) / 2, midpoint.point.z);
    noise(c, front_left, front_center, mid_left, midpoint);
    noise(c, midpoint, front_right, back_center, back_right);
    noise(c, mid_left, midpoint, back_left, back_center);
    noise(c, front_center, front_right, midpoint, mid_right);
    return 0;
}

int main()
{
    unsigned t = time(NULL);
    pixel_t size = {500, 500};
    canvas_t c;
    int frame;

    toolbox_set_name("noise");

    for (frame = 0, noise_min_width = 100; noise_min_width > 2; noise_min_width >>= 1, frame++) {
        
        c = canvas(size);
        srand(t);
        
        noise(c, vector_point(0, 250, 0), vector_point(500, 250, 0),
                 vector_point(0, 250, 500), vector_point(500, 250, 500));

        toolbox_render(c, frame);
        canvas_free(&c);
    }

    toolbox_gif("");
    return 0;
}
