#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "Shapes.h"

typedef enum ComponentType
{
	ButtonType = 0,
	SliderType = 1,
	LightType = 2,
	PresetType = 3
} ComponentType;

typedef enum ButtonState
{
	Off = 0x0, 
	On = 0x1, 
	Dim = 0x2, 
	Pressed = 0x4
} ButtonState;

typedef enum SlideMode
{
	Horizontal = 0,
	Vertical = 1	
} SlideMode;

typedef struct Light
{
	unsigned short id;
	Rect rect;
	unsigned char brightness;
	ButtonState state;
} Light;

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
