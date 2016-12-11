#ifndef BOARDBUTTONS_H
#define BOARDBUTTONS_H

#include "queue.h"
#include "semphr.h"

void vStartButtonsTask( unsigned portBASE_TYPE uxPriority, xQueueHandle xQueue, xSemaphoreHandle xMutex);

#endif
