#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "canvas.h"

#ifndef M_PI
#define M_PI 3.14159265358979323
#endif

int main()
{
    pixel_t size = {1, 1};
    canvas_t c;
    char name[256];
    char filename[256];
    char command[256];

    c = canvas(size);

    canvas_draw_pixel(c, (pixel_t){0, 0}, 1, (color_t){255, 0, 0, 128});
    canvas_draw_pixel(c, (pixel_t){0, 0}, 2, (color_t){0, 0, 255, 255});
    canvas_draw_pixel(c, (pixel_t){0, 0}, 0, (color_t){0, 255, 0, 128});
    
    sprintf(name, "layers");
    sprintf(filename, "../bitmap/%s.rgba", name);
    sprintf(command, "convert -depth 8 -size %dx%d -scale 10000%% -flatten %s /home/fraxtil/fraxme/c/render/%s.png",
        c.size.x, c.size.y, filename, name);

    FILE *outfile = fopen(filename, "wb");
    canvas_write(c, outfile);
    fclose(outfile);
    canvas_free(&c);
    system(command);

    return 0;
}
