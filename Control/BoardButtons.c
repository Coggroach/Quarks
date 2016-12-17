#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "lpc24xx.h"
#include "Messages.h"
#include "BoardButtons.h"
#include "I2C.h"
#include "Verbose.h"


/* FreeRTOS definitions */
#define buttonsTaskSTACK_SIZE			( ( unsigned portBASE_TYPE ) 256 )
	
/* The Consumer task. */
static void vButtonsTask( void *pvParameters );

static xQueueHandle xCmdQ;
static xQueueHandle xMutex;

void vStartButtonsTask( unsigned portBASE_TYPE uxPriority, xQueueHandle xQueue, xQueueHandle xSemphr)
{
    /* We're going to pass a pointer to the new task's parameter block. We don't want the
       parameter block (in this case just a queue handle) that we point to to disappear
       during the lifetime of the task. Declaring the parameter block as static
       achieves this. */    
  xCmdQ = xQueue;
	xMutex = xSemphr;	

	/* Spawn the console task . */
	xTaskCreate( vButtonsTask, "Buttons", buttonsTaskSTACK_SIZE, &xCmdQ, uxPriority, ( xTaskHandle * ) NULL );
}

/* Get I2C button status */
unsigned char getBoardButtons()
{
	unsigned char ledData;

	/* Initialise */
	I20CONCLR =  I2C_AA | I2C_SI | I2C_STA | I2C_STO;
	
	/* Request send START */
	I20CONSET =  I2C_STA;

	/* Wait for START to be sent */
	while (!(I20CONSET & I2C_SI));

	/* Request send PCA9532 ADDRESS and R/W bit and clear SI */		
	I20DAT    =  0xC0;
	I20CONCLR =  I2C_SI | I2C_STA;

	/* Wait for ADDRESS and R/W to be sent */
	while (!(I20CONSET & I2C_SI));

	/* Send control word to read PCA9532 INPUT0 register */
	I20DAT = 0x00;
	I20CONCLR =  I2C_SI;

	/* Wait for DATA with control word to be sent */
	while (!(I20CONSET & I2C_SI));

	/* Request send repeated START */
	I20CONSET =  I2C_STA;
	I20CONCLR =  I2C_SI;

	/* Wait for START to be sent */
	while (!(I20CONSET & I2C_SI));

	/* Request send PCA9532 ADDRESS and R/W bit and clear SI */		
	I20DAT    =  0xC1;
	I20CONCLR =  I2C_SI | I2C_STA;

	/* Wait for ADDRESS and R/W to be sent */
	while (!(I20CONSET & I2C_SI));

	I20CONCLR = I2C_SI;

	/* Wait for DATA to be received */
	while (!(I20CONSET & I2C_SI));

	ledData = I20DAT;

	/* Request send NAQ and STOP */
	I20CONSET =  I2C_STO;
	I20CONCLR =  I2C_SI | I2C_AA;

	/* Wait for STOP to be sent */
	while (I20CONSET & I2C_STO);

	return ledData ^ 0xf;
}


static portTASK_FUNCTION( vButtonsTask, pvParameters )	
{
	portTickType xLastWakeTime;
	unsigned char buttonState;
	unsigned char lastButtonState;
	unsigned char changedState;
	unsigned int i;	
	unsigned char mask;
	int mutex;
	LedMessage m;
	
	/* Just to stop compiler warnings. */
	( void ) pvParameters;
	
	printf(ConsoleLogActionDescription, ActionStart, "BoardButtons");
	printf(ConsoleUnderline);

	lastButtonState = 0;
	
	/* Measure next sleep interval from this point */
	xLastWakeTime = xTaskGetTickCount();
	
	/* Button Poll */
	while( 1 )
	{
		/* Block on a Mutex */		
		xQueueReceive(xMutex, &mutex, portMAX_DELAY);
		
		/* Verbose */
		#if(xMutexVerbose == 1)			
			printf(MutexMessageAction, "BoardButtons", ActionReceived, mutex);			
		#endif
		
		/* Read buttons */
		buttonState = getBoardButtons();
				
		/* Give mutex back */
		xQueueSendToFront(xMutex, &mutex, portMAX_DELAY);
		
		/* Verbose */
		#if(xMutexVerbose == 1)
			printf(MutexMessageAction, "BoardButtons", ActionSent, mutex);		
		#endif
		
		changedState = buttonState ^ lastButtonState;
		if (buttonState != lastButtonState)
		{
			/* Verbose */
			#if(BoardButtonsVerbose == 1)
				printf("[BoardButtons, ChangedState]: %i\r\n", changedState);
			#endif
			
			/* Logic for Message */
			if((changedState & UpdateData) == UpdateData)
				m.mode = UpdateData;
			else if((changedState & UpdateData0) == UpdateData0)
				m.mode = UpdateData0;
			else if((changedState & UpdateData1) == UpdateData1)
				m.mode = UpdateData1;
			else if((changedState & UpdateData2) == UpdateData2)
				m.mode = UpdateData2;
			else if((changedState & UpdateData3) == UpdateData3)
				m.mode = UpdateData3;			
			
			m.data = 
						((buttonState & UpdateData0)) 
					| ((buttonState & UpdateData1) << 2)
					| ((buttonState & UpdateData2) << 4)
					| ((buttonState & UpdateData3) << 6);
			
			/* Send Message to Queue */
			xQueueSendToBack(xCmdQ, &m, portMAX_DELAY);		
			#if(BoardButtonsVerbose == 1)
				printf(LedMessageSenderAction, "BoardButtons", ActionSent, m.mode, m.data, 0, 0);
			#endif
			
			lastButtonState = buttonState;
		}
		vTaskDelayUntil( &xLastWakeTime, 10);
	}
}
