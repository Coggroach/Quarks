#ifndef LEDLIGHTS_H
#define LEDLIGHTS_H

#include "queue.h"

void vStartLightsTask( unsigned portBASE_TYPE uxPriority, xQueueHandle xQueue, xQueueHandle xMutex);

#endif
