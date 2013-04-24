#include <stdio.h>

#include "example.h"
#include "cfrax.h"

int main(int argc, char *argv[]) {
    
    Canvas *c = malloc(sizeof(Canvas));
    assert(c);
    c->width = 500;
    c->height = 500;
    c->depth = 2;
    c->color = gray;
    canvas_mkimage(c);

    canvas_pixel(c, 20, 20, "\xff", 0x8000);
    canvas_pixel(c, 20, 20, "\xff", 0x8000);
    canvas_write(c, "filename");

    return 0;
}
