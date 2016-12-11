#ifndef LEDLIGHTS_H
#define LEDLIGHTS_H

#include "queue.h"
#include "semphr.h"

void vStartLightsTask( unsigned portBASE_TYPE uxPriority, xQueueHandle xQueue, xSemaphoreHandle xMutex);

#endif
