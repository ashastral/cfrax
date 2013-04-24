/* This file was automatically generated.  Do not edit! */
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct Canvas_struct Canvas_struct;
typedef unsigned char Pixel;
typedef unsigned short Alpha;
typedef enum { gray, rgb, rgba }COLOR;
struct Canvas_struct {
    unsigned int width;
    unsigned int height;
    unsigned int depth;
    Pixel *image;
    Alpha *alpha;
    COLOR color;
};
typedef struct Canvas_struct Canvas;
void canvas_free(Canvas *c);
void canvas_write(Canvas *c,char *filename);
typedef struct Point_struct Point_struct;
struct Point_struct {
    float x;
    float y;
    float z;
};
typedef struct Point_struct Point;
void canvas_line(Canvas *c,Point *a,Point *b,Pixel *pixel,Alpha alpha);
typedef struct FlatPoint_struct FlatPoint_struct;
struct FlatPoint_struct {
    int x;
    int y;
};
typedef struct FlatPoint_struct FlatPoint;
extern FlatPoint *line_cap_b;
extern FlatPoint *line_cap_a;
typedef struct DrawnList_struct DrawnList_struct;
struct DrawnList_struct {
    int x;
    int y;
    DrawnList_struct *next;
};
typedef struct DrawnList_struct DrawnList;
extern DrawnList *already_drawn;
void canvas_pixel(Canvas *c,FlatPoint *p,Pixel *pixel,Alpha alpha);
void canvas_mkimage(Canvas *c);
int canvas_size(Canvas *c);
char *color2str(COLOR color);
int color2ps(COLOR color);
int min(int a,int b);
int max(int a,int b);
FlatPoint *flatpoint(int x,int y);
Point *point(float x,float y,float z);
#define SOLID 0xffff
#define INTERFACE 0
