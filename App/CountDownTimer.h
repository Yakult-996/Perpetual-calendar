#include <stdint.h>
#ifndef __COUNTDOWNTIMER_H
#define __COUNTDOWNTIMER_H

extern volatile uint8_t CountDownTimer_StartFlag;
extern volatile uint8_t isTimerReset;
extern volatile uint8_t isTimerFirstStart;
extern volatile uint32_t CountDown_Seconds;

void CountDownTimer(void);
void ResetTimer(void);


#endif

