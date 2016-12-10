#include "FreeRTOS.h"
#include "LCDControl.h"
#include "Graphics.h"
#include "LCDGraphics.h"
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

static portTASK_FUNCTION( vLcdTask, pvParameters )
{
	unsigned int pressure;
	unsigned int xPos;
	unsigned int yPos;
	portTickType xLastWakeTime;
	int ComponentTouchedId = ~(0x0);
	Slider* slider = 0;
	Button* button = 0;

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
		
		ComponentTouchedId = isComponentTouched(xPos, yPos);
		button = getButton(ComponentTouchedId);
		slider = getSlider(ComponentTouchedId);
		
		if(button) {
			handleButton(button);
			drawButtonPointer(button);
		}

		while (pressure > 0)
		{
			/* Get current pressure */
			getTouch(&xPos, &yPos, &pressure);
			
			if(slider) {
				handleSlider(slider, getPoint(xPos, yPos));
				drawSliderPointer(slider);
			}

			/* Delay to give us a 25ms periodic TS pressure sample */
			vTaskDelayUntil( &xLastWakeTime, 25 );
		}		

		/* +++ This point in the code can be interpreted as a screen button release event +++ */
		if(button) {
			handleButton(button);
			drawButtonPointer(button);
		}
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

