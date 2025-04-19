#include <time.h>
#include <stdbool.h>
#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "MyRTC.h"
#include "Key.h"
#include "OLED.h"

void TimeModify(void);

/*对年月日时分秒进行合理的限制，实现滚动************/
// 判断闰年
uint8_t is_leap_year(uint16_t year) {
    return ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));
}
//定义一个数组，确定每月有多少天，2月另作讨论
const uint8_t days_in_month[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

// 通用的循环范围封装函数
uint16_t WrapAround(uint16_t value, uint16_t min, uint16_t max) {
    if (value > max) 
	{
        if (value > 1000) return max; 	// 向下溢出超过0，（-1）则绕回到max
        return min;						// 向上溢出超过max，则绕回到min
    } 
	else if (value < min) 
	{
        return max;						// 向下溢出超过min，则绕回到max
    }
    return value;						//在min到max范围内，则返回原值
}

bool flag24_12 = true;//true表示24小时制，false表示12小时制

// 限制 TempRTC_Time 中的年月日时分秒范围
void AdjustRTC_TimeRange(uint16_t* TempRTC_Time) {
    // 年：1900 ~ 2100
    TempRTC_Time[0] = WrapAround(TempRTC_Time[0], 1900, 2100);

    // 月：1 ~ 12
    TempRTC_Time[1] = WrapAround(TempRTC_Time[1], 1, 12);

    // 日：1 ~ max_day，根据月份和是否闰年判断
    uint8_t max_day = days_in_month[TempRTC_Time[1] - 1];
    if (TempRTC_Time[1] == 2 && is_leap_year(TempRTC_Time[0])) {
        max_day = 29;
    }
    TempRTC_Time[2] = WrapAround(TempRTC_Time[2], 1, max_day);
	
	TempRTC_Time[3] = WrapAround(TempRTC_Time[3], 0, 23);// 24小时：0 ~ 23

    // 分：0 ~ 59
    TempRTC_Time[4] = WrapAround(TempRTC_Time[4], 0, 59);
	
    // 秒：0 ~ 59	
    TempRTC_Time[5] = WrapAround(TempRTC_Time[5], 0, 59);
}
/************************对年月日时分秒进行合理的限制，实现滚动*/


/*显示函数******************************************/
// 显示临时时间数组 TempRTC_Time
void DisplayTempTime(uint16_t* TempRTC_Time) 
{
    OLED_ShowNum(0, 0, TempRTC_Time[0], 4, OLED_6X8);//年
    OLED_ShowNum(30, 0, TempRTC_Time[1], 2, OLED_6X8);//月
    OLED_ShowNum(48, 0, TempRTC_Time[2], 2, OLED_6X8);//日

	uint8_t temp_hour = TempRTC_Time[3];
	bool is_pm = false;
	//如果是12小时制
	if (!flag24_12)
	{
		//中午12：00-23：59分应该对应12：00PM~11：59PM
		if (temp_hour >= 12)
		{
			is_pm = true;
			if (temp_hour > 12) temp_hour -= 12;
		}
		//半夜00：00应该对应12：00AM，而1：00~11：59无需转换
		else if (temp_hour == 0)
		{
			temp_hour = 12;
		}
		OLED_ShowString(108, 30, is_pm ? "PM" : "AM", OLED_6X8);
	}	
	else OLED_ClearArea(108, 30, 113, 37);

	OLED_ShowNum(32, 24, temp_hour, 2, OLED_8X16);//时	
    OLED_ShowNum(56, 24, TempRTC_Time[4], 2, OLED_8X16);//分
    OLED_ShowNum(80, 24, TempRTC_Time[5], 2, OLED_8X16);//秒
	
	time_t temp_cnt;
	struct tm t_temp;
    t_temp.tm_year = TempRTC_Time[0] - 1900;
    t_temp.tm_mon  = TempRTC_Time[1] - 1;
    t_temp.tm_mday = TempRTC_Time[2];
    t_temp.tm_hour = TempRTC_Time[3];
    t_temp.tm_min  = TempRTC_Time[4];
    t_temp.tm_sec  = TempRTC_Time[5];
	
	//星期转换
    temp_cnt = mktime(&t_temp);
	t_temp = *localtime(&temp_cnt);	
	
    OLED_Printf(110, 0, OLED_6X8, "%s", weekStr[t_temp.tm_wday]);//星期
    OLED_Update();
}



//获取硬件RTC的时间，显示全局数组 MyRTC_Time
void RTCTimeDisplay(void)
{
	MyRTC_ReadTime();
	//只会刷新屏幕上被修改显存的地方
	OLED_ShowNum(0, 0, MyRTC_Time[0], 4, OLED_6X8);//年
	OLED_ShowNum(30, 0, MyRTC_Time[1], 2, OLED_6X8);//月
	OLED_ShowNum(48, 0, MyRTC_Time[2], 2, OLED_6X8);//日
	
	uint8_t hour = MyRTC_Time[3];
	bool is_pm = false;
	//如果是12小时制
	if (!flag24_12)
	{
		//中午12：00-23：59分应该对应12：00PM~11：59PM
		if (hour >= 12)
		{
			is_pm = true;
			if (hour > 12) hour -= 12;
		}
		//半夜00：00应该对应12：00AM，而1：00~11：59无需转换
		else if (hour == 0)
		{
			hour = 12;
		}
		OLED_ShowString(108, 30, is_pm ? "PM" : "AM", OLED_6X8);
	}
	else OLED_ClearArea(108, 30, 113, 37);
	
	OLED_ShowNum(32, 24, hour, 2, OLED_8X16);//时
	OLED_ShowNum(56, 24, MyRTC_Time[4], 2, OLED_8X16);//分
	OLED_ShowNum(80, 24, MyRTC_Time[5], 2, OLED_8X16);//秒
	OLED_Printf(110, 0, OLED_6X8, "%s", weekStr[MyRTC_Time[6]]);//星期
	OLED_Update();
}

//连接符的显示
void Display_ClockTitle(void)
{
	//规范各个值显示的位置，并提前画好连接符
	OLED_ShowString(0, 0, "XXXX-XX-XX", OLED_6X8);		//年月日在左上角左对齐显示，像素的范围：X(00,59),Y(00,07)
	OLED_ShowString(32, 24, "XX:XX:XX", OLED_8X16);		//时间水平垂直居中显示，像素的范围：X(32,95),Y(24,29)
	OLED_ShowString(110, 0, "XXX", OLED_6X8);			//星期右上角右对齐显示，像素的范围：X(110,127),Y(00,07)	
}

//时钟信息显示（不开启修改模式时）

void ClockDisplay(void)
{
	Display_ClockTitle();
	while(1)
	{
		//按下的按键
		uint8_t key = Key_GetNum();
		//切换为修改时间模式
		if (key == 4)
		{
			TimeModify();
		}
		//24小时制和12小时制的切换
		else if (key == 3)
		{
			flag24_12 = ! flag24_12;
		}
		
		RTCTimeDisplay();		//显示RTC时钟
		
	}
}
/**************************************显示函数*/


/*时间修改函数**********************************/
void TimeModify(void)
{
	//显示时钟连接符
	Display_ClockTitle();
	
	/*TimePara 指示目前正在修改哪个时间参数
	 *可以的值为0~5，对应时、分、秒、年、月、日
	*/	
	uint8_t TimePara = 0;
	
	//闪烁光标位置
	uint16_t LinePosition[] = {32, 39, 47, 39};//X0 Y0 X1 Y1,默认在 时 这一位
	
	//临时存储数组，存储未保存前的值
	uint16_t TempRTC_Time[] = {MyRTC_Time[0], MyRTC_Time[1], MyRTC_Time[2], MyRTC_Time[3], MyRTC_Time[4], MyRTC_Time[5], MyRTC_Time[6]};
	
	while(1)
	{
		/*在进入修改模式的一瞬间，显示的是前一时刻RTC时间
		 *若无任何改动则会定格
		*若后续使用按键改动某一位，则会对应显示修改后的值
		**/
		DisplayTempTime(TempRTC_Time);
			
		//确定现在正在修改哪一位数据，以确定闪烁光标的位置
		switch(TimePara)
		{
			case 0:LinePosition[0] = 32;LinePosition[1] = 40;LinePosition[2] = 47;LinePosition[3] = 40;break;//时
			case 1:LinePosition[0] = 56;LinePosition[1] = 40;LinePosition[2] = 71;LinePosition[3] = 40;break;//分
			case 2:LinePosition[0] = 80;LinePosition[1] = 40;LinePosition[2] = 95;LinePosition[3] = 40;break;//秒
			case 3:LinePosition[0] = 0;LinePosition[1] = 8;LinePosition[2] = 23;LinePosition[3] = 8;break;//年
			case 4:LinePosition[0] = 30;LinePosition[1] = 8;LinePosition[2] = 41;LinePosition[3] = 8;break;//月
			case 5:LinePosition[0] = 48;LinePosition[1] = 8;LinePosition[2] = 59;LinePosition[3] = 8;break;//日
			default:break;
		}
		
		/*使用闪烁的下划线提示当前正在修改哪一位*/
		//RTC预分频器RTC_DIV由32767减计数到0代表1s的时间
		if (RTC_GetDivider() >= 16384)
		{
			OLED_DrawLine(LinePosition[0], LinePosition[1], LinePosition[2], LinePosition[3]);
		}
		else
		{
			OLED_ClearArea(LinePosition[0], LinePosition[1], LinePosition[2], LinePosition[3]);
		}
		OLED_Update();

		//按下的按键
		uint8_t key = Key_GetNum();
		
		/*按键功能**********************************/
		//如果按下KEY_1，就切换要修改的位
		if (key == 1)
		{
			//将光标移至下一个位置
			OLED_ClearArea(LinePosition[0], LinePosition[1], LinePosition[2], LinePosition[3]);
			OLED_UpdateArea(LinePosition[0], LinePosition[1], LinePosition[2], LinePosition[3]);
			
			//确保要修改的时间位在 年月日时分秒 中选择
			TimePara = (TimePara + 1) % 6;		
		}
		
		//如果按下KEY_2当前位增加1，如果按下KEY_3当前位减小1
		else if (key == 2 || key == 3)
		{
			int8_t delta = (key == 2) ? 1 : -1;
			
			//确保TimePara和TempRTC_Time数组的对应关系
			TempRTC_Time[(TimePara + 3) % 6] += delta;
			
			//保证对应的数在范围内滚动切换
			AdjustRTC_TimeRange(TempRTC_Time);
		}
		
		
		else if (key == 4) 
		{
			for (int i = 0; i < 6; ++i) {
				MyRTC_Time[i] = TempRTC_Time[i];
			}
			MyRTC_SetTime();
			OLED_Clear();
			OLED_ShowString(34, 28, "Time Saved", OLED_6X8);
			OLED_Update();
			Delay_ms(500);
			break;
		}
		/**********************************按键功能*/
	}
	OLED_Clear();
	OLED_Update();
	
	//保存后，回到时间显示状态
	ClockDisplay();
	
}

/*
*备注：	
*	时钟模块可以实现12/24小时制的显示
*	且在修改时间的状态下也能保持对应的时间进制设置
*	最初整个函数都是基于24小时制编写的，12小时制为后面增添的功能
*
*	在修改时间的状态下，只有DisplayTempTime这个函数相较24小时模式有所改变
*	因为RTC时间的存储，时间的上下限规定都是24小时制的
*	所以只需要让“显示”的时候，视觉上为24小时制即可
*	其他的底层原理都不需要改变
*/

