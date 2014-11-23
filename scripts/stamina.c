#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "canvas.h"

#ifndef M_PI
#define M_PI 3.14159265358979323
#endif

#define TRIANGLE_FACES 6
#define ANGLE_COUNT 3

typedef struct {
    vector_t faces[TRIANGLE_FACES];
} open_triangle_t;

open_triangle_t open_triangle(double progress)
{
    static const double angles[ANGLE_COUNT] = { 0, M_PI * 2/3, M_PI * 4/3 };

    open_triangle_t triangle;
    double angle;
    size_t a;

    for (a = 0, angle = angles[0]; a < ANGLE_COUNT; angle=angles[++a]) {
        double next_angle = angles[(a + 1) % ANGLE_COUNT];
        triangle.faces[2*a] = vector_face(
            vector_point(sin(angle), cos(angle), 0),
            vector_point(progress * sin(angle), progress * cos(angle), 0),
            vector_point(sin(next_angle), cos(next_angle), 0));
        triangle.faces[2*a+1] = vector_face(
            vector_point(sin(next_angle), cos(next_angle), 0),
            vector_point(progress * sin(next_angle), progress * cos(next_angle), 0),
            vector_point(progress * sin(angle), progress * cos(angle), 0));
    }

    return triangle;
}

typedef struct {
    size_t tframe;
    pixel_t center;
} tri_t;

int main()
{
    pixel_t size = {250, 250};
    canvas_t c;
    int frame;
    char name[256];
    char filename[256];
    char command[256];

    size_t frames = 128;
    size_t triangle_count = 16;
    
    // Set up triangle array
    pixel_t triangles[triangle_count];
    // Central triangle
    triangles[0] = (pixel_t) { size.x/2, size.y/2 };
    // Other triangles
    size_t t;
    for (t = 1; t < triangle_count; t++) {
        triangles[t] = (pixel_t) {
            (double)rand() / RAND_MAX * size.x,
            (double)rand() / RAND_MAX * size.y,
        };
    }

    for (frame = 0; frame < frames; frame++) {

        c = canvas(size);

        for (t = 0; t < triangle_count; t++) {

            double progress = (double)((frame + t * (frames / triangle_count)) % frames) / (double)frames + 0.001;

            open_triangle_t triangle = open_triangle(progress);
            size_t triangle_face;
            for (triangle_face = 0; triangle_face < TRIANGLE_FACES; triangle_face++) {
                vector_t draw_vector = vector_translate(
                    vector_scale(vector_rotate(triangle.faces[triangle_face],
                                               vector_point(0, 0, progress * M_PI * 2/3)),
                                 vector_point(size.x * .2, size.y * .2, 1)),
                    vector_point(triangles[t].x,
                                 triangles[t].y,
                                 size.x * (3 - progress * 4)));
                unsigned char shade = UCHAR_MAX * (1 - progress);
                color_t draw_co = { shade, shade, shade, 255 };
                canvas_draw_vector(c, draw_vector, draw_co);
            }

        }
        
        sprintf(name, "stamina%03d", frame);
        sprintf(filename, "../bitmap/%s.rgba", name);
        sprintf(command, "convert -depth 8 -size %dx%d -flatten %s /home/fraxtil/fraxme/c/render/%s.png",
            c.size.x, c.size.y, filename, name);

        FILE *outfile = fopen(filename, "wb");
        canvas_write(c, outfile);
        fclose(outfile);
        canvas_free(&c);
        system(command);

        fprintf(stderr, "\r%d", frame);
    }

    fprintf(stderr, "making GIF...");

    system("convert -delay 2.5 -dispose Background /home/fraxtil/fraxme/c/render/stamina*.png /home/fraxtil/fraxme/c/gif/stamina.gif");
    return 0;
}
