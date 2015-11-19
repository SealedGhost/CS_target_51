#include "usart.h"
#include <STC12C5A60S2.h>
#include <string.h>
#include "types.h"
#include <stdio.h>
#include "conf.h"

#define SYSCLK  11059200L
#define BAUD    9600


uint8_t  RxBuf[RXBUF_SIZE] = {0};
uint8_t * pRx  = RxBuf;

bit busy;

unsigned char recBuf[FRM_DATA_SIZE];

char hasNewMsg  = 0;
char uartCnt  = 0  ;
UART_STATES UartStt  = UARTState_Init;

/***串口1初始化******/
void UART1_Init (void)
{

	PCON &= 0x7F;		//波特率不倍速
	SCON = 0x40;		//8位数据,可变波特率
	AUXR |= 0x04;		//独立波特率发生器时钟为Fosc,即1T
	BRT = 0xDC;		//设定独立波特率发生器重装值
	AUXR |= 0x01;		//串口1选择独立波特率发生器为波特率发生器
//	AUXR |= 0x10;		//启动独立波特率发生器
   

/// Prio   = 1	串行口中断优先级 1
   IPH  &= (~0x10);
   IP   &= (~0x10);
   
   TI  = 1;  	    
}

void UART1_Cmd(FunctionalState newState)
{
   if(ENABLE == newState)
   {
					REN  = 1;
					EA   = 1;
					ES   = 1;  		   /// 开串口中断
					AUXR  |= 0x10;       /// Set BRTR,允许独立波特率发生器运行
   }
   else
   {
			  ES  = 1;
			  AUXR  &= (~0x10);
   }
}


void UART1_SendByte(uint8_t dat)
{
   while(busy);
   busy  = 1;
   TI  = 0;
   SBUF  = dat;
}




void UART1_IRQ() interrupt 4 
{
   uint8_t tmpBuf  = 0;

   if(RI)
   {
	  RI  = 0;      

	  tmpBuf  = SBUF;

   switch(UartStt)
   {
	    case UARTState_Init:
		       if(tmpBuf == 0xdf)
			      {
			        UartStt  = UARTState_Nose;	///
		       }
		       break;
         
				 case UARTState_Nose:
				      if(tmpBuf == 0xdf)
					     {
					       UartStt  = UARTState_Nip;
						      uartCnt  = 0;
					     }
					     else
					     {
					       UartStt  = UARTState_Init;
					     }
		        break;

	     case UARTState_Nip:
		      if(uartCnt < 5)
			  {
			     recBuf[uartCnt]  = tmpBuf;
				 uartCnt++;
			  }
			  else
			  {
			     recBuf[INDEX_SUM]  = tmpBuf;
			     if(recBuf[INDEX_SUM]  ==  (recBuf[INDEX_PARA] + recBuf[INDEX_PARA+1] + recBuf[INDEX_PARA+2]) )
				 {
 			        UartStt  = UARTState_Ingested;
				 }
				 else
				 {
					 memset( (void*)recBuf, 0, FRM_DATA_SIZE );
					 UartStt  = UARTState_Init;
				 }
			  }
			  break;

	     case UARTState_Ingested:
		      if(tmpBuf != 0xfd)
			  {
			     memset( (void*)recBuf, 0, FRM_DATA_SIZE );
			  }
			  else
			  {
				 hasNewMsg  = 1;
			  }
			  UartStt  = UARTState_Init;
			  break;
	     
		 default:
		      memset( (void*)recBuf, 0, FRM_DATA_SIZE );
			  UartStt  = UARTState_Init;
			  break;
     }
   }
   if(TI)
   {
	    busy  = 0;
   }
}





