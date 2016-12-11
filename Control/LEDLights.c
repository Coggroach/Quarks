#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <stdio.h>
#include "lpc24xx.h"
#include "LCDControl.h"
#include "LEDLights.h"
#include "LCDGraphics.h"
#include "Messages.h"

#define I2C_AA      0x00000004
#define I2C_SI      0x00000008
#define I2C_STO     0x00000010
#define I2C_STA     0x00000020
#define I2C_I2EN    0x00000040

/* FreeRTOS definitions */
#define consumerTaskSTACK_SIZE			( ( unsigned portBASE_TYPE ) 256 )
	
/* The Consumer task. */
static void vConsumerTask( void *pvParameters );
static xQueueHandle xCmdQ;

static LedMessage lastMessage;

static void setupPinConfig(void)	
{
	/* Enable and configure I2C0 */
	PCONP    |=  (1 << 7);                /* Enable power for I2C0              */

	/* Initialize pins for SDA (P0.27) and SCL (P0.28) functions                */
	PINSEL1  &= ~0x03C00000;
	PINSEL1  |=  0x01400000;

	/* Clear I2C state machine                                                  */
	I20CONCLR =  I2C_AA | I2C_SI | I2C_STA | I2C_I2EN;
	
	/* Setup I2C clock speed                                                    */
	I20SCLL   =  0x80;
	I20SCLH   =  0x80;
	
	I20CONSET =  I2C_I2EN;
}

void vStartLightsTask( unsigned portBASE_TYPE uxPriority, xQueueHandle xQueue)
{
    /* We're going to pass a pointer to the new task's parameter block. We don't want the
       parameter block (in this case just a queue handle) that we point to to disappear
       during the lifetime of the task. Declaring the parameter block as static
       achieves this. */    
  xCmdQ = xQueue;
	
	setupPinConfig();

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
	
	printf("Starting sensor poll ...\r\n");
	
	/* initial xLastWakeTime for accurate polling interval */
	xLastWakeTime = xTaskGetTickCount();
	
	/* pvParameters is actually a pointer to an xQueueHandle. Cast it and then dereference it to save it for later use. */
  //xCmdQ = * ( ( xQueueHandle * ) pvParameters );
	
	while(1)
	{
		/* Get command from Q */
	  xQueueReceive(xCmdQ, &m, portMAX_DELAY);			
		
		/* Check what needs Updating */
		d = ((m.mode & UpdateData) == UpdateData) ? m.data : lastMessage.data;
		p0 = ((m.mode & UpdatePulse0) == UpdatePulse0) ? m.pulse0 : lastMessage.pulse0;
		p1 = ((m.mode & UpdatePulse1) == UpdatePulse1) ? m.pulse1 : lastMessage.pulse1;
		
		/* Update Lights */
		setLEDs(d, p0, p1);
		
		/* Update Last Message */
		lastMessage = m;
		
		vTaskDelayUntil( &xLastWakeTime, 10);
	}
}
