#include "stm32f10x.h"                  // Device header
#include "Key.h"
#include "OLED.h"
#include "ClockModule.h"

volatile uint32_t StopWatch_deciSecond = 0;

volatile uint8_t StopWatch_isStart = 0;
volatile uint8_t StopWatch_isReset = 0;
volatile uint8_t UpperLimit = 0;


void StopWatchTitle(void)
{
	OLED_ShowString(0, 0, "秒表", OLED_8X16);
	OLED_ShowString(24, 24, "XX:XX:XX.x", OLED_8X16);		//时间水平垂直居中显示，像素的范围：X(32,95),Y(24,29)
	OLED_ShowString(13, 54, "Back", OLED_6X8);
	if (StopWatch_isStart == 0)
		OLED_ShowString(49, 54, "Start", OLED_6X8);
	else if (StopWatch_isStart == 1)
		OLED_ShowString(49, 54, "Pause", OLED_6X8);
	
	OLED_ShowString(91, 54, "Rset", OLED_6X8);
}

void Transform(void)
{
	uint32_t totalSeconds = StopWatch_deciSecond / 10;
	uint8_t Hour = totalSeconds /3600;
	uint8_t Minute = (totalSeconds % 3600) / 60;
	uint8_t Second = totalSeconds % 60;
	uint8_t diciSecond = StopWatch_deciSecond % 10;
	Hour = WrapAround(Hour, 0, 99);
	if (Hour == 99 && Minute ==59 && Second == 59 && diciSecond == 9)
	{
		UpperLimit = 1;
	}
	else UpperLimit = 0;
}

void ResetStopWatch(void)
{
	StopWatch_isReset = 1;
	StopWatch_isStart = 0;
	StopWatch_deciSecond = 0;
}



void ShowSWTime(uint32_t StopWatch_deciSecond)
{
	uint32_t totalSeconds = StopWatch_deciSecond / 10;
	uint8_t Hour = totalSeconds /3600;
	uint8_t Minute = (totalSeconds % 3600) / 60;
	uint8_t Second = totalSeconds % 60;
	uint8_t diciSecond = StopWatch_deciSecond % 10;
	OLED_ShowNum(24, 24, Hour, 2, OLED_8X16);
	OLED_ShowNum(48, 24, Minute, 2, OLED_8X16);
	OLED_ShowNum(72, 24, Second, 2, OLED_8X16);
	OLED_ShowNum(96, 24, diciSecond, 1, OLED_8X16);
}

void StopWatch(void)
{
	//清屏
	OLED_Clear();
	
	StopWatchTitle();
	
	while (1)
	{
		if (StopWatch_isStart == 0)
			OLED_ShowString(49, 54, "Start", OLED_6X8);
		else if (StopWatch_isStart == 1)
			OLED_ShowString(49, 54, "Pause", OLED_6X8);
		
		Transform();
		
		if (UpperLimit == 1)
			ResetStopWatch();
		
		ShowSWTime(StopWatch_deciSecond);
		OLED_Update();
		
		uint8_t key = Key_GetNum();
		
		//返回主菜单
		if (key == 1)
		{
			break;
		}
		//开始和暂停
		else if (key == 3)
		{
			//只要按下Start/Pause,Reset归位
			StopWatch_isReset = 0;
			//如果没有start，那么就start
			if (StopWatch_isStart ==0)
			{
				//Start
				StopWatch_isStart = 1;
			}
			//如果已经Start，那么就Pause
			else if (StopWatch_isStart ==1)
				StopWatch_isStart = 0;
		}
		
		//复位
		else if (key == 4)
		{
			ResetStopWatch();
		}
	}
	OLED_Clear();
	OLED_Update();
}


