#ifndef _TIMER_H
#define _TIERM_H
#include "types.h"


#define TMSG_SIZE  8

#define TMSG_TIMER	 0x01


typedef struct 
{
	 uint8_t  MsgID;
		uint8_t  State;
		uint16_t Ticks;
}	TIM_MSG;

void TIM0_Init(void);
Bool TIM_UpdateTimer(uint8_t msg_id, uint16_t ticks);
Bool TIM_CreateTimer(uint8_t msg_id, uint16_t ticks);
void TIM_DeleteTimer(uint8_t msg_id);

#endif