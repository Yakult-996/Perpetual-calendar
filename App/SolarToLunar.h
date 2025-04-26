#include <stdint.h>
#ifndef __SOLARTOLUNAR_H
#define __SOLARTOLUNAR_H

// 农历日期结构体
typedef struct {
    uint16_t lunarYear;      // 农历年
    uint8_t lunarMonth;      // 农历月
    uint8_t lunarDay;        // 农历日
    uint8_t isLeapMonth;     // 是否为闰月
    char lunarYearString[16]; // 天干地支年份
    char lunarMonthString[12]; // 农历月份字符串
    char lunarDayString[8];   // 农历日期字符串
    char zodiac[8];           // 生肖
} LunarDate;
extern LunarDate g_lunarDate;

extern uint16_t SolarYear;
extern uint8_t SolarMonth;
extern uint8_t SolarDay;

void solarToLunar(uint16_t solarYear, uint8_t solarMonth, uint8_t solarDay, LunarDate* lunarDate); 


#endif
