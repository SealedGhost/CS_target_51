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
extern char isTimeOut;
extern char hasNewMsg  ;
extern uint8_t recBuf[FRM_DATA_SIZE];


//sbit chuizhi=P3^2 ;
//sbit shuiping=P3^3;
//sbit dianji=P1^1;
//sbit xinhao=P2^7;
//sbit shang_xia=P2^5;
//sbit LED12V=P2^6;
//sbit led5V=P0^6;

sbit KeyState     = P2^5;
sbit SensorState  = P2^7;


const unsigned char ID  = 0x01;

uint8_t UART_State  = 0;
uint8_t GameMode  = 0;
uint8_t TimeOutVal  = 0;


char strBuf[20]  = 0;

GMMODES GMMode  = GMMode_Init;


void ACK_reply(void)
{
		UART1_SendByte(0xDF);
		UART1_SendByte(0xDF);
		UART1_SendByte(ID);
		UART1_SendByte(0x09);
		UART1_SendByte(0x03);
		UART1_SendByte(0x04);;
		UART1_SendByte(0x05);
		UART1_SendByte(0x06);
		UART1_SendByte(0xFD);
}




void main()
{
	 IO_LEDv5  = 0;
		UART1_Init();
		UART1_Cmd(ENABLE);
	
printf("Uart OK\n");	
		TIM0_Init();
  
	 TGT_Revive();
		  
  while(1)
  {
    if(hasNewMsg)
	   {
      hasNewMsg  = 0;
printf("has new msg\n");					
      if(recBuf[INDEX_ADDR] == 0x00  ||  recBuf[INDEX_ADDR] == ID)
						{
								switch(GMMode)
								{
/*------------------------------------------------------------------------------*/
										case GMMode_Init:
										/// Init -->Prct
										if(recBuf[INDEX_CMD] == 0x07  &&  recBuf[INDEX_PARA])
          {
            TimeOutVal  = recBuf[INDEX_PARA];
            GMMode  = GMMode_Prct;
												TGT_Stand();
												if(TIM_CreateTimer(TMSG_TIMER, TimeOutVal * 20))
													  printf("create timer OK\n");
            printf("init --> prct: %d\n",(int)TimeOutVal);						  
          }
          /// Init -->Shot
										else if(recBuf[INDEX_CMD] == 0x08  &&  recBuf[INDEX_PARA])
										{					     
												TimeOutVal  = recBuf[INDEX_PARA];
												GMMode  = GMMode_Shot;
												TGT_Stand();
												TIM_CreateTimer(TMSG_TIMER, TimeOutVal * 20);
            printf("init --> shot:%d s\n",(int)TimeOutVal);
          }															
										/// Init -->Ack
										else if(recBuf[INDEX_CMD] == 0x09  &&  recBuf[INDEX_PARA] == 0x01)
										{
												
												GMMode  = GMMode_Ack;
												TGT_Fall();
            printf("init --> ack\n");
            /// Reply ack.
												ACK_reply();
										}
										break;				 

/*------------------------------------------------------------------------------*/
        case GMMode_Prct:
													if(recBuf[INDEX_CMD] == 0x07)
													{
					          /// Stop prct.
															if(recBuf[INDEX_PARA] == 0)
															{
							          GMMode  = GMMode_Init;
							          TimeOutVal  = 0;
																	TIM_DeleteTimer(TMSG_TIMER);
																	TGT_Fall();
                 printf("prct --> stop\n");
							        }
															else
															{
                 /// Again.
																	TimeOutVal  = recBuf[INDEX_PARA];
																	TIM_UpdateTimer(TMSG_TIMER, TimeOutVal * 20);
                 printf("prct --> again\n");
								       }
							      }
													/// Prct -->Shot
													else if(recBuf[INDEX_CMD] == 0x08  &&  recBuf[INDEX_PARA])
													{
															TimeOutVal  = recBuf[INDEX_PARA];
															GMMode  = GMMode_Shot;
															TIM_UpdateTimer(TMSG_TIMER, TimeOutVal * 20);
               printf("prct --> shut:%d\n",(int)TimeOutVal);
					        }
													/// Prct -->Ack
													else if(recBuf[INDEX_CMD] == 0x09  &&  recBuf[INDEX_PARA] == 0x01)
													{
		             TimeOutVal  = 0;
															GMMode  = GMMode_Ack;
															TIM_DeleteTimer(TMSG_TIMER);
															TGT_Fall();
               printf("prct --> ack\n");
													  /// Reply ack.
															ACK_reply();
											  }
             break;

/*------------------------------------------------------------------------------*/
       case GMMode_Shot:
            if(recBuf[INDEX_CMD] == 0x08)
            {
              if(recBuf[INDEX_PARA])
              {
                /// Again
																TimeOutVal  = recBuf[INDEX_PARA];
																TIM_UpdateTimer(TMSG_TIMER, TimeOutVal * 20);
                printf("shot --> again\n");
						        }
														else if(recBuf[INDEX_PARA+1] == 0x01)
														{
						          /// Pause shot
																TIM_DeleteTimer(TMSG_TIMER);
																TGT_Fall();
                printf("shot --> pause\n");
						        }
														else if(recBuf[INDEX_PARA+1] == 0x02)
														{	
						          /// Resume shot
																TGT_Stand();
																TIM_CreateTimer(TMSG_TIMER,TimeOutVal * 20);
                printf("shot --> resume\n");
						        }
														else 
														{
                /// Stop
                GMMode  = GMMode_Init;
																TimeOutVal  = 0;
																TIM_DeleteTimer(TMSG_TIMER);
																TGT_Fall();
                printf("shot --> stop\n");
						        }
					       }
					       /// Shot -->Prct
												else if(recBuf[INDEX_CMD] == 0x07  &&  recBuf[INDEX_PARA])
												{
												   TimeOutVal  = recBuf[INDEX_PARA];
															GMMode  = GMMode_Prct;
															TIM_UpdateTimer(TMSG_TIMER, TimeOutVal * 20);
printf("shot --> prct:%d s\n",(int)TimeOutVal);
					       }
					       /// Shot -->Ack
												else if(recBuf[INDEX_CMD] == 0x09  &&  recBuf[INDEX_PARA] == 0x01)
												{	
					         TimeOutVal  = 0;
						        GMMode  = GMMode_Ack;
														TIM_DeleteTimer(TMSG_TIMER);
														TGT_Fall();
						        /// Reply ack.
														ACK_reply();
printf("shot --> ack\n");
					       }				      
					       break;

/*------------------------------------------------------------------------------*/
				  case GMMode_Ack:
				       /// Ack -->Cmpt
				       if(recBuf[INDEX_CMD] == 0x09  &&  recBuf[INDEX_PARA])
					      {
						       GMMode  = GMMode_Cmpt;
					        TimeOutVal  = recBuf[INDEX_PARA];
						       TGT_Stand();
													TIM_CreateTimer(TMSG_TIMER, TimeOutVal * 20);
             printf("ack --> cmpt:%d\n", (int)TimeOutVal);
				       }
					      /// Ack -->Prct
					      else if(recBuf[INDEX_CMD] == 0x07  &&  recBuf[INDEX_PARA])
					      {
             GMMode  = GMMode_Prct;
						       TimeOutVal  = recBuf[INDEX_PARA];
						       TGT_Stand();
													TIM_CreateTimer(TMSG_TIMER, TimeOutVal * 20);
             printf("ack --> prct:%d\n", (int)TimeOutVal);										    
					      }
											/// Ack -->Shot
					      else if(recBuf[INDEX_CMD] == 0x08  &&  recBuf[INDEX_PARA])
					      {
					        GMMode  = GMMode_Shot;
						       TimeOutVal  = recBuf[INDEX_PARA];
						       TGT_Stand();
													TIM_CreateTimer(TMSG_TIMER, TimeOutVal * 20);
             printf("ack --> shot:%d\n",(int)TimeOutVal);
					      }
					      /// Stop
					      else 
					      {
						       GMMode  = GMMode_Init;
						       TimeOutVal  = 0	;
													TIM_DeleteTimer(TMSG_TIMER);
													TGT_Fall();
             printf("ack --> failed\n");
					      }
					      break;

/*------------------------------------------------------------------------------*/
      case GMMode_Cmpt:
				       if(recBuf[INDEX_CMD] == 0x09)
					      {
					        /// Again
					        if(recBuf[INDEX_PARA] == 0x01)
					       	{
													  GMMode  = GMMode_Ack;
															TimeOutVal  = 0;
															TIM_DeleteTimer(TMSG_TIMER);
															TGT_Fall();
															ACK_reply();
               printf("cmpt --> ack\n");
					       	}
					       	/// Stop
						       else 
						       {
						         GMMode  = GMMode_Init;
							        TimeOutVal  = 0;
															TIM_DeleteTimer(TMSG_TIMER);
															TGT_Fall();
               printf("cmpt --> stop\n");
						       }
					      }
											/// Cmpt -->Prct
           else if(recBuf[INDEX_CMD] == 0x07  &&  recBuf[INDEX_PARA])
					      {
             GMMode  = GMMode_Prct;
						       TimeOutVal  = recBuf[INDEX_PARA];
													TIM_UpdateTimer(TMSG_TIMER,TimeOutVal * 20);
             printf("ack --> prct:%d\n", (int)TimeOutVal);										    
					      }
					      else if(recBuf[INDEX_CMD] == 0x08  &&  recBuf[INDEX_PARA])
					      {
					        GMMode  = GMMode_Shot;
						       TimeOutVal  = recBuf[INDEX_PARA];
						       TIM_UpdateTimer(TMSG_TIMER, TimeOutVal * 20);
             printf("ack --> shot:%d\n",(int)TimeOutVal);
					      }
					      break;

				  default:
				       /// Stop ??
				       break;
			   }
	   }
	 } /// End new msg process.
//       
  
		if(TMsgFlags) 
		{
			 if(TMsgFlags&TMSG_TIMER)
				{
				   TMsgFlags &= (~TMSG_TIMER);
					  TGT_Revive();
							printf("timeout:%d s\n",(int)TimeOutVal);
							TIM_UpdateTimer(TMSG_TIMER, TimeOutVal * 20);
				}
		}
		
		if(SensorState == 0)
		{ 
			  IO_LEDv5  = 1;
printf("I'm died\n");			
			  while(!SensorState);
			  IO_LEDv5  = 0;
			  
			  IO_LEDv12  = 0;
			  TGT_Revive();
			  IO_LEDv12  = 1;
		}
		 
 }
}

