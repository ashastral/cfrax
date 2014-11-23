#include <stdio.h>

#include "canvas.h"
#include "vector.h"

int main(int argc, char *argv[])
{
    pixel_t size = {10, 10};
    canvas_t c = canvas(size);
    color_t co = {255, 0, 0, 255};
    canvas_draw_vector(c, vector_line(vector_point(1, 1, 0), vector_point(9, 7, 0)), co);
    FILE *outfile = fopen("../bitmap/a_line.rgba", "wb");
    canvas_write(c, outfile);
    fclose(outfile);
    canvas_free(&c);
    return 0;
}
