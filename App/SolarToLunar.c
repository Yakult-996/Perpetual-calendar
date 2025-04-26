#include "stm32f10x.h"                  // Device header

/**
 * 阳历转农历算法 (1900-2100年)
 * 适用于STM32单片机
 * 使用RTC数组作为输入
 * MyRTC_Time[0]~MyRTC_Time[2]分别代表阳历的年月日
 */

#include <stdint.h>
#include <stdio.h>
#include "MyRTC.h"
#include "OLED.h"
#include "ClockModule.h"
#include "SolarToLunar.h"

LunarDate g_lunarDate;

// 农历数据表 1900-2100 年
// 每个数据的16进制表示为: 0x******
// 前4位表示闰月月份，后12位表示每个月大小月（1为大月30天，0为小月29天）
static const uint32_t LUNAR_INFO[] = 
{
    0x04bd8, 0x04ae0, 0x0a570, 0x054d5, 0x0d260, 0x0d950, 0x16554, 0x056a0, 0x09ad0, 0x055d2, // 1900-1909
    0x04ae0, 0x0a5b6, 0x0a4d0, 0x0d250, 0x1d255, 0x0b540, 0x0d6a0, 0x0ada2, 0x095b0, 0x14977, // 1910-1919
    0x04970, 0x0a4b0, 0x0b4b5, 0x06a50, 0x06d40, 0x1ab54, 0x02b60, 0x09570, 0x052f2, 0x04970, // 1920-1929
    0x06566, 0x0d4a0, 0x0ea50, 0x16a95, 0x05ad0, 0x02b60, 0x186e3, 0x092e0, 0x1c8d7, 0x0c950, // 1930-1939
    0x0d4a0, 0x1d8a6, 0x0b550, 0x056a0, 0x1a5b4, 0x025d0, 0x092d0, 0x0d2b2, 0x0a950, 0x0b557, // 1940-1949
    0x06ca0, 0x0b550, 0x15355, 0x04da0, 0x0a5b0, 0x14573, 0x052b0, 0x0a9a8, 0x0e950, 0x06aa0, // 1950-1959
    0x0aea6, 0x0ab50, 0x04b60, 0x0aae4, 0x0a570, 0x05260, 0x0f263, 0x0d950, 0x05b57, 0x056a0, // 1960-1969
    0x096d0, 0x04dd5, 0x04ad0, 0x0a4d0, 0x0d4d4, 0x0d250, 0x0d558, 0x0b540, 0x0b6a0, 0x195a6, // 1970-1979
    0x095b0, 0x049b0, 0x0a974, 0x0a4b0, 0x0b27a, 0x06a50, 0x06d40, 0x0af46, 0x0ab60, 0x09570, // 1980-1989
    0x04af5, 0x04970, 0x064b0, 0x074a3, 0x0ea50, 0x06b58, 0x05ac0, 0x0ab60, 0x096d5, 0x092e0, // 1990-1999
    0x0c960, 0x0d954, 0x0d4a0, 0x0da50, 0x07552, 0x056a0, 0x0abb7, 0x025d0, 0x092d0, 0x0cab5, // 2000-2009
    0x0a950, 0x0b4a0, 0x0baa4, 0x0ad50, 0x055d9, 0x04ba0, 0x0a5b0, 0x15176, 0x052b0, 0x0a930, // 2010-2019
    0x07954, 0x06aa0, 0x0ad50, 0x05b52, 0x04b60, 0x0a6e6, 0x0a4e0, 0x0d260, 0x0ea65, 0x0d530, // 2020-2029
    0x05aa0, 0x076a3, 0x096d0, 0x04afb, 0x04ad0, 0x0a4d0, 0x1d0b6, 0x0d250, 0x0d520, 0x0dd45, // 2030-2039
    0x0b5a0, 0x056d0, 0x055b2, 0x049b0, 0x0a577, 0x0a4b0, 0x0aa50, 0x1b255, 0x06d20, 0x0ada0, // 2040-2049
    0x14b63, 0x09370, 0x049f8, 0x04970, 0x064b0, 0x168a6, 0x0ea50, 0x06b20, 0x1a6c4, 0x0aae0, // 2050-2059
    0x092e0, 0x0d2e3, 0x0c960, 0x0d557, 0x0d4a0, 0x0da50, 0x05d55, 0x056a0, 0x0a6d0, 0x055d4, // 2060-2069
    0x052d0, 0x0a9b8, 0x0a950, 0x0b4a0, 0x0b6a6, 0x0ad50, 0x055a0, 0x0aba4, 0x0a5b0, 0x052b0, // 2070-2079
    0x0b273, 0x06930, 0x07337, 0x06aa0, 0x0ad50, 0x14b55, 0x04b60, 0x0a570, 0x054e4, 0x0d160, // 2080-2089
    0x0e968, 0x0d520, 0x0daa0, 0x16aa6, 0x056d0, 0x04ae0, 0x0a9d4, 0x0a2d0, 0x0d150, 0x0f252, // 2090-2099
    0x0d520                                                                                   // 2100
};

// 天干
static const char* CELESTIAL_STEMS[] = {"甲", "乙", "丙", "丁", "戊", "己", "庚", "辛", "壬", "癸"};

// 地支
static const char* TERRESTRIAL_BRANCHES[] = {
    "子", "丑", "寅", "卯", "辰", "巳", "午", "未", "申", "酉", "戌", "亥"
};

// 生肖
static const char* CHINESE_ZODIAC[] = {
    "鼠", "牛", "虎", "兔", "龙", "蛇", "马", "羊", "猴", "鸡", "狗", "猪"
};

// 农历月份
static const char* LUNAR_MONTH[] = {
    "正", "二", "三", "四", "五", "六", "七", "八", "九", "十", "冬", "腊"
};

// 农历日期
static const char* LUNAR_DAY[] = {
    "初一", "初二", "初三", "初四", "初五", "初六", "初七", "初八", "初九", "初十",
    "十一", "十二", "十三", "十四", "十五", "十六", "十七", "十八", "十九", "二十",
    "廿一", "廿二", "廿三", "廿四", "廿五", "廿六", "廿七", "廿八", "廿九", "三十"
};



// 判断阳历年是否为闰年
uint8_t isLeapYear(uint16_t year) {
    return ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));
}

// 获取阳历月的天数
uint8_t getSolarMonthDays(uint16_t year, uint8_t month) {
    const uint8_t monthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    if (month == 2 && isLeapYear(year)) {
        return 29;
    }
    
    return monthDays[month - 1];
}

// 计算从1900年1月31日到指定阳历日期的天数
//这个函数的目的是计算从1900年1月31日（作为基准日期）到指定阳历日期的总天数
//这个基准日期很重要，因为1900年1月31日对应农历1900年正月初一，是农历计算的起点。
uint32_t solarDaysFromBase(uint16_t year, uint8_t month, uint8_t day) {
    uint32_t days = 0;
    
    // 年份天数累加
    for (uint16_t i = 1900; i < year; i++) {
        days += isLeapYear(i) ? 366 : 365;
    }
    
    // 月份天数累加
    for (uint8_t i = 1; i < month; i++) {
        days += getSolarMonthDays(year, i);
    }
    
    // 加上当月天数
    days += day;
    
    // 1900年1月31日为基准日期，需要减去31天
    return days - 31;
}

// 获取农历年的闰月月份，0表示无闰月
uint8_t getLeapMonth(uint16_t lunarYear) {
    if (lunarYear < 1900 || lunarYear > 2100) {
        return 0;
    }
    return (LUNAR_INFO[lunarYear - 1900] & 0xF);
}

// 获取农历年指定月份的天数（包括判断闰月）
//uint8_t getLunarMonthDays(uint16_t lunarYear, uint8_t lunarMonth, uint8_t isLeapMonth) {
//    if (lunarYear < 1900 || lunarYear > 2100) {
//        return 0;
//    }
//    
//    uint32_t lunarInfo = LUNAR_INFO[lunarYear - 1900];
//    uint8_t leapMonth = getLeapMonth(lunarYear);
//    
//    // 处理非闰月
//    if (!isLeapMonth) {
//        if (lunarMonth > 12) {
//            return 0;
//        }
//        
//        // 调整月份位置
//        uint8_t monthBit = lunarMonth > leapMonth ? lunarMonth : lunarMonth - 1;
//        
//        if ((lunarInfo & (1 << (12 - monthBit))) == 0) {
//            return 29;  // 小月
//        } else {
//            return 30;  // 大月
//        }
//    }
//    
//    // 处理闰月
//    if (leapMonth != lunarMonth) {
//        return 0;  // 不是闰月
//    }
//    
//    // 获取闰月大小
//    if ((lunarInfo & (1 << 4)) == 0) {
//        return 29;  // 小月
//    } else {
//        return 30;  // 大月
//    }
//}

uint8_t getLunarMonthDays(uint16_t lunarYear, uint8_t lunarMonth, uint8_t isLeapMonth) {
    if (lunarYear < 1900 || lunarYear > 2100) return 0;

    uint32_t lunarInfo = LUNAR_INFO[lunarYear - 1900];
    uint8_t leapMonth = getLeapMonth(lunarYear);

    if (!isLeapMonth) {
        if (lunarMonth < 1 || lunarMonth > 12) return 0;
        uint8_t monthBit = lunarMonth - 1;
        return (lunarInfo & (0x8000 >> monthBit)) ? 30 : 29;
    } else {
        if (leapMonth != lunarMonth) return 0;
        return (lunarInfo & 0x10000) ? 30 : 29;
    }
}



// 获取农历年的总天数
uint16_t getLunarYearDays(uint16_t lunarYear) {
    if (lunarYear < 1900 || lunarYear > 2100) {
        return 0;
    }
    
    uint16_t days = 0;
    uint8_t leapMonth = getLeapMonth(lunarYear);
    
    // 计算12个月的天数
    for (uint8_t i = 1; i <= 12; i++) {
        days += getLunarMonthDays(lunarYear, i, 0);
    }
    
    // 如果有闰月，加上闰月的天数
    if (leapMonth > 0) {
        days += getLunarMonthDays(lunarYear, leapMonth, 1);
    }
    
    return days;
}

// 农历年的第一天距离1900年1月31日的天数
uint32_t getLunarFirstDayOffset(uint16_t lunarYear) {
    if (lunarYear < 1900 || lunarYear > 2100) {
        return 0;
    }
    
    uint32_t offset = 0;
    
    // 累加1900年到指定年份前一年的所有天数
    for (uint16_t i = 1900; i < lunarYear; i++) {
        offset += getLunarYearDays(i);
    }
    
    return offset;
}

// 阳历日期转农历日期
void solarToLunar(uint16_t solarYear, uint8_t solarMonth, uint8_t solarDay, LunarDate* lunarDate) {
    // 输入检查
    if (solarYear < 1900 || solarYear > 2100 || solarMonth < 1 || solarMonth > 12) {
        lunarDate->lunarYear = 0;
        lunarDate->lunarMonth = 0;
        lunarDate->lunarDay = 0;
        lunarDate->isLeapMonth = 0;
        return;
    }
    
    // 计算当前阳历距离1900年1月31日的天数
    uint32_t offset = solarDaysFromBase(solarYear, solarMonth, solarDay);
    
    // 用于查找农历年
    uint16_t lunarYear = 1900;
    uint32_t daysInLunarYear = 0;
    
    // 确定农历年
	//通过不断减去每个农历年的总天数来确定阳历日期落在哪个农历年
    while (lunarYear <= 2100) {
        daysInLunarYear = getLunarYearDays(lunarYear);
		//如果偏移量小于当年的总天数，说明找到了对应的农历年
        if (offset < daysInLunarYear) {
            break;
        }
		//否则，从偏移量中减去当年的天数，继续查找下一年
        offset -= daysInLunarYear;
        lunarYear++;
    }
    
    // 确定农历月和是否为闰月
    uint8_t lunarMonth = 1;							//农历月
    uint8_t isLeapMonth = 0;						//这一年是否闰月
    uint8_t leapMonth = getLeapMonth(lunarYear);	//闰月
    uint8_t daysInMonth = 0;
    
    // 在农历年中查找月份
	//从农历正月开始遍历
	//获取当前月的天数
	//如果偏移量小于当月天数，找到了对应月份
	//否则减去当月天数，继续查找
	//特别处理闰月情况：如果当前月是闰月，先判断是否落在闰月内，若是则标记isLeapMonth=1
    while (offset > 0 && lunarMonth <= 12) 
	{
        // 当前月天数
        daysInMonth = getLunarMonthDays(lunarYear, lunarMonth, 0);
        
        if (offset < daysInMonth) {
            isLeapMonth = 0;
            break;
        }
        
        offset -= daysInMonth;
        
        // 处理闰月
        if (leapMonth == lunarMonth) {
            daysInMonth = getLunarMonthDays(lunarYear, lunarMonth, 1);
            
            if (offset < daysInMonth) {
                isLeapMonth = 1;
                break;
            }
            
            offset -= daysInMonth;
        }
        
        lunarMonth++;
    }
    
    // 农历日为offset+1
    uint8_t lunarDay = offset + 1;
    
    // 设置结果
    lunarDate->lunarYear = lunarYear;
    lunarDate->lunarMonth = lunarMonth;
    lunarDate->lunarDay = lunarDay;
    lunarDate->isLeapMonth = isLeapMonth;
    
    // 计算天干地支年
	//这里计算农历年的天干地支索引
	//减4是因为历史上第一个甲子年（天干地支纪年的起点）比1900年早了4年。
    uint8_t stemIndex = (lunarYear - 4) % 10;
    uint8_t branchIndex = (lunarYear - 4) % 12;
    sprintf(lunarDate->lunarYearString, "%s%s年", CELESTIAL_STEMS[stemIndex], TERRESTRIAL_BRANCHES[branchIndex]);
    
    // 计算生肖
    sprintf(lunarDate->zodiac, "%s", CHINESE_ZODIAC[branchIndex]);
    
    // 设置农历月份字符串
    if (isLeapMonth) {
        sprintf(lunarDate->lunarMonthString, "闰%s月", LUNAR_MONTH[lunarMonth - 1]);
    } else {
        sprintf(lunarDate->lunarMonthString, "%s月", LUNAR_MONTH[lunarMonth - 1]);
    }
    
    // 设置农历日期字符串
    sprintf(lunarDate->lunarDayString, "%s", LUNAR_DAY[lunarDay - 1]);
}






