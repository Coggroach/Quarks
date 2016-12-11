#include "FreeRTOS.h"
#include "LCDControl.h"
#include "LCDGraphics.h"
#include "IDManager.h"
#include "Messages.h"
#include "task.h"
#include "semphr.h"
#include "lcd_hw.h"
#include <stdio.h>
#include <string.h>


/* Maximum task stack size */
#define lcdSTACK_SIZE			( ( unsigned portBASE_TYPE ) 256 )

/* Interrupt handlers */
extern void vLCD_ISREntry( void );
void vLCD_ISRHandler( void );

/* The LCD task. */
static void vLcdTask( void *pvParameters );

/* Semaphore for ISR/task synchronisation */
xSemaphoreHandle xLcdSemphr;
static xQueueHandle xCmdQ;

void vStartLcd(unsigned portBASE_TYPE uxPriority, xQueueHandle xQueue)
{	
	xCmdQ = xQueue;
	
	setupLedScreen();	
	vSemaphoreCreateBinary(xLcdSemphr);
	
	xTaskCreate( vLcdTask, "Lcd", lcdSTACK_SIZE, &xCmdQ, uxPriority, ( xTaskHandle * ) NULL );
}

static LedMessage getLedMessage(int id)
{
	LedMessage m;		

	m.mode = getLedMode(id);
	
	if((m.mode & UpdateData) == UpdateData)
		m.data = getLedData();
	if((m.mode & UpdatePulse0) == UpdatePulse0)
		m.pulse0 = getLedPulse0();
	if((m.mode & UpdatePulse1) == UpdatePulse1)
		m.pulse1 = getLedPulse1();
	
	xQueueSendToBack(xCmdQ, &m, portMAX_DELAY);
	printf("Sent Message: %i, %i, %i, %i \r\n", m.mode, m.data, m.pulse0, m.pulse1);
	return m;
}

static portTASK_FUNCTION( vLcdTask, pvParameters )
{
	unsigned int pressure;
	unsigned int xPos;
	unsigned int yPos;
	portTickType xLastWakeTime;
	int ComponentTouchedId = ~(0x0);
	ComponentType cType;
	LedMessage m;
	Slider* slider = 0;
	Button* button = 0;

	/* Just to stop compiler warnings. */
	( void ) pvParameters;

	printf("LEDControl task started ...\r\n");
	printf("===========================\r\n");

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

		/* Measure next sleep interval from this point */
		xLastWakeTime = xTaskGetTickCount();

		/* Start polling the touchscreen pressure and position ( getTouch(...) ) */
		/* Keep polling until pressure == 0 */
		getTouch(&xPos, &yPos, &pressure);
		
		/* Get Touched Component */
		ComponentTouchedId = isComponentTouched(xPos, yPos);
		cType = GetComponent(ComponentTouchedId);
		
		switch(cType)
		{
			case PresetType:
			case ButtonType:
				button = getButton(ComponentTouchedId);
				break;
			case SliderType:
				slider = getSlider(ComponentTouchedId);
				break;
			default:
				break;
		}				
		
		/* Down Event */
		if(button) {
			if(!doesRectContainPoint(button->rect, getPoint(xPos, yPos)))
					continue;
			
			/* Handle if Preset */
			if(cType == PresetType) {
				/* Handle Preset */
				handlePreset(button);
				drawButtonPointer(button);
				m = getLedMessage(ComponentTouchedId);
				drawLights(m);
				drawButtons();
				drawSliders();
				//drawLedScreen();				
				//vTaskDelayUntil(&xLastWakeTime, 25);
				continue;
			}
			
			/* Handle Button */
			handleButton(button);
			
			/* Draw Button */
			drawButtonPointer(button);
			
			/* Handle and Draw Lights */
			m = getLedMessage(ComponentTouchedId);
			drawLights(m);
		}

		/* Drag Event */
		while (pressure > 0)
		{
			/* Get current pressure */
			getTouch(&xPos, &yPos, &pressure);
			
			/* Pressed Slider */
			if(slider) {
				if(!doesRectContainPoint(slider->rect, getPoint(xPos, yPos)))
					continue;
				
				/* Handle and Draw Slider */
				handleSlider(slider, getPoint(xPos, yPos));
				drawSliderPointer(slider);
				
				printf("Slider: %i, %i\r\n", slider->id, slider->sPos);
				
				/* Handle and Draw Lights */
				m = getLedMessage(ComponentTouchedId);
				drawLights(m);
			}
			
			/* Delay to give us a 25ms periodic TS pressure sample */
			vTaskDelayUntil(&xLastWakeTime, 25);
		}		

		/* Up Event */
		printf("===========================\r\n");
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

