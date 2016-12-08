#include "FreeRTOS.h"
#include "Control/LEDControl.h"
#include "Control/Graphics.h"
#include "task.h"
#include "semphr.h"
#include "lcd_hw.h"
#include <stdio.h>
#include <string.h>

static LedScreen screen;

static void drawLedScreen(void)
{
	int i = 0, j = 0;
	fillScreen(MuddyWhite);
	
	for(i = 0; i < NumberOfButtons; i++) 
	{
		Colour color = (i % 2 == 0) ? TanahLess : TanahLot;
		drawButton(screen.buttons[i], color, SpeedingOrange);
	}
	
	for(j = 0; j < NumberOfInterfaces; j++) 
	{
		drawRectStruct(screen.interfaces[j].rect, Acoppola);
		drawSlider(screen.interfaces[j].slider, Acoppola, SpeedingOrange);
		drawButton(screen.interfaces[j].button, Acoppola, SpeedingOrange);
	}
}

static void handleButton(Button *b) 
{
	b->value = b->value == 0 ? 1 : 0;
}

static void handleSlider(Slider *s, Point p) 
{
	int x = s->mode == 0 ? p.x : p.y;
	int w = s->mode == 0 ? s->rect.x + s->rect.width : s->rect.y + s->rect.height;
	
	s->sPos = x - s->sSize/2;
	if(s->sPos < 0)
		s->sPos = 0;
	if(s->sPos + s->sSize >= w)
		s->sPos = w - s->sSize;
}

static void handleInterface(LedInterface *i, int id, xQueueHandle xQueue, Point p) 
{
	LedMessage message;
	unsigned char sendMessage = 0;
	
	message.id = id;	
	if(doesRectContainPoint(i->button.rect, p) > 0)
	{
		handleButton(&i->button);		
		message.enable = i->button.value;
		sendMessage = 1;
	}
	if(doesRectContainPoint(i->slider.rect, p) > 0)
	{			
		handleSlider(&i->slider, p);
		message.intensity = (unsigned char) (256 * i->slider.sPos / i->slider.rect.width);
		sendMessage = 1;
	}
	if(sendMessage == 1) 
	{
		xQueueSendToBack(xQueue, &message, portMAX_DELAY);
	}		
}

static void updateLedScreen(xQueueHandle xQueue, unsigned int x, unsigned int y)
{
	int i = 0, j = 0;
	Point p;
	p.x = x;
	p.y = y;
	
	for(i = 0; i < NumberOfButtons; i++) 
	{
		if(doesRectContainPoint(screen.buttons[i].rect, p) > 0) 
		{
			handleButton(&screen.buttons[i]);
		}
	}
	
	for(j = 0; j < NumberOfInterfaces; j++) 
	{
		if(doesRectContainPoint(screen.interfaces[j].rect, p) > 0)
		{
			handleInterface(&screen.interfaces[j], j, xQueue, p);
		}
	}
}

static void setupLedScreen(void)
{
	int buttonWidth = DisplayWidth / NumberOfButtons;
	int buttonHeight = buttonWidth;
	int i = 0, j = 0;		
	int interfaceWidth = DisplayWidth / NumberOfInterfaces;
	int interfaceHeight = interfaceWidth;
	int interfaceWidth8th = interfaceWidth / 8;
	int interfaceHeight8th = interfaceHeight / 8;
	int numberOfRows = (NumberOfInterfaces % 2 == 0) ? NumberOfInterfaces / 2 : (NumberOfInterfaces + 1) / NumberOfInterfacesInRow;
	
	for(i = 0; i < NumberOfButtons; i++)
	{
		screen.buttons[i].rect.x = i * buttonWidth;
		screen.buttons[i].rect.y = 0;
		screen.buttons[i].rect.width = buttonWidth;
		screen.buttons[i].rect.height = buttonHeight;
		screen.buttons[i].value = 0;
	}
	
	for(i = 0; i < NumberOfInterfacesInRow; i++) 
	for(j = 0; j < numberOfRows; j++) 
	{
		int xPos = i * interfaceWidth;
		int yPos = j * interfaceHeight;
		int k = i + j * NumberOfInterfacesInRow;
		screen.interfaces[k].rect.x = xPos; 
		screen.interfaces[k].rect.y = yPos;
		screen.interfaces[k].rect.width = interfaceWidth; 
		screen.interfaces[k].rect.height = interfaceHeight;
		
		screen.interfaces[k].slider.rect.x = xPos + 1 * interfaceWidth8th;
		screen.interfaces[k].slider.rect.y = yPos + 1 * interfaceHeight8th;
		screen.interfaces[k].slider.rect.width = 2 * interfaceWidth8th;
		screen.interfaces[k].slider.rect.height = 6 * interfaceHeight8th;
		
		screen.interfaces[k].slider.mode = 1;		
		screen.interfaces[k].slider.sPos = 0;
		screen.interfaces[k].slider.sSize = 1 * interfaceHeight8th;
		
		screen.interfaces[k].button.rect.x = xPos + 4 * interfaceWidth8th;
		screen.interfaces[k].button.rect.y = yPos + 3 * interfaceHeight8th;
		screen.interfaces[k].button.rect.width = 2 * interfaceWidth8th;
		screen.interfaces[k].button.rect.height = 2 * interfaceHeight8th;
		screen.interfaces[k].button.value = 0;
	}				
}

/* Maximum task stack size */
#define lcdSTACK_SIZE			( ( unsigned portBASE_TYPE ) 256 )

/* Interrupt handlers */
extern void vLCD_ISREntry( void );
void vLCD_ISRHandler( void );

/* The LCD task. */
static void vLcdTask( void *pvParameters );

/* Semaphore for ISR/task synchronisation */
xSemaphoreHandle xLcdSemphr;

void vStartLcd(unsigned portBASE_TYPE uxPriority, xQueueHandle xQueue)
{
	static xQueueHandle xCmdQ;
	xCmdQ = xQueue;
	
	setupLedScreen();
	
	vSemaphoreCreateBinary(xLcdSemphr);
	
	xTaskCreate( vLcdTask, "Lcd", lcdSTACK_SIZE, &xCmdQ, uxPriority, ( xTaskHandle * ) NULL );
}

static portTASK_FUNCTION( vLcdTask, pvParameters )
{
	unsigned int pressure;
	unsigned int xPos;
	unsigned int yPos;
	portTickType xLastWakeTime;
	xQueueHandle xCmdQ;

	/* Just to stop compiler warnings. */
	( void ) pvParameters;

	printf("LEDControl Running\r\n");

	/* Initialise LCD display */
	/* NOTE: We needed to delay calling lcd_init() until here because it uses
	 * xTaskDelay to implement a delay and, as a result, can only be called from
	 * a task */
	lcd_init();

	drawLedScreen();

	/* Infinite loop blocks waiting for a touch screen interrupt event from
	 * the queue. */
	for( ;; )
	{
		/* Clear TS interrupts (EINT3) */
		/* Reset and (re-)enable TS interrupts on EINT3 */
		EXTINT = 8;						/* Reset EINT3 */

		/* Enable TS interrupt vector (VIC) (vector 17) */
		VICIntEnable = 1 << 17;			/* Enable interrupts on vector 17 */

		/* Block on a queue waiting for an event from the TS interrupt handler */
		xSemaphoreTake(xLcdSemphr, portMAX_DELAY);
				
		/* Disable TS interrupt vector (VIC) (vector 17) */
		VICIntEnClr = 1 << 17;

		/* +++ This point in the code can be interpreted as a screen button push event +++ */\
		/* No Push event yet */

		/* Measure next sleep interval from this point */
		xLastWakeTime = xTaskGetTickCount();

		/* Start polling the touchscreen pressure and position ( getTouch(...) ) */
		/* Keep polling until pressure == 0 */
		getTouch(&xPos, &yPos, &pressure);

		while (pressure > 0)
		{
			/* Get current pressure */
			getTouch(&xPos, &yPos, &pressure);

			/* Delay to give us a 25ms periodic TS pressure sample */
			vTaskDelayUntil( &xLastWakeTime, 25 );
		}		

		/* +++ This point in the code can be interpreted as a screen button release event +++ */
		updateLedScreen(xCmdQ, xPos, yPos);
	}
}

void vLCD_ISRHandler( void )
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	/* Process the touchscreen interrupt */
	/* We would want to indicate to the task above that an event has occurred */
	xSemaphoreGiveFromISR(xLcdSemphr, &xHigherPriorityTaskWoken);

	EXTINT = 8;					/* Reset EINT3 */
	VICVectAddr = 0;			/* Clear VIC interrupt */

	/* Exit the ISR.  If a task was woken by either a character being received
	or transmitted then a context switch will occur. */
	portEXIT_SWITCHING_ISR( xHigherPriorityTaskWoken );
}

