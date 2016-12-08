#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "lpc24xx.h"
#include "LEDControl.h"
#include "LEDLights.h"

#define I2C_AA      0x00000004
#define I2C_SI      0x00000008
#define I2C_STO     0x00000010
#define I2C_STA     0x00000020
#define I2C_I2EN    0x00000040

/* FreeRTOS definitions */
#define consumerTaskSTACK_SIZE			( ( unsigned portBASE_TYPE ) 256 )
	
/* The Consumer task. */
static void vConsumerTask( void *pvParameters );

static LedLogic interfaces[NumberOfInterfaces];

static void setupLedLogic(void)
{
	int j;	
	for(j = 0; j < NumberOfInterfaces; j++) 
	{				
		interfaces[j].slider = 0;		
		interfaces[j].button = 0;
	}				
}

static void updateLedLogic(LedMessage m)
{
	int i;
	interfaces[m.id].button = m.enable;
	interfaces[m.id].slider = m.intensity;
	for(i = 0; i < NumberOfInterfaces; i++) 
	{
		interfaces[i].update = 0;
	}
	interfaces[m.id].update = 1;
}

unsigned short createLedData()
{
	unsigned short i, data = 0;	
	for(i = 0; i < NumberOfInterfaces; i++) 
	{
		data |= (interfaces[i].button) << (2 * i);
	}
	return data;
}

void vStartLightsTask( unsigned portBASE_TYPE uxPriority, xQueueHandle xQueue)
{
    /* We're going to pass a pointer to the new task's parameter block. We don't want the
       parameter block (in this case just a queue handle) that we point to to disappear
       during the lifetime of the task. Declaring the parameter block as static
       achieves this. */
  static xQueueHandle xCmdQ;
    
  xCmdQ = xQueue;
	
	setupLedLogic();

	/* Spawn the console task . */
	xTaskCreate( vConsumerTask, "Consumer", consumerTaskSTACK_SIZE, &xCmdQ, uxPriority, ( xTaskHandle * ) NULL );
}

unsigned char setLEDs()
{
	unsigned char ledData = createLedData();

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
  I20DAT    =  0; // PWM0 level
  I20CONCLR =  I2C_SI | I2C_STA;
 
  while (!(I20CONSET & I2C_SI));
	
  // PWM1 is on (not blinking)
  I20DAT    =  0x00;
  I20CONCLR =  I2C_SI | I2C_STA;
 
  while (!(I20CONSET & I2C_SI));
   
  //PWM1 brightness level
  I20DAT    =  0; //PWM1 level
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
	I20DAT = ledData;
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

	return ledData ^ 0xf;
}

static portTASK_FUNCTION( vConsumerTask, pvParameters )	
{
	xQueueHandle xCmdQ;
	LedMessage message;
	
	/* pvParameters is actually a pointer to an xQueueHandle. Cast it and then dereference it to save it for later use. */
  xCmdQ = * ( ( xQueueHandle * ) pvParameters );
	
	while(1)
	{
		/* Get command from Q */
	  xQueueReceive(xCmdQ, &message, portMAX_DELAY);		
		
		/* Update Local Data */
		updateLedLogic(message);		
		
		/* Update Lights */
		setLEDs();
	}
}
