#ifndef VERBOSE_H
#define VERBOSE_H

#define MainProgramVerbose 			1


#define BoardButtonsVerbose 		0
#define LCDControlVerbose 			0
#define LEDLightsVerbose 				0

#define LCDGraphicsVerbose 			0
#define GraphicsVerbose 				0

#define xMutexVerbose 					0
#define xQueueVerbose						0

#define ConsoleUnderline "===========================\r\n"

#define ActionSent 			"Sent"
#define ActionStart 		"Starting"
#define ActionError 		"Error"
#define ActionReceived	"Received"

#define ConsoleLogActionDescription			"[%s]: %s \r\n"
#define LedMessageSenderAction					"[%s, %s]: %i, %i, %i, %i \r\n"
#define MutexMessageAction							"[%s, %s]: %i \r\n"

#endif

