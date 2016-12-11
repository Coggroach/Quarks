#ifndef MESSAGES_H
#define MESSAGES_H

#define UpdateData0 0x1
#define UpdateData1 0x2
#define UpdateData2 0x4
#define UpdateData3 0x8
#define UpdateData (UpdateData0 | UpdateData1 | UpdateData2 | UpdateData3)
#define UpdatePulse0 0x10
#define UpdatePulse1 0x20

typedef struct LedMessage
{	
	unsigned char mode;
	unsigned char data, pulse0, pulse1;	
} LedMessage;

#endif
