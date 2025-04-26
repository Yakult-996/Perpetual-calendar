#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "Key.h"
#include "OLED.h"
#include "Encoder.h"
#include "ClockModule.h"
#include "Alarm.h"
#include "CountDownTimer.h"
#include "StopWatch.h"
#include "menu.h"
#include "Calendar.h"
#include <string.h>

volatile uint8_t MenuAutoBackSeconds = 10;
volatile uint8_t is_Lv_1_Menu_Open = 0;

/*开机标题及开发信息*/
void AboutMessage(void)
{
	OLED_Printf(8, 0, OLED_8X16, "欢迎使用万年历");
	OLED_Printf(0, 16, OLED_8X16, "版本：V1.0");
	OLED_Printf(0, 32, OLED_8X16, "作者：牟峻永");
	OLED_Printf(0, 48, OLED_8X16, "学号：2220222790");
	OLED_Update();
	Delay_ms(1500);
	OLED_Clear();
	OLED_Update();
}

void Lv_1_Menu(void)
{
	//清屏
	OLED_Clear();
	//当前处于菜单哪一页
	uint8_t Page = 1;
	//光标指向的菜单
	int8_t Cursor = 0;
	//标志着一级菜单是否正处于打开状态
	is_Lv_1_Menu_Open = 1;
	//全局变量，用于计时
	MenuAutoBackSeconds = 10;
	
	
	
	//闹钟设定	Cursor = 0
	//倒计时	Cursor = 1
	//秒表		Cursor = 2

	//日历		Cursor = 3
	//天气		Cursor = 4
	//自动校时	Cursor = 5
	while (1)
	{
		OLED_ShowString(0, 0, "菜单", OLED_8X16);

		//如果在LV1菜单十秒没有操作，返回时钟页面
		if (is_Lv_1_Menu_Open == 1 && MenuAutoBackSeconds == 0)
		{
			break;
		}
		
		if (Cursor <=2)
			Page = 1;
		else if (Cursor >=3)
			Page = 2;
		
		//根据光标位置切换页
		if (Page == 1)
		{
			//清除第2-4行
			OLED_ClearArea(0, 16, 127, 63);
			OLED_ShowString(6, (Cursor + 1) * 16 + 6, ">", OLED_6X8);
			OLED_ShowString(24, 16, "1.闹钟设定", OLED_8X16);		//闹钟设定	Cursor = 0
			OLED_ShowString(24, 32, "2.倒计时", OLED_8X16);       //倒计时	Cursor = 1
			OLED_ShowString(24, 48, "3.秒表", OLED_8X16);         //秒表		Cursor = 2
		}
		else if (Page == 2)
		{
			OLED_ClearArea(0, 16, 127, 63);
			OLED_ShowString(6, (Cursor - 2) * 16 + 6, ">", OLED_6X8);
			OLED_ShowString(24, 16, "4.设置时间", OLED_8X16);			//设置时间		Cursor = 3
			OLED_ShowString(24, 32, "5.日历", OLED_8X16);     			//日历		Cursor = 4
			OLED_ShowString(24, 48, "6.天气", OLED_8X16); 				//天气	Cursor = 5
		}
		
		OLED_Update();
		
		//使用旋转编码器代替原先KEY2和KEY3的加减功能
		//实现光标上下移动
		if (Encoder_Down_Flag == 1 || Encoder_Up_Flag == 1)
		{
			MenuAutoBackSeconds = 10;
			int8_t delta = (Encoder_Up_Flag == 1) ? 1 : -1;
			Encoder_Down_Flag = 0;
			Encoder_Up_Flag = 0;
			Cursor += delta;
			Cursor = WrapAround(Cursor, 0, 5);
		}
		
		uint8_t key = Key_GetNum();
		
		//Key4确认键
		if (key == 4)
		{
			is_Lv_1_Menu_Open = 0;
			MenuAutoBackSeconds = 10;
			if (Cursor == 0)	Alarm_SettingsPage();		//闹钟设定	Cursor = 0
			else if (Cursor == 1)	CountDownTimer();       //倒计时	Cursor = 1
			else if (Cursor == 2)	StopWatch();            //秒表		Cursor = 2
			else if (Cursor == 3)	TimeModify();           //设置时间	Cursor = 3
			else if (Cursor == 4)	Calendar();             //日历		Cursor = 4
			else if (Cursor == 5)	break;                  //天气		Cursor = 5

		}
		
		//Key1是返回键
		else if (key ==1)
		{
			break;
		}
		is_Lv_1_Menu_Open = 1;
		
	}
	is_Lv_1_Menu_Open = 0;
	MenuAutoBackSeconds = 10;
	OLED_Clear();
	OLED_Update();
	
}


