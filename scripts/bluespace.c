#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "canvas.h"
#include "model.h"
#include "vector.h"

#ifndef M_PI
#define M_PI 3.14159265358979323
#endif

#define CUBE_FACES 12
#define ANGLE_COUNT 3

#define FACE(a, b, c) vector_face(vector_point a, vector_point b, vector_point c)

model_t cube(double size)
{
    double s = size / 2;
    model_t cube_front_back = model();

    // front
    model_add(&cube_front_back, FACE((-s, -s, -s), (-s, +s, -s), (+s, -s, -s)));
    model_add(&cube_front_back, FACE((+s, +s, -s), (-s, +s, -s), (+s, -s, -s)));
    // back
    model_add(&cube_front_back, FACE((-s, -s, +s), (-s, +s, +s), (+s, -s, +s)));
    model_add(&cube_front_back, FACE((+s, +s, +s), (-s, +s, +s), (+s, -s, +s)));

    // rotate 90 degrees along X axis
    model_t cube_top_bottom = model_clone(cube_front_back);
    model_rotate(&cube_top_bottom, vector_point(M_PI/2, 0, 0));
    
    // rotate 90 degrees along Y axis
    model_t cube_left_right = model_clone(cube_front_back);
    model_rotate(&cube_left_right, vector_point(0, M_PI/2, 0));
    
    // combine all three models
    int i;
    vector_t *v;
    model_t cube_model = model();
    model_foreach(cube_front_back, i, v) {
        model_add(&cube_model, *v);
    }
    model_foreach(cube_top_bottom, i, v) {
        model_add(&cube_model, *v);
    }
    model_foreach(cube_left_right, i, v) {
        model_add(&cube_model, *v);
    }
    
    // free partial models
    model_free(&cube_front_back);
    model_free(&cube_top_bottom);
    model_free(&cube_left_right);

    return cube_model;
}

int main()
{
    pixel_t size = {540, 640};
    canvas_t c;
    int frame;
    char name[256];
    char filename[256];
    char command[256];

    size_t cube_size = 50;
    size_t frames = 64;

    vector_t rotation_center = vector_point(size.x/2, size.y/2, size.y/2 - cube_size);
    
    for (frame = 0; frame < frames; frame++) {

        c = canvas(size);
        double progress = (double) frame / (double) frames;

        model_t cube_model = cube(cube_size);
        model_translate(&cube_model, vector_point(rotation_center.point.x, cube_size, rotation_center.point.z));
        model_rotate_around(&cube_model, vector_point(progress * M_PI*2, 0, 0), rotation_center);
        
        color_t draw_co = { 0, 0, 0, 255 };
        canvas_draw_model(c, cube_model, draw_co);
        
        sprintf(name, "bluespace%03d", frame);
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

    system("convert -delay 2.5 -dispose Background /home/fraxtil/fraxme/c/render/bluespace*.png /home/fraxtil/fraxme/c/gif/bluespace.gif");
    return 0;
}
