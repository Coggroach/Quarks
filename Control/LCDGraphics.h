#ifndef LCDGRAPHICS_H
#define LCDGRAPHICS_H

#include "FreeRTOS.h"
#include "Graphics.h"
#include "Components.h"
#include "queue.h"

#define NumberOfButtons 5
#define NumberOfInterfaces 2
#define NumberOfButtonsInInterface 2
#define NumberOfInterfacesInRow 2

typedef struct LedInterface
{
	Slider slider;
	Button buttons[NumberOfButtonsInInterface];	
} LedInterface;

typedef struct LedScreen
{
	Button buttons[NumberOfButtons];	
	LedInterface interfaces[NumberOfInterfaces];	
} LedScreen;

void drawLedScreen(void);
void drawButtonPointer(Button* b);
void drawSliderPointer(Slider* b);
void handleButton(Button *b);
void handleSlider(Slider *s, Point p);
void handleInterface(LedInterface *i, int id, xQueueHandle xQueue, Point p);
int isComponentTouched(unsigned int x, unsigned int y);
void setupLedScreen(void);
Slider* getSlider(int id);
Button* getButton(int id);
Point getPoint(unsigned int x, unsigned int y);

#endif
