#include "stm32f10x.h"                  // Device header
#include "Key.h"
#include "OLED.h"
#include "Encoder.h"
#include "ClockModule.h"

//全局变量
volatile uint8_t CountDownTimer_StartFlag = 0;
volatile uint8_t isTimerReset = 1;
volatile uint8_t isTimerFirstStart = 1;
volatile uint32_t CountDown_Seconds = 300;


//倒计时的各元素位置
void CountDownTimer_UI(void)
{
	OLED_ShowString(0, 0, "计时器", OLED_8X16);	
	OLED_ShowString(32, 24, "XX:XX:XX", OLED_8X16);	
	OLED_ShowString(13, 50, "Back", OLED_6X8);
	if (CountDownTimer_StartFlag == 0)
		OLED_ShowString(49, 50, "Start", OLED_6X8);
	else if (CountDownTimer_StartFlag == 1)
		OLED_ShowString(49, 50, "Pause", OLED_6X8);

	OLED_ShowString(91, 50, "Rset", OLED_6X8);
}

//显示时间
void DisplayTime(uint16_t* Temp_TimerSet)
{
	OLED_ShowNum(32, 24, Temp_TimerSet[0], 2, OLED_8X16);
	OLED_ShowNum(56, 24, Temp_TimerSet[1], 2, OLED_8X16);
	OLED_ShowNum(80, 24, Temp_TimerSet[2], 2, OLED_8X16);
}

//保证时间范围合理，不越界
void Adjust_TimeRange(uint16_t* Temp_TimerSet)
{
	Temp_TimerSet[0] = WrapAround(Temp_TimerSet[0], 0, 23);
	Temp_TimerSet[1] = WrapAround(Temp_TimerSet[1], 0, 59);
	Temp_TimerSet[2] = WrapAround(Temp_TimerSet[2], 0, 59);
}

void ResetTimer(void)
{
	isTimerReset = 1;
	CountDownTimer_StartFlag = 0;
	isTimerFirstStart = 1;
	//清除编码器的标志位，防止在运行时，旋转了编码器，导致复位会轮询到变化
	Encoder_Up_Flag = 0;
	Encoder_Down_Flag = 0;	
}

//主页面
void CountDownTimer(void)
{
	OLED_Clear();
	
	//显示时间连接符和按键提示
	CountDownTimer_UI();
	
	//光标和光标位置
	uint8_t Cursor = 0;
	uint16_t CursorPosition[4] = {32, 39, 47, 39};
	
	//临时存储数组，存储未保存时的值
	uint16_t Temp_TimerSet[3] = {0, 5, 0};
	
	//用来显示正在运行时的时间
	uint16_t Running_Time[3] = {0, 5, 0};
	
	
	while (1)
	{
		if (CountDownTimer_StartFlag == 0)
			OLED_ShowString(49, 50, "Start", OLED_6X8);
		else if (CountDownTimer_StartFlag == 1)
			OLED_ShowString(49, 50, "Pause", OLED_6X8);
		
		if (CountDown_Seconds == 0)
		{
			isTimerReset = 1;
		}
		Running_Time[0] = CountDown_Seconds / 3600;
		Running_Time[1] = (CountDown_Seconds % 3600) / 60;
		Running_Time[2] = CountDown_Seconds % 60;
		//如果复位后，就显示调整时间
		if (isTimerReset == 1)
			DisplayTime(Temp_TimerSet);
		//如果正在运行，就显示倒计时时间
		else if (isTimerReset == 0)
		{
			DisplayTime(Running_Time);
		}
		
		switch(Cursor)
		{
			case 0:CursorPosition[0] = 32;	CursorPosition[2] = 47;	break;
			case 1:CursorPosition[0] = 56;	CursorPosition[2] = 71;	break;
			case 2:CursorPosition[0] = 80;	CursorPosition[2] = 95;	break;
			default:break;
		}
		//如果复位后，就显示调整的光标
		if (isTimerReset == 1)
		{
			OLED_DrawLine(CursorPosition[0], CursorPosition[1], CursorPosition[2], CursorPosition[3]);
		}
		//如果正在运行，就隐藏光标
		else if (isTimerReset == 0)
		{
			OLED_EraseLine(CursorPosition[0], CursorPosition[1], CursorPosition[2], CursorPosition[3]);
		}
		
		OLED_Update();
		
		//只有在复位时，才能使用旋转编码器调整时间
		if (isTimerReset == 1)
		{
			//使用旋转编码器代替原先KEY2和KEY3的加减功能
			if (Encoder_Down_Flag == 1 || Encoder_Up_Flag == 1)
			{
				int8_t delta = (Encoder_Up_Flag == 1) ? 1 : -1;
				Encoder_Down_Flag = 0;
				Encoder_Up_Flag = 0;
				Temp_TimerSet[Cursor] += delta;
				//保证对应的数在范围内滚动切换
				Adjust_TimeRange(Temp_TimerSet);
			}
		}
						
		uint8_t key = Key_GetNum();
		
		//KEY2移动光标位置
		if (key == 2)
		{
			if (CountDownTimer_StartFlag == 0)
			{
				OLED_EraseLine(CursorPosition[0], CursorPosition[1], CursorPosition[2], CursorPosition[3]);
				OLED_Update();
				//确保光标在指定位置上移动
				Cursor = (Cursor + 1) % 3;
			}

		}
		//KEY1返回主菜单
		else if (key == 1)
		{
			break;
		}		
		//KEY3开启或暂停计时器
		else if (key == 3)
		{
			//只要按下Start/Pause，那么就让isReset归位
			isTimerReset = 0;
			//如果处于Pause状态，那么就Start
			if (CountDownTimer_StartFlag == 0)
			{
				CountDownTimer_StartFlag = 1;
				//如果是第一次start，才使CountDownSeconds载入设定值
				//如果是暂停后再次Start，不载入设定值
				if (isTimerFirstStart == 1)
				{
					CountDown_Seconds = Temp_TimerSet[0] * 3600 + Temp_TimerSet[1] * 60 + Temp_TimerSet[2];
					isTimerFirstStart = 0; // 标记已经开始过一次了
				}
			}
			//如果处于Start状态，那么就Pause
			else if (CountDownTimer_StartFlag == 1)
				CountDownTimer_StartFlag = 0;
		}
		//KEY4复位计时器
		else if (key == 4)
		{
			ResetTimer();
		}
	}
	OLED_Clear();
	OLED_Update();
}



