#ifndef _GAME_H
#define _GAME_H

enum _GAME_MODE { GMMode_Init = 0, 
                  GMMode_Prct, 
				  GMMode_Shot, 
				  GMMode_Cmpt,
				  GMMode_Ack, 
				  GMMode_Pause, 
				  GMMode_Resume};
typedef enum _GAME_MODE GMMODES;

#define GAME_MODE_PRCT  0x07
#define GAME_MODE_SHOT  0x08
#define GAME_MODE_CMPT 	0x09


#endif