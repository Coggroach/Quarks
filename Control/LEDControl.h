#ifndef LEDCONTROL_H
#define LEDCONTROL_H

#include "queue.h"

void vStartLcd(unsigned portBASE_TYPE uxPriority, xQueueHandle xQueue);

#endif
