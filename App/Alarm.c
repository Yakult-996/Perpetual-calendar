#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "MyRTC.h"
#include "key.h"
#include "LED.h"
#include "Buzzer.h"
#include "OLED.h"
#include "Encoder.h"
#include "ClockModule.h"
#include "Alarm.h"


/*该文件是闹钟功能的代码*/

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
	OLED_ShowString(80, 55, "SET", OLED_6X8);
	OLED_ShowString(110, 55, "DEL", OLED_6X8);
	OLED_Update();
}

//显示当前设置界面的AlarmTime和Repetition
void DisplayAlarmTemp(uint16_t* TempAlarm_Time, bool* TempAlarm_Repetition)
{
	char WeekDay[7] = {55, 49, 50, 51, 52, 53, 54};
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
	
	OLED_ShowNum(36, 18, TempAlarm_Time[0], 2, OLED_8X16);		//小时
	OLED_ShowNum(76, 18, TempAlarm_Time[1], 2, OLED_8X16);		//分钟
	
	for (int j = 0; j < 7; j++)
	{
		//如果选中了，就反色显示
		if (TempAlarm_Repetition[j])
		{
			OLED_DrawMode = 1;
			OLED_ShowChar(40 + j*12, 42, WeekDay[j], OLED_6X8);
			OLED_DrawMode = 0;
		}
		else
		{
			OLED_ShowChar(40 + j*12, 42, WeekDay[j], OLED_6X8);	
		}
	}

}


//设置闹钟
void EditAlarm(uint8_t i)
{
	OLED_Clear();
	Display_EditAlarmTitle();
	
	uint8_t Cursor = 9;									//周日0~周六6，SET 7，DEL 8，时9，分10
	uint16_t CursorPosition[] = {36, 34, 51, 34};		//光标绘图位置
	
	
	uint16_t TempAlarm_Time[2] = {0, 0};
	bool TempAlarm_Repetition[7] = {false, false, false, false, false, false, false};
	
	//如果第i个闹钟不存在，默认的配置
	if (AlarmList[i].Status == -1)
	{
		MyRTC_ReadTime();
		TempAlarm_Time[0] = MyRTC_Time[3];
		TempAlarm_Time[1] = MyRTC_Time[4];
		for (int j = 0; j < 7; j++)
			TempAlarm_Repetition[j] = false;
	}
	//若第i个闹钟本身存在，无论时开启还是关闭，都继承原有设置
	else if (AlarmList[i].Status == 0 || AlarmList[i].Status == 1)
	{
		TempAlarm_Time[0] = AlarmList[i].Hour;
		TempAlarm_Time[1] = AlarmList[i].Minute;
		for (int j = 0; j < 7; j++)
			TempAlarm_Repetition[j] = AlarmList[i].Repetition[j];
	}
	
	
	while(1)
	{
		//显示时间
		DisplayAlarmTemp(TempAlarm_Time, TempAlarm_Repetition);
	
		switch (Cursor)
		{
			case 0:	CursorPosition[0] = 40;	CursorPosition[1] = 50;	CursorPosition[2] = 46;	CursorPosition[3] = 50;break;	//周日
			case 1:	CursorPosition[0] = 52;	CursorPosition[1] = 50;	CursorPosition[2] = 58;	CursorPosition[3] = 50;break;	//周一
			case 2:	CursorPosition[0] = 64;	CursorPosition[1] = 50;	CursorPosition[2] = 69;	CursorPosition[3] = 50;break;	//周二
			case 3:	CursorPosition[0] = 76;	CursorPosition[1] = 50;	CursorPosition[2] = 82;	CursorPosition[3] = 50;break;	//周三
			case 4:	CursorPosition[0] = 88;	CursorPosition[1] = 50;	CursorPosition[2] = 94;	CursorPosition[3] = 50;break;	//周四
			case 5:	CursorPosition[0] = 100;CursorPosition[1] = 50;	CursorPosition[2] = 106;CursorPosition[3] = 50;break;	//周五
			case 6:	CursorPosition[0] = 112;CursorPosition[1] = 50;	CursorPosition[2] = 118;CursorPosition[3] = 50;break;	//周六
			case 7:	CursorPosition[0] = 80;	CursorPosition[1] = 63;	CursorPosition[2] = 98;	CursorPosition[3] = 63;break;	//SET
			case 8:	CursorPosition[0] = 110;CursorPosition[1] = 63;	CursorPosition[2] = 127;CursorPosition[3] = 63;break;	//DEL
			case 9:	CursorPosition[0] = 36;	CursorPosition[1] = 34;	CursorPosition[2] = 51;	CursorPosition[3] = 34;break;	//时
			case 10:CursorPosition[0] = 76;	CursorPosition[1] = 34;	CursorPosition[2] = 91;	CursorPosition[3] = 34;break;	//分
			default:break;
		}
		
		OLED_DrawLine(CursorPosition[0], CursorPosition[1], CursorPosition[2], CursorPosition[3]);
		
		OLED_Update();
		
		//使用旋转编码器代替原先KEY2和KEY3的加减功能
		if (Encoder_Down_Flag == 1 || Encoder_Up_Flag == 1)
		{
			//光标9和光标10分别对应 时 和 分
			if (Cursor ==9 || Cursor ==10)
			{
				int8_t delta = (Encoder_Up_Flag == 1) ? 1 : -1;
				Encoder_Down_Flag = 0;
				Encoder_Up_Flag = 0;
				TempAlarm_Time[Cursor-9] += delta;			//时和分对应Temp数组的索引0和1
				AdjustAlarm_TimeRange(TempAlarm_Time);		//确保时间在合理范围内移动
			}
		}
		
		uint8_t key = Key_GetNum();
		
		//KEY3移动光标位置
		if (key == 3)
		{
			OLED_EraseLine(CursorPosition[0], CursorPosition[1], CursorPosition[2], CursorPosition[3]);
			OLED_Update();
			//确保光标在指定位置上移动
			Cursor = (Cursor + 1) % 11;
		}
		else if (key == 1)
		{
			break;
		}
		
//		//KEY2 KEY3当设置时和分的时候，分别作为增加或减小
//		else if (key == 2 || key == 3)
//		{
//			//光标9和光标10分别对应 时 和 分
//			if (Cursor ==9 || Cursor ==10)
//			{
//				int8_t delta = (key == 2) ? 1 : -1;
//				TempAlarm_Time[Cursor-9] += delta;			//时和分对应Temp数组的索引0和1
//				AdjustAlarm_TimeRange(TempAlarm_Time);		//确保时间在合理范围内移动
//			}

//		}
		
		//KEY4 功能
		else if (key == 4)
		{
			//当光标指在某一重复日上时，点击KEY4进行设置或取消设置
			if (Cursor < 7)
			{
				TempAlarm_Repetition[Cursor] = !TempAlarm_Repetition[Cursor];
			}
			
			//当光标指在SET上时，点击KEY4将当前设置保存到第i个闹钟
			else if (Cursor == 7)
			{
				AlarmList[i].Status = 1;	//闹钟开启
				AlarmList[i].Hour = TempAlarm_Time[0];		//保存时间设置
				AlarmList[i].Minute = TempAlarm_Time[1];
				for (int j = 0; j < 7; j++)
				{
					AlarmList[i].Repetition[j] = TempAlarm_Repetition[j];		//保存重复设置
				}
				break;
			}
			else if (Cursor == 8)
			{
				//删除当前闹钟
				Alarm_Delete(i);
				break;
			}
		}
	}
	//设置完成后，清屏退出
	OLED_Clear();
	OLED_Update();
}

uint8_t SumNumofAlarm()
{
	uint8_t sum = 0;
	//统计闹钟列表现在有多少个闹钟
	for (int i = 0; i < ALARMNUM; i++)
	{
		if (AlarmList[i].Status != -1)
			sum ++;
	}
	return sum;
}

void Add_Alarm()
{
	uint8_t i = SumNumofAlarm();
	if (i < 5)	EditAlarm(i);
}

// 交换两个 AlarmPara 的值
void SwapAlarms(AlarmPara *a, AlarmPara *b)
{
    AlarmPara temp = *a;
    *a = *b;
    *b = temp;
}

//将闹钟列表根据时间排序，并把被删除的列表（state=-1）移至最后
void SortAlarmByTime(void)
{
    for (int i = 0; i < ALARMNUM - 1; i++)
    {
        for (int j = 0; j < ALARMNUM - 1 - i; j++)
        {
            // 如果当前项或下一项是无效项，跳过这次比较
            if (AlarmList[j].Status == -1 || AlarmList[j + 1].Status == -1)
                continue;

            // 比较时间，当前项时间比下一项大则交换
            if ((AlarmList[j].Hour > AlarmList[j + 1].Hour) ||
                (AlarmList[j].Hour == AlarmList[j + 1].Hour && AlarmList[j].Minute > AlarmList[j + 1].Minute))
            {
                SwapAlarms(&AlarmList[j], &AlarmList[j + 1]);
            }
        }
    }

    // 再次扫一遍，把 Status == -1 的闹钟全部移动到数组末尾
    for (int i = 0; i < ALARMNUM - 1; i++)
    {
        for (int j = 0; j < ALARMNUM - 1 - i; j++)
        {
            if (AlarmList[j].Status == -1 && AlarmList[j + 1].Status != -1)
            {
                SwapAlarms(&AlarmList[j], &AlarmList[j + 1]);
            }
        }
    }
}

//闹钟界面
void Alarm_SettingsPage(void)
{
	OLED_Clear();
	
	//现存闹钟的数量
	uint8_t Existed_AlarmNum = SumNumofAlarm();

	//当前OLED显示哪一页
	uint8_t page = 1;
	
	//每页显示的最大闹钟数
	uint8_t page1MaxNum = 0;
	uint8_t page2MaxNum = 0;
	
	//光标的位置,取值范围1~6，每页最多有3行可交互
	//光标在1-3在第1页，光标在4-6在第2页
	int8_t Cursor = 1;
	
	//当前光标指向的闹钟列表
	uint8_t CurrentAlarm = 0;
	
	while(1)
	{
		//闹钟菜单的标题，在第1页和第2页都显示
		OLED_ShowString(0, 0, "闹钟", OLED_8X16);
		OLED_Update();
		//现存闹钟的数量
		Existed_AlarmNum = SumNumofAlarm();
		
		//如果闹钟数是3、4，那么第一页不能全部显示
		//第一页除“添加闹钟”这一行外，最多显示2行
		//那么第二页就要显示  （闹钟数量 - 2）  行
		if (Existed_AlarmNum > 2 && Existed_AlarmNum < ALARMNUM)
		{
			page1MaxNum = 2;
			page2MaxNum = Existed_AlarmNum - 2;
		}
		//同理，如果到达了最大可设置闹钟数（5），那么就不能再显示“添加闹钟”这一行
		//则第一页最大可以显示3行闹钟
		//第二页最大可以显示2行闹钟
		else if (Existed_AlarmNum == ALARMNUM)
		{
			page1MaxNum = 3;
			page2MaxNum = 2;
		}
		//如果闹钟数量小于等于2，则一页即可显示的开
		else if (Existed_AlarmNum <= 2)
		{
			page1MaxNum = Existed_AlarmNum;
			page2MaxNum = 0;
		}
	
		//根据光标位置，确定显示哪一页
		if (Cursor >= 1 && Cursor <= 3)
			page = 1;
		else if (Cursor > 3 && Cursor <= 6)
			page = 2;
		
		//如果光标在第一页
		if (page == 1)
		{
			OLED_ClearArea(0, 16, 127, 63);			
			OLED_ShowString(6, Cursor * 16 + 6, ">", OLED_6X8);
			if (page1MaxNum <= 2)
			{
				CurrentAlarm = Cursor - 2;
				OLED_ShowString(24, 16, "添加闹钟", OLED_8X16);
				for (int i = 0; i < page1MaxNum; i++)
				{
					OLED_ShowString(24, (i + 2) * 16, "XX:XX", OLED_8X16);
					OLED_ShowNum(24, (i + 2) * 16, AlarmList[i].Hour, 2, OLED_8X16);
					OLED_ShowNum(48, (i + 2) * 16, AlarmList[i].Minute, 2, OLED_8X16);
				}
			}
			else if (page1MaxNum == 3)
			{
				CurrentAlarm = Cursor -1;
				for (int i = 0; i < page1MaxNum; i++)
				{
					OLED_ShowString(24, (i + 1) * 16, "XX:XX", OLED_8X16);
					OLED_ShowNum(24, (i + 1) * 16, AlarmList[i].Hour, 2, OLED_8X16);
					OLED_ShowNum(48, (i + 1) * 16, AlarmList[i].Minute, 2, OLED_8X16);
				}
			}
		}
		
		//如果光标在第二页
		if (page == 2)
		{
			OLED_ClearArea(0, 16, 127, 63);
			if (Existed_AlarmNum < 5)
				CurrentAlarm = Cursor - 2;
			else if (Existed_AlarmNum == 5)
				CurrentAlarm = Cursor - 1;
			OLED_ShowString(6, (Cursor - 3) * 16 + 6, ">", OLED_6X8);
			for(int i = 0; i < page2MaxNum; i++)
			{
				OLED_ShowString(24, (i + 1) * 16, "XX:XX", OLED_8X16);
				OLED_ShowNum(24, (i + 1) * 16, AlarmList[i+2].Hour, 2, OLED_8X16);
				OLED_ShowNum(48, (i + 1) * 16, AlarmList[i+2].Minute, 2, OLED_8X16);
			}	
		}
		
		OLED_Update();
		
		//使用旋转编码器代替原先KEY2和KEY3的加减功能，上下移动光标
		if (Encoder_Down_Flag == 1 || Encoder_Up_Flag == 1)
		{
			int8_t delta = (Encoder_Up_Flag == 1) ? 1 : -1;
			Encoder_Down_Flag = 0;
			Encoder_Up_Flag = 0;
			Cursor += delta;
			if (Existed_AlarmNum < 5)
				Cursor = WrapAround(Cursor, 1, Existed_AlarmNum + 1);
			else if (Existed_AlarmNum == 5)
				Cursor = WrapAround(Cursor, 1, Existed_AlarmNum);
		}
		
		uint8_t key = Key_GetNum();
		
		//KEY1返回上级菜单，跳出while循环		
		if (key == 1)
		{
			break;
		}
		
//		//KEY2、KEY3上下移动光标
//		else if (key == 2 || key == 3)
//		{
//			int8_t delta = (key == 2) ? -1 : 1;
//			Cursor += delta;
//			if (Existed_AlarmNum < 5)
//				Cursor = WrapAround(Cursor, 1, Existed_AlarmNum + 1);
//			else if (Existed_AlarmNum == 5)
//				Cursor = WrapAround(Cursor, 1, Existed_AlarmNum);
//		}
		
		//KEY4进入光标所在行，调整闹钟
		else if (key == 4)
		{
			if (page == 1 && Cursor == 1 && page1MaxNum <= 2)
			{
				Add_Alarm();
			}
			else	EditAlarm(CurrentAlarm);
			SortAlarmByTime();
			Existed_AlarmNum = SumNumofAlarm();
			//对闹钟列表进行编辑后，调整光标位置
			if (Existed_AlarmNum < 5)
				Cursor = WrapAround(Cursor, 1, Existed_AlarmNum + 1);
			else if (Existed_AlarmNum == 5)
				Cursor = WrapAround(Cursor, 1, Existed_AlarmNum);
		}
	}
	//主菜单
	OLED_Clear();
	OLED_Update();
}

// 保存上次触发的分钟，防止重复触发
static uint16_t lastTriggerMinute = 61;

//闹钟触发检测函数，返回值=1说明有闹钟触发，返回值=0说明没有闹钟触发
uint8_t AlarmCheck(void)
{
	//MyRTC_ReadTime();
	uint16_t nowHour = MyRTC_Time[3];
    uint16_t nowMinute = MyRTC_Time[4];
    uint8_t  nowWeekday = MyRTC_Time[6];
	
	//如果当前minute已经触发过了，那么就不再触发闹钟
	if (nowMinute == lastTriggerMinute)
		return 0;
	
	//一共有ALARMNUM（5）个闹钟
	for (int i = 0; i < ALARMNUM; i++)
    {
		//alarm是指针（结构体类型的）
        AlarmPara *alarm = &AlarmList[i];

		//如果闹钟没有开启，或者当前闹钟属于删除状态，跳过本次循环，进入下一个闹钟的检测
        if (alarm->Status != 1)
            continue;

		//如果到了闹钟设定时间
        if (alarm->Hour == nowHour && alarm->Minute == nowMinute)
        {
			//使用变量hasRepeat判断这个闹钟是不是重复闹钟
            bool hasRepeat = false;
			
			//遍历周日到周六的重复设置
            for (int j = 0; j < 7; j++)
            {
				//某一天是 true，表示该闹钟是重复的
                if (alarm->Repetition[j])
                {
                    hasRepeat = true;
                    break;
                }
            }

            if (!hasRepeat)
            {
                alarm->Status = 0;  // 一次性闹钟触发后关闭
                lastTriggerMinute = nowMinute;
                return 1;
            }
			//如果今天是对应设定的星期，则触发闹钟响铃
            else if (alarm->Repetition[nowWeekday])
            {
                lastTriggerMinute = nowMinute;
                return 1;
            }
        }
    }
    return 0;  // 所有闹钟检查完毕，没有触发，返回0	
}


//全局变量
volatile uint8_t AlarmTriggeredFlag = 0;	//判断当前是否有闹钟触发


