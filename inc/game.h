#ifndef _GAME_H
#define _GAME_H

typedef enum {   
															GMODE_Init = 0, 
															GMODE_Prct, 
															GMODE_Shot, 
															GMODE_Cmpt 
													} GAME_MODE;


typedef enum {
               GSTT_Stop  = 0,
	              GSTT_Ack,
	              GSTT_Pause,
	              GSTT_Resume,  
	              GSTT_Start
             } GAME_STT;


#define GAME_MODE_PRCT  0x07
#define GAME_MODE_SHOT  0x08
#define GAME_MODE_CMPT 	0x09


#endif