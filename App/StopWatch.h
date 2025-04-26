#include <stdint.h>
#ifndef __STOPWATCH_H
#define __STOPWATCH_H


extern volatile uint32_t StopWatch_deciSecond;
extern volatile uint8_t StopWatch_isStart;
extern volatile uint8_t StopWatch_isReset;
extern volatile uint8_t UpperLimit;



void StopWatch(void);

#endif



