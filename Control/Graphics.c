#include <LPC24xx.H>
#include <general.h>
#include "lcd_hw.h"
#include "Graphics.h"
#include "font5x7.h"


static Colour foregroundColor = White;
static Colour backgroundColor = Black;

static unsigned char const  font_mask[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};


/******************************************************************************
** Function name:		hLine		
**
** Descriptions:		Draw a horizontal line from x0 to x1 on y0.		
**
** parameters:			x0, y0, x1, color
** Returned value:		None
** 
******************************************************************************/
static void hLine(unsigned short x0, unsigned short y0, unsigned short x1, Colour color) 
{
  unsigned short bak;

  if (x0 > x1) 						
  {
    bak = x1;
    x1 = x0;
    x0 = bak;
  }
  drawPoint(x0, y0, color);
  x0++;
   
  while(x1 >= x0)
  {
    writeToDisp(color);
    x0++;
  }
  return;
}

/******************************************************************************
** Function name:		vLine
**
** Descriptions:		Draw a vertical line from y0 to y1 on x0.
**
** parameters:			x0, y0, y1, color
** Returned value:		None
** 
******************************************************************************/
static void vLine(unsigned short x0, unsigned short y0, unsigned short y1, Colour color)
{
  unsigned short bak;

  if(y0 > y1) 						
  {
    bak = y1;
    y1 = y0;
    y0 = bak;
  }

  while(y1 >= y0)
  {
    drawPoint(x0, y0, color);
    y0++;
  }
  return;
}


/******************************************************************************
** Function name:		lcd_movePen
**
** Descriptions:		Move the pen to a particular location.		
**
** parameters:			pixel x and y
** Returned value:		None
** 
******************************************************************************/
void movePen(unsigned short x, unsigned short y)
{
  if (activeController == V2_CONTROLLER)
  {
    writeToReg(0x4e, x & 0xff);
    writeToReg(0x4f, y & 0x1ff);
  }
  else
  {
    writeLcdCommand(0x4200 | (x & 0xff));	  	  /* x start address */
    writeLcdCommand(0x4300 | ((y>>8) & 0xff));  /* y start address MSB */
    writeLcdCommand(0x4400 | (y & 0xff));       /* y start address LSB */
  }
  return;
}

void movePenPoint(Point p)
{
  if (activeController == V2_CONTROLLER)
  {
    writeToReg(0x4e, p.x & 0xff);
    writeToReg(0x4f, p.y & 0x1ff);
  }
  else
  {
    writeLcdCommand(0x4200 | (p.x & 0xff));	  	  /* x start address */
    writeLcdCommand(0x4300 | ((p.y>>8) & 0xff));  /* y start address MSB */
    writeLcdCommand(0x4400 | (p.y & 0xff));       /* y start address LSB */
  }
  return;
}

/******************************************************************************
** Function name:		lcd_setWindow
**
** Descriptions:		Set the window area without filling the color		
**
** parameters:			x0, y0, x1, y1
** Returned value:		If the range is not set correctly, e.g. x1 <= x0
**				y1 <= y0, return false, the window will not be set.
** 
******************************************************************************/
unsigned long setWindow(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1)  
{
  if (x1 > DisplayWidth-1) 
  {  
    x1 = DisplayWidth-1;
  }
  
  if (y1 > DisplayHeight-1)
  {
    y1 = DisplayHeight-1;
  }
  
  if ((x1 <= x0) || (y1 <= y0))
  {
    return( FALSE );
  }

  if (activeController == V1_CONTROLLER)
  {
    /* TODO x and y values aren't used below. 240x320 (239x319) is always used */
    writeLcdCommand(0x4500); /* X-start address */
    writeLcdCommand(0x46EF); /* X-end address */
    writeLcdCommand(0x4700); /* Y-start address MSB */
    writeLcdCommand(0x4800); /* Y-start address LSB */
    writeLcdCommand(0x4901); /* Y-end address MSB */
    writeLcdCommand(0x4A3F); /* Y-end address LSB */
  }
  return( TRUE );
}

/******************************************************************************
** Function name:		lcd_fillScreen
**
** Descriptions:		Fill the LCD screen with color		
**
** parameters:			Color
** Returned value:		None
** 
******************************************************************************/
void fillScreen(Colour color)
{
  unsigned short i = 0;
  unsigned short j = 0;
  
  setWindow(0, 0, DisplayWidth-1, DisplayHeight-1); 
  movePen(0, 0);

  for(i=0; i < DisplayHeight; i++)
  {
    for(j=0; j<DisplayWidth; j++)
    {
      writeToDisp(color);
    }
  }
  return;
}


/******************************************************************************
** Function name:		lcd_point
**
** Descriptions:		Draw a point at {x0, y0} on the LCD		
**				if {x0,y0} is out of range, display nothing.
** parameters:			x0, y0, color
** Returned value:		None
** 
******************************************************************************/
void drawPoint(unsigned short x, unsigned short y, Colour color)
{
  if( x >= DisplayWidth )  
  {
	return;
  }
  
  if(y >= DisplayHeight)
  {
	return;
  }
  movePen(x, y);
  writeToDisp(color);
  return;
}

void drawPointStruct(Point p, Colour color)
{
  if( p.x >= DisplayWidth )  
  {
	return;
  }
  
  if( p.y >= DisplayHeight)
  {
	return;
  }
  movePenPoint(p);
  writeToDisp(color);
  return;
}


/******************************************************************************
** Function name:		lcd_movePen
**
** Descriptions:		
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void drawRect(unsigned short x, unsigned short y, unsigned short w, unsigned short h, Colour color)
{  
  hLine(x, y, x + w, color);
  hLine(x, y + h, x + w, color);
  vLine(x, y, y + h, color);
  vLine(x + w, y, y + h, color);
  return;
}

void drawRectStruct(Rect r, Colour color)
{  
  hLine(r.x, r.y, r.x + r.width, color);
  hLine(r.x, r.y + r.height, r.x + r.width, color);
  vLine(r.x, r.y, r.y + r.height, color);
  vLine(r.x + r.width, r.y, r.y + r.height, color);
  return;
}

/******************************************************************************
** Function name:		lcd_fillRect
**
** Descriptions:		Fill a rectangle with color, the area is
**						{x0, y0, x1, y1 }
**
** parameters:			x0, y0, x1, y1, color
** Returned value:		None
** 
******************************************************************************/
void fillRect(unsigned short x0, unsigned short y0, unsigned short w, unsigned short h, Colour color)
{  
  unsigned short i = 0;
	unsigned short x1 = x0 + w;
	unsigned short y1 = y0 + h;
  
  if(x0 > x1)
  {   
	i  = x0;
	x0 = x1;
	x1 = i;
  }

  if(y0 > y1)
  {   
	i  = y0;
	y0 = y1;
	y1 = i;
  }
   
  if(y0 == y1) 
  {  
	hLine(x0, y0, x1, color);
	return;
  }

  if(x0 == x1) 
  {  
	vLine(x0, y0, y1, color);
	return;
  }

  while(y0 <= y1)						
  {  
	hLine(x0, y0, x1, color);
	y0++;
  }
  return;
}

void fillRectStruct(Rect r, Colour c)
{
	fillRect(r.x, r.y, r.width, r.height, c);
}

/******************************************************************************
** Function name:		lcd_line
**
** Descriptions:		draw a line between {x0,y0} and {x1,y1}
**						the last parameter is the color of the line		
**
** parameters:			x0, y0, x1, y1, color
** Returned value:		None
** 
******************************************************************************/
void drawLine(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, Colour color)
{  
  signed short   dx = 0, dy = 0;
  signed char    dx_sym = 0, dy_sym = 0;
  signed short   dx_x2 = 0, dy_x2 = 0;
  signed short   di = 0;
 
  dx = x1-x0;
  dy = y1-y0;
  

  if(dx == 0)			/* vertical line */ 
  {
	vLine(x0, y0, y1, color);
	return;
  }

  if(dx > 0)
  {    
	dx_sym = 1;
  }
  else
  { 
	dx_sym = -1;
  }

  if(dy == 0)			/* horizontal line */  
  {
	hLine(x0, y0, x1, color);
	return;
  }

  if(dy > 0)
  {    
	dy_sym = 1;
  }
  else
  { 
	dy_sym = -1;
  }

  dx = dx_sym*dx;
  dy = dy_sym*dy;
 
  dx_x2 = dx*2;
  dy_x2 = dy*2;
   
  if(dx >= dy)
  { 
	di = dy_x2 - dx;
	while(x0 != x1)
	{   
	  drawPoint(x0, y0, color);
	  x0 += dx_sym;
	  if(di<0)
	  {
		di += dy_x2;
	  }
	  else
	  {
		di += dy_x2 - dx_x2;
		y0 += dy_sym;
	  }
	}
	drawPoint(x0, y0, color);
  }
  else
  {
	di = dx_x2 - dy;
	while(y0 != y1)
	{   
	  drawPoint(x0, y0, color);
	  y0 += dy_sym;
	  if(di < 0)
	  { 
		di += dx_x2;
	  }
	  else
	  {
		di += dx_x2 - dy_x2;
		x0 += dx_sym;
	  }
	}
		drawPoint(x0, y0, color);
  }
  return; 
}

/******************************************************************************
** Function name:		lcd_circle
**
** Descriptions:		Use x0 and y0 as the center point to draw a 
**				a cycle with radius length r, and the latest parameter
**				is the color of the circle		
**
** parameters:			x0, y0, radius, color
** Returned value:		None
** 
******************************************************************************/
void drawCircle(unsigned short x0, unsigned short y0, unsigned short r, Colour color)
{
  signed short draw_x0, draw_y0;
  signed short draw_x1, draw_y1;	
  signed short draw_x2, draw_y2;	
  signed short draw_x3, draw_y3;	
  signed short draw_x4, draw_y4;	
  signed short draw_x5, draw_y5;	
  signed short draw_x6, draw_y6;	
  signed short draw_x7, draw_y7;	
  signed short xx, yy;
  signed short  di;
   
  if(r == 0)		  /* no radius */ 
  {
	return;
  }
   
  draw_x0 = draw_x1 = x0;
  draw_y0 = draw_y1 = y0 + r;
  if(draw_y0 < DisplayHeight)
  { 
		drawPoint(draw_x0, draw_y0, color);		/* 90 degree */
  }
	
  draw_x2 = draw_x3 = x0;
  draw_y2 = draw_y3 = y0 - r;
  if(draw_y2 >= 0)
  {
		drawPoint(draw_x2, draw_y2, color);    /* 270 degree */
  }
   	
  draw_x4 = draw_x6 = x0 + r;
  draw_y4 = draw_y6 = y0;
  if(draw_x4 < DisplayWidth)
  {
		drawPoint(draw_x4, draw_y4, color);		/* 0 degree */
  }
   
  draw_x5 = draw_x7 = x0 - r;
  draw_y5 = draw_y7 = y0;
  if(draw_x5>=0)
  {
		drawPoint(draw_x5, draw_y5, color);		/* 180 degree */
  }

  if(r == 1)
  {
	return;
  }
   
  di = 3 - 2*r;
  xx = 0;
  yy = r;
  while(xx < yy)
  {  
	if(di < 0)
	{
	  di += 4*xx + 6;	      
	}
	else
	{  
	  di += 4*(xx - yy) + 10;
	  yy--;	  
	  draw_y0--;
	  draw_y1--;
	  draw_y2++;
	  draw_y3++;
	  draw_x4--;
	  draw_x5++;
	  draw_x6--;
	  draw_x7++;	 	
	}  
	xx++;   
	draw_x0++;
	draw_x1--;
	draw_x2++;
	draw_x3--;
	draw_y4++;
	draw_y5++;
	draw_y6--;
	draw_y7--;
	
	if( (draw_x0 <= DisplayWidth) && (draw_y0>=0) )	
	{
		drawPoint(draw_x0, draw_y0, color);
	}
	    
	if( (draw_x1 >= 0) && (draw_y1 >= 0) )	
	{ 
		drawPoint(draw_x1, draw_y1, color);
	}

	if( (draw_x2 <= DisplayWidth) && (draw_y2 <= DisplayHeight) )
	{
	  drawPoint(draw_x2, draw_y2, color);
	}

	if( (draw_x3 >=0 ) && (draw_y3 <= DisplayHeight) )	
	{
	  drawPoint(draw_x3, draw_y3, color);
	}

	if( (draw_x4 <= DisplayHeight) && (draw_y4 >= 0) )	
	{
	  drawPoint(draw_x4, draw_y4, color);
	}

	if( (draw_x5 >= 0) && (draw_y5 >= 0) )
	{  
	  drawPoint(draw_x5, draw_y5, color);
	}
	if( (draw_x6 <= DisplayWidth) && (draw_y6 <= DisplayHeight) )	
	{
	  drawPoint(draw_x6, draw_y6, color);
	}
	if( (draw_x7 >= 0) && (draw_y7 <= DisplayHeight) )	
	{  
	  drawPoint(draw_x7, draw_y7, color);
	}
  }
  return;
}

void drawCircleStruct(Circle o, Colour c)
{
	drawCircle(o.x, o.y, o.radius, c);
}

/******************************************************************************
** Function name:		lcd_putChar
**
** Descriptions:		Put one chacter on the LCD for display		
**
** parameters:			pixel X and Y, and the character
** Returned value:		TRUE or FALSE, if the pixels given are out of range,
**						nothing will be written.
** 
******************************************************************************/
unsigned long putChar(unsigned short x, unsigned short y, unsigned char ch)
{  
  unsigned char data = 0;
  unsigned char i = 0, j = 0;
  
  Colour color = Black;

  if((x >= (DisplayWidth - 8)) || (y >= (DisplayHeight - 8)) )
  {
	return( FALSE );
  }

  if( (ch < 0x20) || (ch > 0x7f) )
  {
	ch = 0x20;		/* unknown character will be set to blank */
  }
   
  ch -= 0x20;
  for(i=0; i<8; i++)
  {
    data = font5x7[ch][i];
    for(j=0; j<6; j++)
    {
	    if( (data&font_mask[j])==0 )
	    {  
		    color = backgroundColor;
	    }
	    else
	    {
		    color = foregroundColor;
	    }
	    drawPoint(x, y, color);       
	    x++;
    }   
    y++;
    x -= 6;
  }
  return( TRUE );
}

/******************************************************************************
** Function name:		lcd_putString
**
** Descriptions:		Put a string of characters for display		
**
** parameters:			x and y pixels, and the pointer to the string characters
** Returned value:		None
** 
******************************************************************************/
void putString(unsigned short x, unsigned short y, unsigned char *pStr)
{
  while(1)
  {      
	  if( (*pStr)=='\0' )
	  {
		  break;
	  }
	  if( putChar(x, y, *pStr++) == 0 )
	  {
  		break;
  	}
  	x += 6;
  }
  return;
}

/******************************************************************************
** Function name:		lcd_fontColor
**
** Descriptions:		foreground and back ground color setting		
**
** parameters:			foreground color and background color
** Returned value:		None
** 
******************************************************************************/

void fontColor(Colour foreground, Colour background)
{
  foregroundColor = foreground;
  backgroundColor = background;
  return;
}

/******************************************************************************
** Function name:		lcd_pictureBegin
**
** Descriptions:		Set where the pixels of the picture should be set, the 
**						size of the picture.		
**
** parameters:			x, y, width, and height
** Returned value:		None
** 
******************************************************************************/
void pictureBegin(unsigned short x, unsigned short y, unsigned short width, unsigned short height)
{
  /* set window */
  setWindow(x,y, x+width-1,y+height-1);
  movePen(x, y);
  return;
}

/******************************************************************************
** Function name:		lcd_pictureData
**
** Descriptions:		
**
** parameters:			pointer to the picture and total size which is normally
**				the width multiply by the height.
** Returned value:		None
** 
******************************************************************************/
void pictureData(unsigned short *pPicture, unsigned short len)
{
  unsigned short i = 0;
  
  for (i=0; i<len; i++)
  {
    writeToDisp(*pPicture++);
  }
  return;
}

/******************************************************************************
** Function name:		lcd_pictureEnd
**
** Descriptions:		Restore window		
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void pictureEnd(void)
{
  /* restore window */
  setWindow(0,0, DisplayWidth-1,DisplayHeight-1);
  return;
}

/******************************************************************************
** Function name:		lcd_picture
**
** Descriptions:		put a picture file on the LCD display
**						pixels, width, and height are the parameters of
**						of the location, pPicuture is the pointer to the
**						picture		
**
** parameters:			x, y, width, height, pointer to the picture
** Returned value:		None
** 
******************************************************************************/
void picture(unsigned short x, unsigned short y, unsigned short width, unsigned short height, unsigned short *pPicture)
{
  pictureBegin(x, y, width, height);
  pictureData(pPicture, width*height);
  pictureEnd();
  return;

}

/******************************************************************************
** Descriptions:		Components
**
**
** parameters:			x, y, width, height, pointer to the picture
** Returned value:		None
** 
******************************************************************************/
void drawButton(Button b, ColourPalette cc, int m)
{
	Colour c;
	Rect rect = b.rect;
	switch(b.state) {
		case On:
			c = cc.four;
			break;
		case Off:
			c = m == 0 ? cc.one : cc.two;
			break;
		case Dim:
			c = cc.three;
			break;
		case Pressed:
			break;
	}	 
	fillRectStruct(rect, c);
}

void drawSlider(Slider s, ColourPalette c, int m)
{
	Rect rect = s.rect;
	int x = s.mode == 0 ? rect.x : rect.y;
	int w = s.mode == 0 ? rect.width : rect.height;
	
	int r1_size = s.sPos;
	int r3_size = w - r1_size - s.sSize;
	int r2 = x + s.sSize;
	int r3 = r2 + s.sSize;
	Colour off = (m == 0) ? c.one : c.two;
	
	if(s.mode == Horizontal) 	
	{
		fillRect(rect.x, rect.y, r1_size, rect.height, off);
		fillRect(r2, rect.y, s.sSize, rect.height, c.four);	
		fillRect(r3, rect.y, r3_size, rect.height, off);	
	}
	else
	{
		fillRect(rect.x, rect.y, rect.width, r1_size, off);
		fillRect(rect.x, r2, rect.width, s.sSize, c.four);	
		fillRect(rect.x, r3, rect.width, r3_size, off);
	}	
}

Colour getBrightnessColour(unsigned char b)
{
	unsigned char t = b >> 3; //5 Bits
	unsigned char r = 0;
	
	r |= (t << RPos) | (t << GPos) | (t << BPos);
	return r;	
}

void drawLight(Light l)
{
	Colour c = getBrightnessColour(l.brightness);
	fillRect(l.rect.x, l.rect.y, l.rect.width, l.rect.height, c);
}
