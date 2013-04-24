/* This file was automatically generated.  Do not edit! */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct Canvas_struct Canvas_struct;
typedef unsigned short Alpha;
typedef enum { gray, rgb, rgba }COLOR;
struct Canvas_struct {
    unsigned int width;
    unsigned int height;
    unsigned int depth;
    unsigned char *image;
    Alpha *alpha;
    COLOR color;
};
typedef struct Canvas_struct Canvas;
void canvas_write(Canvas *c,char *filename);
void canvas_pixel(Canvas *c,int x,int y,unsigned char *pixel,Alpha alpha);
void canvas_mkimage(Canvas *c);
int canvas_size(Canvas *c);
char *color2str(COLOR color);
int color2ps(COLOR color);
int min(int a,int b);
int max(int a,int b);
#define INTERFACE 0
