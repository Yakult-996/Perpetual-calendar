#include "stm32f10x.h"                  // Device header
#include "MyRTC.h"
#include "key.h"
#include "OLED.h"
#include "ClockModule.h"


/*该文件是闹钟功能的代码*/

#define ALARMNUM 5			//最多能设置5个闹钟

/** 
  * @brief  闹钟参数结构体，用来存储闹钟设置的相关参数，可以使用该结构体定义多个闹钟
  * @param  Status: 可以的取值 0 or 1 or -1，代表这个闹钟是否为开启状态，或者暂未设置
  *			0表示关闭，1表示开启，-1表示没有被设置（即删除状态）
  * @param	Hour: 可以的取值 0~23
  * @param  Minute: 可以的取值 0~59
  * @param  Repetition[x]: x 可以的取值0~6分别对应周日~周六
  *			Repetition[x]可以取 0 or 1，代表这天是否开启闹钟，全0时代表不重复
  */
typedef struct AlarmPara
{
	int8_t Status;
	uint16_t Hour;		//闹钟的时
	uint16_t Minute;	//闹钟的分	
	bool Repetition[7];		//闹钟的重复周期，周日~周六
}AlarmPara;


AlarmPara AlarmList[ALARMNUM];

//调整闹钟的时和分
void AdjustAlarm_TimeRange(uint16_t* TempAlarm_Time)
{
	TempAlarm_Time[0] = WrapAround(TempAlarm_Time[0], 0, 23);
	TempAlarm_Time[1] = WrapAround(TempAlarm_Time[1], 0, 59);	
}

//删除第i个闹钟，并将其设置清空
void Alarm_Delete(uint8_t i)
{
	AlarmList[i].Status = -1;
	AlarmList[i].Hour = 0;
	AlarmList[i].Minute = 0;
	for (int j = 0; j < 7; j++)
		AlarmList[i].Repetition[j] = false;
}

//初始化闹钟列表
void Alarm_Init(void)
{
	for (int i = 0; i < ALARMNUM; i++)
		Alarm_Delete(i);
}

//编辑页面的文本显示对齐
void Display_EditAlarmTitle(void)
{
	OLED_ShowString(31, 2, "Edit  Alarm", OLED_6X8);			//顶部居中显示 Edit  Alarm
	OLED_ShowString(36, 18, "XX : XX",OLED_8X16);				//时间水平居中、偏上显示
	OLED_ShowString(10, 42, "Rep  7 1 2 3 4 5 6", OLED_6X8);	//重复
	OLED_ShowString(80, 56, "SET", OLED_6X8);
	OLED_ShowString(110, 56, "DEL", OLED_6X8);
	OLED_Update();
}

//显示当前设置界面的AlarmTime和Repetition
void DisplayAlarmTemp(uint16_t* TempAlarm_Time, bool* TempAlarm_Repetition)
{
	bool is_pm = false;
	//如果是12小时制
	if (!flag24_12)
	{
		//中午12：00-23：59分应该对应12：00PM~11：59PM
		if (TempAlarm_Time[0] >= 12)
		{
			is_pm = true;
			if (TempAlarm_Time[0] > 12) TempAlarm_Time[0] -= 12;
		}
		//半夜00：00应该对应12：00AM，而1：00~11：59无需转换
		else if (TempAlarm_Time[0] == 0)
		{
			TempAlarm_Time[0] = 12;
		}
		OLED_ShowString(108, 24, is_pm ? "PM" : "AM", OLED_6X8);
	}	
	else OLED_ClearArea(108, 24, 113, 31);
	
	OLED_ShowNum(36, 18, TempAlarm_Time[1], 2, OLED_8X16);		//小时
	OLED_ShowNum(76, 18, TempAlarm_Time[2], 2, OLED_8X16);		//分钟
	
	for (int j = 0; j < 7; j++)
	{
		//如果选中了，就反色显示
		if (TempAlarm_Repetition[j])
		{
			OLED_DrawMode = 1;
			OLED_ShowChar(40 + j*12, 42, (j+7) % 7 + 48, OLED_6X8);
			OLED_DrawMode = 0;
		}
		else
		{
			OLED_ShowChar(40 + j*12, 42, (j+7) % 7 + 48, OLED_6X8);	
		}
	}

}


//设置闹钟
void EditAlarm(uint8_t i)
{
	Display_EditAlarmTitle();
	
	uint8_t Cursor = 9;									//周日0~周六6，SET 7，DEL 8，时9，分10
	uint16_t CursorPosition[] = {36, 34, 47, 34};		//光标绘图位置
	
	//继承第i个闹钟的原有设置
	uint16_t TempAlarm_Time[] = {AlarmList[i].Hour, AlarmList[i].Minute};
	bool TempAlarm_Repetition[7] = {false, false, false, false, false, false, false};
	
	//如果第i个闹钟不存在，默认的配置
	if (AlarmList[i].Status == -1)
	{
		MyRTC_ReadTime();
		TempAlarm_Time[0] = MyRTC_Time[3];
		TempAlarm_Time[1] = MyRTC_Time[4];
		for (int j = 0; j<7; j++)
			TempAlarm_Repetition[j] = false;
	}
	
	
	while(1)
	{
		//显示时间
		DisplayAlarmTemp(TempAlarm_Time, TempAlarm_Repetition);
	
		switch (Cursor)
		{
			case 0:	CursorPosition[0] = 40;	CursorPosition[1] = 50;	CursorPosition[2] = 45;	CursorPosition[3] = 50;break;	//周日
			case 1:	CursorPosition[0] = 52;	CursorPosition[1] = 50;	CursorPosition[2] = 57;	CursorPosition[3] = 50;break;	//周一
			case 2:	CursorPosition[0] = 64;	CursorPosition[1] = 50;	CursorPosition[2] = 69;	CursorPosition[3] = 50;break;	//周二
			case 3:	CursorPosition[0] = 76;	CursorPosition[1] = 50;	CursorPosition[2] = 81;	CursorPosition[3] = 50;break;	//周三
			case 4:	CursorPosition[0] = 88;	CursorPosition[1] = 50;	CursorPosition[2] = 93;	CursorPosition[3] = 50;break;	//周四
			case 5:	CursorPosition[0] = 90;	CursorPosition[1] = 50;	CursorPosition[2] = 105;CursorPosition[3] = 50;break;	//周五
			case 6:	CursorPosition[0] = 102;CursorPosition[1] = 50;	CursorPosition[2] = 117;CursorPosition[3] = 50;break;	//周六
			case 7:	CursorPosition[0] = 80;	CursorPosition[1] = 63;	CursorPosition[2] = 97;	CursorPosition[3] = 63;break;	//SET
			case 8:	CursorPosition[0] = 110;CursorPosition[1] = 63;	CursorPosition[2] = 127;CursorPosition[3] = 63;break;	//DEL
			case 9:	CursorPosition[0] = 36;	CursorPosition[1] = 34;	CursorPosition[2] = 43;	CursorPosition[3] = 34;break;	//时
			case 10:CursorPosition[0] = 76;	CursorPosition[1] = 34;	CursorPosition[2] = 83;	CursorPosition[3] = 34;break;	//分
			default:break;
		}
		
		OLED_DrawLine(CursorPosition[0], CursorPosition[1], CursorPosition[2], CursorPosition[3]);
		
		OLED_Update();
		
		uint8_t key = Key_GetNum();
		
		//KEY1移动光标位置
		if (key == 1)
		{
			OLED_ClearArea(CursorPosition[0], CursorPosition[1], CursorPosition[2], CursorPosition[3]);
			OLED_UpdateArea(CursorPosition[0], CursorPosition[1], CursorPosition[2], CursorPosition[3]);
			//确保光标在指定位置上移动
			Cursor = (Cursor + 1) % 11;
		}
		
		//KEY2 KEY3当设置时和分的时候，分别作为增加或减小
		else if (key == 2 || key == 3)
		{
			//光标9和光标10分别对应 时 和 分
			if (Cursor ==9 || Cursor ==10)
			{
				int8_t delta = (key == 2) ? 1 : -1;
				TempAlarm_Time[Cursor-9] += delta;
				AdjustAlarm_TimeRange(TempAlarm_Time);		//确保时间在合理范围内移动
			}

		}
		
		//KEY4 功能
		else if (key == 4)
		{
			if (Cursor < 7)
			{
				TempAlarm_Repetition[Cursor] = !TempAlarm_Repetition[Cursor];
			}
			
			//当光标指在SET上时，点击KEY4将当前设置保存到第i个闹钟
			else if (Cursor == 8)
			{
				AlarmList[i].Status = 1;	//闹钟开启
				AlarmList[i].Hour = TempAlarm_Time[0];		//保存时间设置
				AlarmList[i].Minute = TempAlarm_Time[1];
				for (int j = 0; j < 7; j++)
				{
					AlarmList[i].Repetition[j] = TempAlarm_Repetition[j];		//保存重复设置
				}
			}
			else if (Cursor == 9)
			{
				//删除当前闹钟
				Alarm_Delete(i);
			}
		}
	}
}

void Alarm_SettingInterface(void)
{
	OLED_ShowString(0, 0, "Alarm", OLED_8X16);
	for (int i = 0; i < ALARMNUM; i++)
	{
		
	}
	
}











