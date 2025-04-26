#include "stm32f10x.h"                  // Device header
#include "MyRTC.h"
#include "Key.h"
#include "OLED.h"
#include "Encoder.h"
#include "SolarToLunar.h"

void Calendar(void)
{
	OLED_Clear();
	
	OLED_ShowString(24, 4, "XXXX-XX-XX", OLED_8X16);
	OLED_ShowString(24, 24, "甲子年  鼠", OLED_8X16);
	OLED_ShowString(24, 44, "正月  ", OLED_8X16);
	OLED_ShowString(72, 44, "初一", OLED_8X16);
	
	OLED_Update();
	
	
	while (1)
	{
		OLED_ShowNum(24, 4, MyRTC_Time[0], 4, OLED_8X16);
		OLED_ShowNum(64, 4, MyRTC_Time[1], 2, OLED_8X16);
		OLED_ShowNum(88, 4, MyRTC_Time[2], 2, OLED_8X16);
		
		OLED_ShowString(24, 24, g_lunarDate.lunarYearString, OLED_8X16);
		OLED_ShowString(88, 24, g_lunarDate.zodiac, OLED_8X16);
		OLED_ShowString(24, 44, g_lunarDate.lunarMonthString, OLED_8X16);
		OLED_ShowString(72, 44, g_lunarDate.lunarDayString, OLED_8X16);
		
		OLED_Update();
		
		uint8_t key = Key_GetNum();
		
		if (key == 1)
		{
			break;
		}
		
	}
	//清屏退出（back）
	OLED_Clear();
	OLED_Update();
}

