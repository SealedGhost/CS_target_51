#include <STC12C5A60S2.h>
#include "usart.h"
#include "game.h"
#include "delay.h"
#include "types.h"
#include "stdio.h"
#include "timer.h"
#include "conf.h"
#include "target.h"

extern uint8_t TMsgFlags;
extern  hasNewMsg  ;
extern uint16_t Cnt50ms;
extern uint8_t UART_Buf[RXBUF_SIZE];

sbit KeyState     = P2^5;
sbit SensorState  = P2^7;

char mode_str[4][6] = {
	                       "Init",
	                       "Prct",
	                       "Shot",
	                       "Cmpt"
                      };
char stt_str[5][8] =  {
																								"Stop",
																								"Ack",
																								"Pause",
																								"Resume",
																								"Start"
                      };
const unsigned char ID  = 0x01;


void ACK_Reply(void);
void ReturnRes(void);

uint8_t TimeOutVal  = 0;


GAME_MODE GameMode  = GMODE_Init;
GAME_STT  GameStt   = GSTT_Stop;
GAME_STT  OldStt    = GSTT_Stop;



void main()
{
	 IO_LEDv5  = 0;
	 IO_LEDv12 = 0;
	
	 TIM0_Init(); 	
		UART1_Init();
		UART1_Cmd(ENABLE);
	 
	 TGT_Revive(); 
	
  while(1)
  {			
				if(UARTMsgFlags&UART_POST)
				{				 					
					 UARTMsgFlags  &= (~UART_POST);
							
					 if(UART_Buf[INDEX_ID] == 0x00  ||  UART_Buf[INDEX_ID] == ID)
						{
						  	IO_LEDv12  = 1;
							  DelayTicks(10);
							  IO_LEDv12  = 0;
							
							  if(UART_Buf[INX_MOD]<=GMODE_Cmpt)
									{
										  GameMode  = UART_Buf[INX_MOD];
//									  	printf("mod:%d %s\n",(int)GameMode, mode_str[UART_Buf[INX_MOD]]);
									}
									
									if(UART_Buf[INX_STT] <= GSTT_Start)
									{
										  GameStt  = UART_Buf[INX_STT];
//												printf("stt:%d %s\n",(int)GameStt, stt_str[UART_Buf[INX_STT]]);
									}
									
									TimeOutVal  = UART_Buf[INX_VAL];
									
									switch(GameStt)
									{
										  case GSTT_Stop:
												     TimeOutVal  = 0;
																	TIM_DeleteTimer(TMSG_TIMER);
																	/// Target fall. 
												     TGT_Fall();
												     break;
												case GSTT_Ack:
													    if(OldStt != GSTT_Ack)
																	{
																			TimeOutVal  = 0;
																			TIM_DeleteTimer(TMSG_TIMER);
																			/// Target fall.
																			TGT_Fall();																		
																	}
																	
																	/// Reply.
												     ACK_Reply();
												     break;
												case GSTT_Pause:
																	TIM_DeleteTimer(TMSG_TIMER);
                 // Target fall.		 
                 TGT_Fall();												
																	break;
												case GSTT_Resume:
												     /// Target stand.
												     TGT_Stand();
																	if(TimeOutVal)
												        TIM_CreateTimer(TMSG_TIMER, TimeOutVal * 20);
																	break;
												case GSTT_Start:
												     /// Target stand.
												     TGT_Stand();
												     if(TimeOutVal)
																	   TIM_CreateTimer(TMSG_TIMER, TimeOutVal * 20);
																	break;
									}
									
									OldStt  = GameStt;
						}
				}  /// UARTMsg end. 
				
				if(GameMode>=GMODE_Prct  &&  GameMode<=GMODE_Cmpt)
				{
					  if(GameStt >= GSTT_Resume  &&  GameStt <= GSTT_Start)
							{
				
								 if(SensorState ==0)
									{
												
												IO_LEDv5  = 1;
												while(!SensorState);
												
										  if(GameMode == GMODE_Cmpt)
												{
													  TGT_Fall();
													  GameStt  = GSTT_Stop;
												}
												else
												{
													   TGT_Revive();
												}
												IO_LEDv5  = 0;
												
												if(GameMode != GMODE_Prct)
												   ReturnRes();
												TIM_UpdateTimer(TMSG_TIMER, TimeOutVal * 20);
									}
							}
				}
				
				 
				if(TMsgFlags&TMSG_TIMER)
				{
							TMsgFlags  &= (~TMSG_TIMER);
							TGT_Revive();
							TIM_UpdateTimer(TMSG_TIMER, TimeOutVal * 20);
				}
				 
  }
}




void ACK_Reply(void)
{
		UART1_SendByte(0xDF);
		UART1_SendByte(0xDF);
		UART1_SendByte(ID);
		UART1_SendByte(3);
		UART1_SendByte(1);
		UART1_SendByte(0);;
		UART1_SendByte(0);
		UART1_SendByte(1);
		UART1_SendByte(0xFD);
}


void ReturnRes(void)
{
	 UART1_SendByte(0xDF);
	 UART1_SendByte(0xDF);
	 UART1_SendByte(ID);
  UART1_SendByte(5);
	 UART1_SendByte(0);
	 UART1_SendByte(0);
	 UART1_SendByte(0);
	 UART1_SendByte(0);
	 UART1_SendByte(0xFD);
}