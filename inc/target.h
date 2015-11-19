#ifndef _TARGET_H
#define _TARGET_H	

#include <STC12C5A60S2.h>


sbit IO_VSignal  = P3^2;
sbit IO_HSignal  = P3^3;

sbit IO_Motor    = P1^1;

sbit IO_Laser    = P2^7;

sbit IO_Key      = P2^5;

sbit IO_LEDv12   = P2^6;

sbit IO_LEDv5    = P0^6;


void TGT_Stand(void);
void TGT_Fall(void);
void TGT_Revive(void);

#endif