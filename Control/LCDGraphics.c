#include "LCDGraphics.h"
#include "IDManager.h"
#include "Messages.h"

LedScreen screen;
ColourPalette palette;

/* Drawing Methods */

void drawLedScreen(void)
{
	int i = 0, j = 0;
	fillScreen(MuddyWhite);
	
	for(j = 0; j < NumberOfInterfaces; j++) 
	{		
		drawSlider(screen.interfaces[j].slider, palette, (j % 2));
		drawButton(screen.interfaces[j].buttons[0], palette, 0);
		drawButton(screen.interfaces[j].buttons[1], palette, 1);
		drawLight(screen.interfaces[j].lights[0]);
		drawLight(screen.interfaces[j].lights[1]);
	}
	
	for(i = 0; i < NumberOfButtons; i++) 
	{
		drawButton(screen.buttons[i], palette, (i % 2));
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

void drawLights(LedMessage m)
{
	unsigned short l, d;
	
	if((m.mode & UpdatePulse0) == UpdatePulse0)
	{
		screen.interfaces[0].lights[0].brightness = m.pulse0;
		screen.interfaces[0].lights[1].brightness = m.pulse0;
	}
	if((m.mode & UpdatePulse1) == UpdatePulse1)
	{
		screen.interfaces[1].lights[0].brightness = m.pulse1;
		screen.interfaces[1].lights[1].brightness = m.pulse1;
	}
	
	if((m.mode & UpdateData) == UpdateData)
	{
		l = getLightData();
		d = l ^ m.data;
		
		if((d & 0x3) > 0)
			drawLight(screen.interfaces[0].lights[0]);
		if(((d >> 2) & 0x3) > 0)
			drawLight(screen.interfaces[0].lights[1]);
		if(((d >> 4) & 0x3) > 0)
			drawLight(screen.interfaces[1].lights[0]);
		if(((d >> 6) & 0x3) > 0)
			drawLight(screen.interfaces[1].lights[1]);		
		setLightData(m.data);
	}
}

/* Logic Computation */

unsigned char isPresetButton(int id)
{
	int j;
	for(j = 0; j < NumberOfButtons; j++) 
	{
		if(screen.buttons[j].id == id)
			return 1;
	}	
	return 0;
}

unsigned char isSlider(int id)
{
	int j;
	for(j = 0; j < NumberOfInterfaces; j++) 
	{
		if(screen.interfaces[j].slider.id == id)
			return j;
	}
	return 0xFF;
}

unsigned short getLedMode(int id)
{
	unsigned char preset, slider;
	
	preset = isPresetButton(id);
	if(preset) 
		return UpdateData | UpdatePulse0 | UpdatePulse1;
	
	slider = isSlider(id);
	if(slider != 0xFF)
		return slider == 0 ? UpdatePulse0 : UpdatePulse1;
	
	return UpdateData;	
}

void setLightData(unsigned short d)
{
	unsigned short t;
	t = d & 0x3;
	t = (t == 0x3) ? Dim : t;
	screen.interfaces[0].lights[0].state = t;
	t = (d >> 2) & 0x3;
	t = (t == 0x3) ? Dim : t;
	screen.interfaces[0].lights[1].state = t;
	t = (d >> 4) & 0x3;
	t = (t == 0x3) ? Dim : t;
	screen.interfaces[1].lights[0].state = t;
	t = (d >> 6) & 0x3;
	t = (t == 0x3) ? Dim : t;
	screen.interfaces[1].lights[1].state = t;
}

unsigned short getLightData(void)
{
	unsigned short data = 0, state;	
	
	state = screen.interfaces[0].lights[0].state;	
	data |= state & ~(Pressed);
	
	state = screen.interfaces[0].lights[1].state;
	data |= (state & ~(Pressed)) << 2;
	
	state = screen.interfaces[1].lights[0].state;	
	data |= (state & ~(Pressed)) << 4;
	
	state = screen.interfaces[1].lights[1].state;	
	data |= (state & ~(Pressed)) << 6;
	
	return data;
}

unsigned short getLedData(void)
{
	unsigned short data = 0, state;	
	
	state = screen.interfaces[0].buttons[0].state;	
	data |= state & ~(Pressed);
	
	state = screen.interfaces[0].buttons[1].state;
	data |= (state & ~(Pressed)) << 2;
	
	state = screen.interfaces[1].buttons[0].state;
	state = state == Dim ? Dim + 1 : state; 	
	data |= (state & ~(Pressed)) << 4;
	
	state = screen.interfaces[1].buttons[1].state;
	state = state == Dim ? Dim + 1 : state;
	data |= (state & ~(Pressed)) << 6;
	
	return data;
}

unsigned short getLedPulse0(void)
{
	Slider s = screen.interfaces[0].slider;
	int i = s.mode == Horizontal ? s.rect.width : s.rect.height;
	return 256 * s.sPos	/ i;
}

unsigned short getLedPulse1(void)
{
	Slider s = screen.interfaces[1].slider;
	int i = s.mode == Horizontal ? s.rect.width : s.rect.height;
	return 256 * s.sPos	/ i;
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
	if(b->state == Off)
		b->state = On;
	else if(b->state == On)
		b->state = Dim;
	else if(b->state == Dim)
		b->state = Off;

	/*if((b->state & Pressed) == Off)
	{
		b->state |= Pressed;
		b->state |= ((b->state & On) == On) ? Off : On;		
	}
	else
		b->state &= ~(Pressed); */
}

void handleSlider(Slider *s, Point p) 
{
	int x = s->mode == Horizontal ? p.x : p.y;
	int w = s->mode == Horizontal ? s->rect.x + s->rect.width : s->rect.y + s->rect.height;	
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
	
	xPos = 0 + interfaceSize9th;
	yPos = buttonSize;
	
	for(i = 0; i < NumberOfInterfaces; i++)
	{				
		screen.interfaces[i].buttons[0].rect.x = xPos + (2 + 4*i) * interfaceSize9th;
		screen.interfaces[i].buttons[0].rect.y = yPos + 1 * interfaceSize9th;
		screen.interfaces[i].buttons[0].rect.width = 1 * interfaceSize9th;
		screen.interfaces[i].buttons[0].rect.height = 1 * interfaceSize9th;
		screen.interfaces[i].buttons[0].state = Off;
		screen.interfaces[i].buttons[0].id = Register(ButtonType);
		
		screen.interfaces[i].buttons[1].rect.x = xPos + (2 + 4*i) * interfaceSize9th;
		screen.interfaces[i].buttons[1].rect.y = yPos + 7 * interfaceSize9th;
		screen.interfaces[i].buttons[1].rect.width = 1 * interfaceSize9th;
		screen.interfaces[i].buttons[1].rect.height = 1 * interfaceSize9th;
		screen.interfaces[i].buttons[1].state = Off;
		screen.interfaces[i].buttons[1].id = Register(ButtonType);
		
		screen.interfaces[i].slider.rect.x = xPos + (4*i) * interfaceSize9th;
		screen.interfaces[i].slider.rect.y = yPos + 1 * interfaceSize9th;
		screen.interfaces[i].slider.rect.width = 1 * interfaceSize9th;
		screen.interfaces[i].slider.rect.height = 7 * interfaceSize9th;
		screen.interfaces[i].slider.id = Register(SliderType);
		
		screen.interfaces[i].slider.mode = Vertical;		
		screen.interfaces[i].slider.sPos = 0;
		screen.interfaces[i].slider.sSize = 1 * interfaceSize9th;
		
		screen.interfaces[i].lights[0].rect.x = xPos + (2 + 4*i) * interfaceSize9th;
		screen.interfaces[i].lights[0].rect.y = yPos + 3 * interfaceSize9th;
		screen.interfaces[i].lights[0].rect.width = 1 * interfaceSize9th;
		screen.interfaces[i].lights[0].rect.height = 1 * interfaceSize9th;
		screen.interfaces[i].lights[0].brightness = 0;
		screen.interfaces[i].lights[0].id = Register(LightType);
		
		screen.interfaces[i].lights[1].rect.x = xPos + (2 + 4*i) * interfaceSize9th;
		screen.interfaces[i].lights[1].rect.y = yPos + 5 * interfaceSize9th;
		screen.interfaces[i].lights[1].rect.width = 1 * interfaceSize9th;
		screen.interfaces[i].lights[1].rect.height = 1 * interfaceSize9th;
		screen.interfaces[i].lights[1].brightness = 0;
		screen.interfaces[i].lights[1].id = Register(LightType);
	}				
	
	for(i = 0; i < NumberOfButtons; i++)
	{
		screen.buttons[i].rect.x = i * buttonSize;
		screen.buttons[i].rect.y = 0;
		screen.buttons[i].rect.width = buttonSize;
		screen.buttons[i].rect.height = buttonSize;
		screen.buttons[i].state = Off;
		screen.buttons[i].id = Register(PresetType);
	}
}
