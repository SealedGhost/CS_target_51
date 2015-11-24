#include "usart.h"
#include <STC12C5A60S2.h>
#include <string.h>
#include "types.h"
#include <stdio.h>
#include "conf.h"
#include "target.h"

#define SYSCLK  11059200L
#define BAUD    9600

uint8_t UARTMsgFlags  = 0;

volatile uint16_t IdleCnt  = 0;

uint8_t  RxBuf[RXBUF_SIZE] = {0};
uint8_t  UART_Buf[RXBUF_SIZE] = {0};

volatile uint8_t * pRx  = RxBuf;

bit busy;


char hasNewMsg  = 0;
char uartCnt  = 0  ;
UART_STATES UartStt  = UARTState_Init;


void TIM1_Init()
{
	 	AUXR &= 0xBF;		
		 TMOD &= 0x0F;		
		 TMOD |= 0x10;	
		 TL1   = 0x00;	
		 TH1   = 0x4C;	
		 TF1   = 0;	
	  ET1   = 1;
}


/***����1��ʼ��******/
void UART1_Init (void)
{

		PCON &= 0x7F;		//�����ʲ�����
		SCON = 0x40;		//8λ����,�ɱ䲨����
		AUXR |= 0x04;		//���������ʷ�����ʱ��ΪFosc,��1T
		BRT = 0xDC;		//�趨���������ʷ�������װֵ
		AUXR |= 0x01;		//����1ѡ����������ʷ�����Ϊ�����ʷ�����
	//	AUXR |= 0x10;		//�������������ʷ�����
				

	/// Prio   = 1	���п��ж����ȼ� 1
		IPH  &= (~0x10);
		IP   &= (~0x10);
			
		
		TIM1_Init();
}

void UART1_Cmd(FunctionalState newState)
{
   if(ENABLE == newState)
   {
					REN  = 1;
				 TR1  = 1;
					EA   = 1;
					ES   = 1;  		   /// �������ж�
					AUXR  |= 0x10;       /// Set BRTR,������������ʷ���������
   }
   else
   {
			  ES   = 1;
				 TR1  = 0;
			  AUXR &= (~0x10);
   }
}


void UART1_SendByte(uint8_t dat)
{
   while(busy);
   busy  = 1;
//   TI  = 0;
   SBUF  = dat;
}


static void UART1_ResetIdle()
{
	 TR1  = 0;
	 TH0  = 0x4C;
	 TL0  = 0;
	 IdleCnt  = 0;
	 TR1  = 1;
}



void UART1_IRQ() interrupt 4
{
	 if(RI)
		{
			 RI  = 0;
			
				TR1  = 0;
				TH1  = 0x4C;
				TL1  = 0;
				IdleCnt  = 0;
				TR1  = 1;
			 
			 if(pRx - RxBuf  <= RXBUF_SIZE)
				{
					 *pRx  = SBUF;
						pRx++;
				}
			 else
				{
					 pRx  = RxBuf;
						*pRx = RxBuf;
						pRx++;
				}				
		}
		
		if(TI)
		{
			 busy  = 0;
		  TI  = 0;
		}
}



void UART_Idle_IRQ() interrupt 3
{
	 TR1  = 0;	
	 TF1  = 0;
	 TL1  = 0;	  
	 TH1  = 0x4C;
	
	 if(IdleCnt < 10)
		{
			 IdleCnt++;
			 TR1  = 1;
		}	
  else
		{
					 if(  RxBuf[0]==0xdf && RxBuf[1]==0xdf && RxBuf[8]==0xfd 
							  && (RxBuf[4]+RxBuf[5]+RxBuf[6]) == RxBuf[7] )
						{
						  memcpy((void*)UART_Buf,(void*)RxBuf, 9);
							 UARTMsgFlags  |= UART_POST;
								memset((void*)RxBuf, 0, RXBUF_SIZE);
//								printf("idle\n");
						}
			   IdleCnt  = 0;
			   pRx  = RxBuf;
		}	
}




