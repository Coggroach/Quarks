#ifndef LCDGRAPHICS_H
#define LCDGRAPHICS_H

#include "FreeRTOS.h"
#include "Graphics.h"
#include "queue.h"
#include "Messages.h"

#define NumberOfButtons 5
#define NumberOfInterfaces 2
#define NumberOfButtonsInInterface 2
#define NumberOfLightsInInterface NumberOfButtonsInInterface
#define NumberOfInterfacesInRow 2
#define NumberOfComponents	(NumberOfButtons + NumberOfInterfaces * (NumberOfButtonsInInterface + NumberOfLightsInInterface))

typedef struct LedInterface
{
	Slider slider;
	Button buttons[NumberOfButtonsInInterface];
	Light lights[NumberOfLightsInInterface];
} LedInterface;

typedef struct LedScreen
{
	Button buttons[NumberOfButtons];	
	LedInterface interfaces[NumberOfInterfaces];	
} LedScreen;

void drawLedScreen(void);
void drawButtonPointer(Button* b);
void drawSliderPointer(Slider* b);

void drawLights(LedMessage m);
void drawButtons(void);
void drawSliders(void);
void drawPresets(void);

void handleButton(Button *b);
void handlePreset(Button *b);
void handleSlider(Slider *s, Point p);
void handleInterface(LedInterface *i, int id, xQueueHandle xQueue, Point p);
int isComponentTouched(unsigned int x, unsigned int y);
void setupLedScreen(void);
Slider* getSlider(int id);
Button* getButton(int id);
int getPresetButtonId(Button* b);
Point getPoint(unsigned int x, unsigned int y);

unsigned char isPresetButton(int id);
unsigned char isSlider(int id);
unsigned short getLedMode(int id);
unsigned short getLedData(void);
unsigned short getLightData(void);
void setLightData(unsigned short d);
unsigned int getLedPulse0(void);
unsigned int getLedPulse1(void);

#endif
