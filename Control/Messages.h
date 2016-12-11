#ifndef MESSAGES_H
#define MESSAGES_H

#define UpdateData 0x1
#define UpdatePulse0 0x2
#define UpdatePulse1 0x4

typedef struct LedMessage
{	
	unsigned char mode;
	unsigned char data, pulse0, pulse1;	
} LedMessage;

#endif
