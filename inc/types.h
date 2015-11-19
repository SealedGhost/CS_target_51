#ifndef _TYPES_H
#define _TYPES_H

typedef unsigned char uint8_t;
typedef unsigned int  uint16_t;

typedef enum {DISABLE = 0, ENABLE = !DISABLE} FunctionalState;
typedef enum {FALSE = 0, TRUE  = !FALSE} Bool;

#endif