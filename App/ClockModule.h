#include <stdint.h>
#include <stdbool.h>
#ifndef __CLOCKMODULE_H
#define __CLOCKMODULE_H


uint16_t WrapAround(uint16_t value, uint16_t min, uint16_t max);
extern bool flag24_12;

void ClockDisplay(void);
void RTCTimeDisplay(void);
void TimeModify(void);

#endif

