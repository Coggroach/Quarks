#ifndef LCDCONTROL_H
#define LCDCONTROL_H

#include "queue.h"

void vStartLcd(unsigned portBASE_TYPE uxPriority, xQueueHandle xQueue);

#endif
