#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "Shapes.h"

typedef enum ButtonState
{
	Off = 0, 
	On = 1, 
	Pressed = 2,
	OnAndPressed = 3
} ButtonState;

typedef enum SlideMode
{
	Horizontal = 0,
	Vertical = 1	
} SlideMode;

typedef struct Button
{
	unsigned short id;
	Rect rect;
	ButtonState state;
} Button;

typedef struct Slider
{
	unsigned short id;
	Rect rect;
	unsigned short sPos;
	unsigned short sSize;
	SlideMode mode;
} Slider;

#endif
