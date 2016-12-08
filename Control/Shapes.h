#ifndef SHAPES_H
#define SHAPES_H

#include "Colour.h"

typedef struct Point 
{
	unsigned short x, y;
} Point;

typedef struct Rect
{
	unsigned short x, y;
	unsigned short width, height;
} Rect;

typedef struct Circle
{
	unsigned short x, y;
	unsigned short radius;
} Circle;

unsigned short doesRectContainPoint(Rect rect, Point point);
unsigned short doesCircleContainPoint(Circle circle, Point point);

#endif
