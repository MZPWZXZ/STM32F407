#include "port.h"


void EnterCriticalSection( )
{
	__disable_irq();
}



void ExitCriticalSection( )
{
    __enable_irq();
}
