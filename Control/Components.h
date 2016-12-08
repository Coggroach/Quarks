#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "Shapes.h"

typedef struct Button
{
	Rect rect;
	unsigned short value;
} Button;

typedef struct Slider
{
	Rect rect;
	unsigned short sPos;
	unsigned short sSize;
	unsigned short mode;
} Slider;

#endif
