#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "lpc24xx.h"
#include "LEDLights.h"
#include "Messages.h"
#include "I2C.h"

/* FreeRTOS definitions */
#define consumerTaskSTACK_SIZE			( ( unsigned portBASE_TYPE ) 256 )
	
/* The Consumer task. */
static void vConsumerTask( void *pvParameters );
static xQueueHandle xCmdQ;

static LedMessage lastMessage;
static xSemaphoreHandle xMutex;

void vStartLightsTask( unsigned portBASE_TYPE uxPriority, xQueueHandle xQueue, xSemaphoreHandle xSemphr)
{
    /* We're going to pass a pointer to the new task's parameter block. We don't want the
       parameter block (in this case just a queue handle) that we point to to disappear
       during the lifetime of the task. Declaring the parameter block as static
       achieves this. */    
  xCmdQ = xQueue;
	xMutex = xSemphr;

	/* Spawn the console task . */
	xTaskCreate( vConsumerTask, "Consumer", consumerTaskSTACK_SIZE, &xCmdQ, uxPriority, ( xTaskHandle * ) NULL );
}

void setLEDs(unsigned char data, unsigned char pwm0, unsigned char pwm1)
{
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

	/* Send Offset */
	I20DAT = 0x12;
	I20CONCLR =  I2C_SI | I2C_STA;

	/* Wait for DATA with control word to be sent */
	while (!(I20CONSET & I2C_SI));
	
	/* PWM0 is on (not blinking) */
	I20DAT    =  0x00;
  I20CONCLR =  I2C_SI | I2C_STA;
 
  while (!(I20CONSET & I2C_SI));
	
	/* PWM0 brightness level */
  I20DAT    =  pwm0;
  I20CONCLR =  I2C_SI | I2C_STA;
 
  while (!(I20CONSET & I2C_SI));
	
  // PWM1 is on (not blinking)
  I20DAT    =  0x00;
  I20CONCLR =  I2C_SI | I2C_STA;
 
  while (!(I20CONSET & I2C_SI));
   
  //PWM1 brightness level
  I20DAT    =  pwm1;
  I20CONCLR =  I2C_SI | I2C_STA;
 
  while (!(I20CONSET & I2C_SI));
	
	// leave blank (not used)
  I20DAT    =  0x00;
  I20CONCLR =  I2C_SI | I2C_STA;
 
  while (!(I20CONSET & I2C_SI));
   
  // leave blank (not used)
  I20DAT    =  0x00;
  I20CONCLR =  I2C_SI | I2C_STA;
 
  while (!(I20CONSET & I2C_SI));
	
	/* Send Data word to read PCA9532 INPUT0 register */
	I20DAT = data;
	I20CONCLR =  I2C_SI;

	/* Wait for DATA with control word to be sent */
  while (!(I20CONSET & I2C_SI));

  // leave blank (not used)
  I20DAT    =  0x00;
  I20CONCLR =  I2C_SI | I2C_STA;
 
  while (!(I20CONSET & I2C_SI));

	/* End of Input */
  I20CONSET =  I2C_STO;
  I20CONCLR =  I2C_SI | I2C_AA;
 
  while (I20CONSET & I2C_STO);
}

static portTASK_FUNCTION( vConsumerTask, pvParameters )	
{
	portTickType xLastWakeTime;	
	LedMessage m;
	unsigned char d, p0, p1;
	
	/* Just to stop compiler warnings. */
	( void ) pvParameters;
	
	printf("Starting LEDs task ...\r\n");
	
	/* initial xLastWakeTime for accurate polling interval */
	xLastWakeTime = xTaskGetTickCount();
	
	/* pvParameters is actually a pointer to an xQueueHandle. Cast it and then dereference it to save it for later use. */
  //xCmdQ = * ( ( xQueueHandle * ) pvParameters );
	
	while(1)
	{
		/* Get command from Q */
	  xQueueReceive(xCmdQ, &m, portMAX_DELAY);
		printf("Received Message: %i, %i, %i, %i \r\n", m.mode, m.data, m.pulse0, m.pulse1);		
		
		/* Check what needs Updating */
		if((m.mode & UpdateData) == UpdateData) 		
			d = m.data;
		else if((m.mode & UpdateData0) == UpdateData0)
			d = (m.data & UpdateData0) | ((lastMessage.data & ~(UpdateData0)) & UpdateData);
		else if((m.mode & UpdateData1) == UpdateData1)
			d = (m.data & UpdateData1) | ((lastMessage.data & ~(UpdateData1)) & UpdateData);
		else if((m.mode & UpdateData2) == UpdateData2)			
			d = (m.data & UpdateData2) | ((lastMessage.data & ~(UpdateData2)) & UpdateData);
		else if((m.mode & UpdateData3) == UpdateData3)
			d = (m.data & UpdateData3) | ((lastMessage.data & ~(UpdateData3)) & UpdateData);
		else
			d = lastMessage.data;
					
		p0 = ((m.mode & UpdatePulse0) == UpdatePulse0) ? m.pulse0 : lastMessage.pulse0;
		p1 = ((m.mode & UpdatePulse1) == UpdatePulse1) ? m.pulse1 : lastMessage.pulse1;
		
		
		/* Block on a mutex */
		//xSemaphoreTake(xMutex, portMAX_DELAY);
		
		/* Update Lights */
		printf("Setting LEDs: %i, %i, %i\r\n", d, p0, p1);
		setLEDs(d, p0, p1);
		
		/* Give mutex back */
		//xSemaphoreGive(xMutex);
		
		/* Update Last Message */
		lastMessage.data = d;
		lastMessage.pulse0 = p0;
		lastMessage.pulse1 = p1;
		
		vTaskDelayUntil( &xLastWakeTime, 10);
	}
}
