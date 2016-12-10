#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "Shapes.h"
#include "Colour.h"
#include "Components.h"

#define DisplayWidth 	240
#define DisplayHeight 320

void movePen(unsigned short x, unsigned short y);
void movePenPoint(Point p);

void fillScreen(Colour c);

void drawPoint(unsigned short x, unsigned short y, Colour c);
void drawPointStruct(Point p, Colour c);

void drawRect(unsigned short x, unsigned short y, unsigned short w, unsigned short h, Colour c);
void drawRectStruct(Rect r, Colour c);

void fillRect(unsigned short x, unsigned short y, unsigned short w, unsigned short h, Colour c);
void fillRectStruct(Rect r, Colour c);

void drawLine(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, Colour c);

void drawCircle(unsigned short x, unsigned short y, unsigned short r, Colour c);
void drawCircleStruct(Circle o, Colour c);

unsigned long putChar(unsigned short x, unsigned short y, unsigned char ch);

void putString(unsigned short x, unsigned short y, unsigned char *pStr);

void fontColour(Colour fore, Colour back);

void picture(unsigned short x, unsigned short y, unsigned short width, unsigned short height, unsigned short *pPicture);
void pictureBegin(unsigned short x, unsigned short y, unsigned short width, unsigned short height);
void pictureData(unsigned short *pPicture, unsigned short len);
void pictureEnd(void);

void drawButton(Button b, ColourPalette c, int m);
void drawSlider(Slider s, ColourPalette c, int m);

#endif
