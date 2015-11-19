#include "target.h"
#include "delay.h"

#define IS_AT_VPOS (IO_VSignal == 0)
#define IS_AT_HPOS (IO_HSignal == 0)
#define MOTOR_ON    IO_Motor = 0;IO_Motor  = 1
#define MOTOR_OFF		 IO_Motor = 0

void TGT_Fall()
{
	  if(!IS_AT_HPOS)   
			{
				  MOTOR_ON;
						while(!IS_AT_HPOS);
						MOTOR_OFF;
			}
}


void TGT_Stand()
{
			if(!IS_AT_VPOS)
			{
				  MOTOR_ON;
						while(!IS_AT_VPOS);
						MOTOR_OFF;
		 }		
}


void TGT_Revive()
{
		if(!IS_AT_HPOS)
			  MOTOR_ON;

			while(!IS_AT_HPOS);

			MOTOR_OFF;

			DelayTicks(50);

			if(!IS_AT_VPOS)
		   MOTOR_ON;

			while(!IS_AT_VPOS);

			MOTOR_OFF;
}