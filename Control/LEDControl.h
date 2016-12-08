#ifndef LEDCONTROL_H
#define LEDCONTROL_H

#include "Components.h"
#include "queue.h"

#define NumberOfButtons 5
#define NumberOfInterfaces 4
#define NumberOfInterfacesInRow 2

typedef struct LedMessage
{
	int enable;
	int intensity;
	int id;	
} LedMessage;

typedef struct LedInterface
{
	Rect rect;
	Slider slider;
	Button button;	
} LedInterface;

typedef struct LedScreen
{
	Button buttons[NumberOfButtons];	
	LedInterface interfaces[NumberOfInterfaces];	
} LedScreen;

void vStartLcd(unsigned portBASE_TYPE uxPriority, xQueueHandle xQueue);

#endif
