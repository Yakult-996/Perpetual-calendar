#include <stdint.h>
#include <stdbool.h>
#ifndef __ALARM_H
#define __ALARM_H

#define ALARMNUM 5			//最多能设置5个闹钟

/** 
  * @brief  闹钟参数结构体，用来存储闹钟设置的相关参数，可以使用该结构体定义多个闹钟
  * @param  Status: 可以的取值 0 or 1 or -1，代表这个闹钟是否为开启状态，或者暂未设置
  *			0表示关闭，1表示开启，-1表示没有被设置（即删除状态）
  * @param	Hour: 可以的取值 0~23
  * @param  Minute: 可以的取值 0~59
  * @param  Repetition[x]: x 可以的取值0~6分别对应周日~周六
  *			Repetition[x]可以取 false or true，代表这天是否开启闹钟，全flase时代表不重复
  */
typedef struct AlarmPara
{
	int8_t Status;
	uint16_t Hour;		//闹钟的时
	uint16_t Minute;	//闹钟的分	
	bool Repetition[7];		//闹钟的重复周期，周日~周六
}AlarmPara;

extern AlarmPara AlarmList[ALARMNUM];
extern volatile uint8_t AlarmTriggeredFlag;

	

void Alarm_Init(void);
void Display_EditAlarmTitle(void);
void EditAlarm(uint8_t i);
void Alarm_SettingsPage(void);
uint8_t AlarmCheck(void);

#endif
