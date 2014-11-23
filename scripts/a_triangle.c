#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "canvas.h"

#ifndef M_PI
#define M_PI 3.14159265358979323
#endif

int main()
{
    pixel_t size = {50, 50};
    color_t co = {0, 0, 0, 255};
    canvas_t c;
    int frame;
    char name[256];
    char filename[256];
    char command[256];

    size_t frames = 32;

    for (frame = 0; frame < frames; frame++) {
        
        double angle = (double)frame / (double)frames * M_PI * 2 / 3;
        c = canvas(size);
        
        canvas_draw_vector(c, vector_face(
            vector_point(25 + 20*sin(angle),          25 + 20*cos(angle),          0),
            vector_point(25 + 20*sin(angle+M_PI*2/3), 25 + 20*cos(angle+M_PI*2/3), 0),
            vector_point(25 + 20*sin(angle+M_PI*4/3), 25 + 20*cos(angle+M_PI*4/3), 0)),
            co);
        
        sprintf(name, "too_many_fucking_triangles%02d", frame);
        sprintf(filename, "../bitmap/%s.rgba", name);
        sprintf(command, "convert -depth 8 -size %dx%d -flatten %s /home/fraxtil/fraxme/c/render/%s.png",
            c.size.x, c.size.y, filename, name);

        FILE *outfile = fopen(filename, "wb");
        canvas_write(c, outfile);
        fclose(outfile);
        canvas_free(&c);
        system(command);
    }

    system("convert -delay 2.5 -dispose Background /home/fraxtil/fraxme/c/render/too_many_fucking_triangles*.png /home/fraxtil/fraxme/c/gif/too_many_fucking_triangles.gif");
    return 0;
}
