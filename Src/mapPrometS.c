#include "main.h"

#include "cmsis_os.h"

#include "map.h"



static osMutexId MapMutexHandle;



void map_Init(void)
{
	osMutexDef(MapMutex);
	MapMutexHandle = osMutexCreate(osMutex(MapMutex));

}

