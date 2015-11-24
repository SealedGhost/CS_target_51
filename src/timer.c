#include "timer.h"
#include <STC12C5A60S2.h>
#include <stdio.h>
uint8_t TMsgFlags  = 0;

volatile uint16_t  Cnt50ms  = 0;

static TIM_MSG TMsgQueue[TMSG_SIZE]	= 0;



void TIM0_Init(void)		//50∫¡√Î@11.0592MHz
{
	 AUXR &= 0x7F;		//?????12T??
	 TMOD &= 0xF0;		//???????
	 TMOD |= 0x01;		//???????
 	TL0 = 0x00;		//??????
	 TH0 = 0x4C;		//??????
	 TF0 = 0;		//??TF0??
		ET0   = 1;
	 EA    = 1;
		TR0   = 1;
}




Bool TIM_UpdateTimer( uint8_t msg_id, uint16_t ticks )
{
	  uint16_t i  = 0;

			for(; i<TMSG_SIZE; i++)
			{
				  if(TMsgQueue[i].MsgID == msg_id)
						{
							  TMsgFlags  &= (~msg_id);
									TMsgQueue[i].Ticks  = ticks + Cnt50ms;
									TMsgQueue[i].State  = ENABLE;
									return TRUE;
						}
			}
			return FALSE;
}

Bool TIM_CreateTimer( uint8_t msg_id, uint16_t ticks )
{
			uint16_t i  = 0;

			if( TIM_UpdateTimer(msg_id, ticks)	)
			   return TRUE;
			else
			{
				 for(; i<TMSG_SIZE; i++)
					{
						 if(TMsgQueue[i].MsgID == 0)
							{
							  TMsgFlags  &= (~msg_id);
								 TMsgQueue[i].MsgID  = msg_id;
									TMsgQueue[i].Ticks  = ticks + Cnt50ms;
									TMsgQueue[i].State  = ENABLE;
									return TRUE;
							}
					}
			}
			return FALSE;
}


void TIM_DeleteTimer(uint8_t msg_id)
{
	 uint16_t  i  = 0;

		for(; i<TMSG_SIZE; i++)
		{
			 if(TMsgQueue[i].MsgID == msg_id)
				{
					 TMsgFlags  &= (~msg_id);
						TMsgQueue[i].MsgID  = 0;
						TMsgQueue[i].Ticks  = 0;
						TMsgQueue[i].State  = DISABLE;
				}
		}
}


void Timer0_IRQ() interrupt 1
{
   uint16_t  i ;

   TR0   = 0;
   TF0   = 0;
	 	TL0 = 0x00;		//??????
	 	TH0 = 0x4C;		//??????

	  Cnt50ms++;
   for(i=0; i<TMSG_SIZE; i++)
			{
				 if(TMsgQueue[i].MsgID && TMsgQueue[i].State)
					{
						 if(TMsgQueue[i].Ticks == Cnt50ms)
							{
								 TMsgFlags  |= TMsgQueue[i].MsgID;
									TMsgQueue[i].State  = DISABLE;
							}
					}
			}
			TR0  = 1;
}


