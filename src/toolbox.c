#include <stdio.h>
#include <stdlib.h>

#include "canvas.h"
#include "toolbox.h"

static const int BUFSIZE = 256;
static const char *toolbox_name = NULL;
static const char *render_command = "convert -depth 8 -size %dx%d %s render/%s.png";
static const char *gif_command = "convert -delay 2.5 %s render/%s*.png gif/%s.gif";

void toolbox_set_name(const char *name)
{
    toolbox_name = name;
}

void toolbox_render(canvas_t c, int frame)
{
    char frame_name[BUFSIZE];
    char filename[BUFSIZE];
    char command[BUFSIZE * 2];
    snprintf(frame_name, BUFSIZE, "%s%d", toolbox_name, frame);
    snprintf(filename, BUFSIZE, "../bitmap/%s.rgba", frame_name);
    snprintf(command, BUFSIZE * 2, render_command, c.size.x, c.size.y, filename, frame_name);

    FILE *outfile = fopen(filename, "wb");
    canvas_write(c, outfile);
    fclose(outfile);
    system(command);
}

void toolbox_gif(const char *options)
{
    char command[BUFSIZE * 2];
    snprintf(command, BUFSIZE * 2, gif_command, options, toolbox_name, toolbox_name);
    system(command);
}
