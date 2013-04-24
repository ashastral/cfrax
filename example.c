#include <stdio.h>
#include <math.h>

#include "example.h"
#include "cfrax.h"

int main(int argc, char *argv[]) {
    
    Canvas *c = malloc(sizeof(Canvas));
    assert(c);
    c->width = 500;
    c->height = 500;
    c->depth = 2;
    c->color = rgb;
    canvas_mkimage(c);

    Pixel *white = (unsigned char *)"\xff\x80\x00";
    canvas_line(c, point(100, 100, 0), point(200, 300, 0), white, 0xffff);
    canvas_line(c, point(100, 300, 0), point(200, 100, 100), white, 0xffff);

    canvas_write(c, "filename");

    canvas_free(c);

    return 0;
}
