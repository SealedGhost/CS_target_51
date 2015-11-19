#include "delay.h"

extern uint16_t Cnt50ms;

void DelayTicks(uint16_t ticks)
{
	  uint16_t resCnt  = ticks + Cnt50ms;
			while(resCnt != Cnt50ms);
}