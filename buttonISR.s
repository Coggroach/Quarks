	INCLUDE portmacro.inc
	
	IMPORT vBUTTON_ISRHandler
	EXPORT vBUTTON_ISREntry

	;/* Interrupt entry must always be in ARM mode. */
	ARM
	AREA	|.text|, CODE, READONLY


vBUTTON_ISREntry

	PRESERVE8

	; Save the context of the interrupted task.
	portSAVE_CONTEXT			

	; Call the C handler function - defined within lcd.c.
	LDR R0, =vBUTTON_ISRHandler
	MOV LR, PC				
	BX R0

	; Finish off by restoring the context of the task that has been chosen to 
	; run next - which might be a different task to that which was originally
	; interrupted.
	portRESTORE_CONTEXT

	END