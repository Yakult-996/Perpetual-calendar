#include "stm32f10x.h"                  // Device header

//System
#include "Delay.h"
#include "Timer.h"
#include "MyRTC.h"

//Hardware
#include "Key.h"
#include "LED.h"
#include "Buzzer.h"
//#include "LightSensor.h"
#include "OLED.h"
//#include "CountSensor.h"
#include "Encoder.h"
//#include "PWM.h"
//#include "Motor.h"

//App
#include "ClockModule.h"
#include "Alarm.h"
#include "Menu.h"

//所有文件编码格式：UTF-8



int main(void)
{	
	/*NVIC中断分组*/	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);				//配置NVIC为分组2
	
	/*所有Hardware的初始化*/
	Key_Init();
	LED_Init();
	Buzzer_Init();
	OLED_Init();
	MyRTC_Init();
	
	Display_EditAlarmTitle();
	//ClockDisplay();
	
	while (1)
	{




	}
}

