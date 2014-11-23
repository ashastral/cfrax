#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "canvas.h"

int main()
{
    unsigned t = time(NULL);
    pixel_t size = {50, 50};
    color_t co = {0, 0, 0, 255};
    canvas_t c;
    int frame;
    char name[256];
    char filename[256];
    char command[256];

    for (frame = 0; frame < 10; frame++) {
        
        c = canvas(size);
        srand(t);
        
        canvas_draw_vector(c, vector_circle(vector_point(25, 25, 0), frame), co);
        
        sprintf(name, "a_circle%d", frame);
        sprintf(filename, "../bitmap/%s.rgba", name);
        sprintf(command, "convert -depth 8 -size %dx%d %s /home/fraxtil/fraxme/c/render/%s.png",
            c.size.x, c.size.y, filename, name);

        FILE *outfile = fopen(filename, "wb");
        canvas_write(c, outfile);
        fclose(outfile);
        canvas_free(&c);
        system(command);
    }

    system("convert -delay 2.5 /home/fraxtil/fraxme/c/render/a_circle*.png /home/fraxtil/fraxme/c/gif/a_circle.gif");
    return 0;
}
