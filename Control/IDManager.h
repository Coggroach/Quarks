#ifndef IDMANAGER_H
#define IDMANAGER_H

#include "Components.h"

unsigned int Register(ComponentType t);
ComponentType GetComponent(unsigned int id);

#endif
