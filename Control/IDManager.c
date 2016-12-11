#include "IDManager.h"
#include "LCDGraphics.h"

ComponentType types[NumberOfComponents];
int latestId = 0;

unsigned int Register(ComponentType t)
{
	unsigned int id = latestId;
	types[id] = t;
	latestId++;	
	return id;
}

ComponentType GetComponent(unsigned int id)
{
	return types[id];
}
