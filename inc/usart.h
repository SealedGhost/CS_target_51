/*      usart.h        */

/******Author:躺老板*********/
/******Time:2015-7-21******/

/******串行口1  RXD1--P3.0     TXD--P3.1 ********/
/******串行口2  RXD2--P1.2     TXD2--P1.3 ********/
#ifndef __USART_H__
#define __USART_H__
#include <intrins.h>
#include "types.h"


#define UART_POST  0x01

#define RXBUF_SIZE 20


#define FRM_DATA_SIZE  10	///一帧数据中的要用到的数据的大小
#define INDEX_ID 2
#define INDEX_MODE  3
#define INDEX_CMD 2
#define INDEX_SUM  5

#define INX_ID  2
#define INX_MOD 3
#define INX_STT 4
#define INX_VAL 5
#define INX_RSV 6
#define INX_SUM 7


enum _UART_STATES {UARTState_Init = 0, UARTState_Nose, UARTState_Nip, UARTState_Ingested};
typedef enum _UART_STATES  UART_STATES;

extern uint8_t UARTMsgFlags;


void UART1_Init (void);
void UART1_Cmd(FunctionalState newState);
void UART1_SendByte(uint8_t dat);

void usart1_Init (void);
//void usart1_SendOneByte(uchar w);

//void usart2_Init (void);
//void usart2_SendOneByte(uchar c);

void wuxian_all(int x,int y);

void usart_reply(void);
#endif