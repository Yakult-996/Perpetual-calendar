#include <stdint.h>
#ifndef __ENCODER_H
#define __ENCODER_H


extern volatile uint8_t Encoder_Up_Flag;
extern volatile uint8_t Encoder_Down_Flag;

void Encoder_Init(void);
//int16_t Encoder_Get(void);


#endif
