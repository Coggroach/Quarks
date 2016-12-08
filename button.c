/* 
	Sample task that initialises the EA QVGA LCD display
	with touch screen controller and processes touch screen
	interrupt events.

	Jonathan Dukes (jdukes@scss.tcd.ie)
*/

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "button.h"
#include <stdio.h>
#include <string.h>

/* Maximum task stack size */
#define buttonSTACK_SIZE			( ( unsigned portBASE_TYPE ) 256 )

/* Interrupt handlers */
extern void vBUTTON_ISREntry( void );
void vBUTTON_ISRHandler( void );

xQueueHandle xButtonQ;			/* Button event queue */
unsigned int uPressed = 0;		/* Button state */


/* Setup button interrupts. There is no tash associated with the button ...
 * it just sends button events to a queue. */
xQueueHandle xSetupButton( void )
{
	/* Create queue */
	xButtonQ = xQueueCreate( 16, sizeof(unsigned int) );

	/* Initialise Button Hardware */
	PINSEL4 |= 1 << 20;			/* Enable P2.10 for EINT0 function */
	EXTMODE |= 1;				/* EINT0 edge-sensitive mode */
	EXTPOLAR &= ~1;				/* Falling rising-edge mode for EINT0 initially */
	EXTINT = 1;					/* Reset EINT0 */

	VICIntSelect &= ~(1 << 14);	/* Configure vector 14 (EINT0) for IRQ */
	VICVectPriority14 = 15;		/* Set priority 15 (lowest) for vector 14 */
	VICVectAddr14 = (unsigned long)vBUTTON_ISREntry;
								/* Set handler vector */
	VICIntEnable |= 1 << 14;	/* Enable interrupts on vector 14 */

	return xButtonQ;
}


void vBUTTON_ISRHandler( void )
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	VICIntEnClr |= 1 << 14;	    	/* Disable interrupts on vector 14 */

    if (!uPressed)
	{
	    EXTPOLAR |= 1;				/* EINT0 falling edge mode */    
		uPressed = 1;
    }
    else
    {
	    EXTPOLAR &= ~1;				/* EINT0 rising edge mode */
 		uPressed = 0;
   }
    
	/* Send an integer to the queue to indicate the event.
	 * 1=button down event; 0=button up event */	
	xQueueSendFromISR( xButtonQ, &uPressed, &xHigherPriorityTaskWoken  );

	EXTINT = 1;			    		/* Reset EINT0 */
	VICIntEnable |= 1 << 14;		/* Enable interrupts on vector 14 */

	VICVectAddr = 0;		    	/* Clear VIC interrupt source */
		
	/* Exit the ISR.  If a task was woken by either a character being received
	or transmitted then a context switch will occur. */
	portEXIT_SWITCHING_ISR( xHigherPriorityTaskWoken );
}


