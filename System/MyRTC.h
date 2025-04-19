#include <stdint.h>
#ifndef __MYRTC_H
#define __MYRTC_H

extern uint16_t MyRTC_Time[];
extern const char* weekStr[];

void MyRTC_Init(void);
void MyRTC_SetTime(void);
void MyRTC_ReadTime(void);

#endif
