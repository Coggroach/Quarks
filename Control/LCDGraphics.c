#include "LCDGraphics.h"

LedScreen screen;
ColourPalette palette;

/* Drawing Methods */

void drawLedScreen(void)
{
	int i = 0, j = 0;
	fillScreen(White);
	
	for(i = 0; i < NumberOfButtons; i++) 
	{
		drawButton(screen.buttons[i], palette, (i % 2));
	}
	
	for(j = 0; j < NumberOfInterfaces; j++) 
	{		
		drawSlider(screen.interfaces[j].slider, palette, (j % 2));
		drawButton(screen.interfaces[j].buttons[0], palette, 0);
		drawButton(screen.interfaces[j].buttons[1], palette, 1);
	}
}

void drawButtonPointer(Button* b) 
{
	drawButton(*b, palette, b->id % 2);
}

void drawSliderPointer(Slider* s)
{
	drawSlider(*s, palette, s->id % 2);
}

/* Data Accessors */

Slider* getSlider(int id)
{
	int j;
	for(j = 0; j < NumberOfInterfaces; j++) 
	{
		if(screen.interfaces[j].slider.id == id)
			return &screen.interfaces[j].slider;
	}
	return 0;
}

Button* getButton(int id)
{
	int j;
	for(j = 0; j < NumberOfButtons; j++) 
	{
		if(screen.buttons[j].id == id)
			return &screen.buttons[j];
	}	
	for(j = 0; j < NumberOfInterfaces; j++) 
	{
		if(screen.interfaces[j].buttons[0].id == id)
			return &screen.interfaces[j].buttons[0];
		if(screen.interfaces[j].buttons[1].id == id)
			return &screen.interfaces[j].buttons[1];
	}
	return 0;
}

Point getPoint(unsigned int x, unsigned int y)
{
	Point p;
	p.x = x;
	p.y = y;
	return p;
}

/* Event Handling Methods */

void handleButton(Button* b)
{
	if((b->state & Pressed) == Off)
	{
		b->state |= Pressed;
		b->state |= ((b->state & On) == On) ? Off : On;		
	}
	else
		b->state &= ~(Pressed);
}

void handleSlider(Slider *s, Point p) 
{
	int x = s->mode == 0 ? p.x : p.y;
	int w = s->mode == 0 ? s->rect.x + s->rect.width : s->rect.y + s->rect.height;	
	int result = x - s->sSize/2;
	
	if(result < 0)
		result = 0;
	if(result + s->sSize >= w)
		result = w - s->sSize;
	s->sPos = result;
}

int isComponentTouched(unsigned int x, unsigned int y)
{
	int i = 0, j = 0;
	Point p;
	p.x = x;
	p.y = y;
	
	for(i = 0; i < NumberOfButtons; i++) 
	{
		if(doesRectContainPoint(screen.buttons[i].rect, p) > 0)		
			return screen.buttons[i].id;
	}
	
	for(j = 0; j < NumberOfInterfaces; j++) 
	{
		if(doesRectContainPoint(screen.interfaces[j].slider.rect, p) > 0)
			return screen.interfaces[j].slider.id;
		if(doesRectContainPoint(screen.interfaces[j].buttons[0].rect, p) > 0)
			return screen.interfaces[j].buttons[0].id;
		if(doesRectContainPoint(screen.interfaces[j].buttons[1].rect, p) > 0)
			return screen.interfaces[j].buttons[1].id;
	}
	return ~(0x0);
}

/* Initialisation Methods */
void setupLedScreen(void)
{
	int i;		
	int buttonSize = DisplayWidth / NumberOfButtons;			
	int interfaceSize9th = DisplayWidth / 9;		
	int xPos, yPos;
	
	palette.one = Acoppola;
	palette.two = TanahLess;
	palette.three = TanahLot;
	palette.four = SpeedingOrange;
	
	for(i = 0; i < NumberOfButtons; i++)
	{
		screen.buttons[i].rect.x = i * buttonSize;
		screen.buttons[i].rect.y = 0;
		screen.buttons[i].rect.width = buttonSize;
		screen.buttons[i].rect.height = buttonSize;
		screen.buttons[i].state = Off;
		screen.buttons[i].id = i + 6;
	}
	
	xPos = 0 + interfaceSize9th;
	yPos = buttonSize;
	
	for(i = 0; i < NumberOfInterfaces; i++)
	{				
		screen.interfaces[i].slider.rect.x = xPos + (4*i) * interfaceSize9th;
		screen.interfaces[i].slider.rect.y = yPos + 1 * interfaceSize9th;
		screen.interfaces[i].slider.rect.width = 1 * interfaceSize9th;
		screen.interfaces[i].slider.rect.height = 7 * interfaceSize9th;
		screen.interfaces[i].slider.id = i + 4;
		
		screen.interfaces[i].slider.mode = Vertical;		
		screen.interfaces[i].slider.sPos = 0;
		screen.interfaces[i].slider.sSize = 1 * interfaceSize9th;
		
		screen.interfaces[i].buttons[0].rect.x = xPos + (2 + 4*i) * interfaceSize9th;
		screen.interfaces[i].buttons[0].rect.y = yPos + 1 * interfaceSize9th;
		screen.interfaces[i].buttons[0].rect.width = 1 * interfaceSize9th;
		screen.interfaces[i].buttons[0].rect.height = 1 * interfaceSize9th;
		screen.interfaces[i].buttons[0].state = Off;
		screen.interfaces[i].buttons[0].id = i * 2;
		
		screen.interfaces[i].buttons[1].rect.x = xPos + (2 + 4*i) * interfaceSize9th;
		screen.interfaces[i].buttons[1].rect.y = yPos + 8 * interfaceSize9th;
		screen.interfaces[i].buttons[1].rect.width = 1 * interfaceSize9th;
		screen.interfaces[i].buttons[1].rect.height = 1 * interfaceSize9th;
		screen.interfaces[i].buttons[1].state = Off;
		screen.interfaces[i].buttons[1].id = i * 2 + 1;
	}				
}
