# 📅 万年历 / Perpetual Calendar

基于 **STM32F103C8T6** 的万年历课程设计项目，已实现基础功能，支持阳历、阴历、闹钟、倒计时等常用功能。

A **perpetual calendar** project based on **STM32F103C8T6**, designed as a simple embedded systems course project. Basic features such as time display, alarm, countdown, and lunar calendar conversion are implemented.

---

## ✅ 已实现功能 / Implemented Features (V1.0)

### 📆 时间显示 / Time Display
- 显示 年、月、日、时、分、秒、星期  
  Display of year, month, day, hour, minute, second, and weekday  
- 支持 12 小时制与 24 小时制切换  
  Supports 12-hour and 24-hour time formats  

### ⏰ 闹钟 / Alarm
- 最多添加 5 个闹钟  
  Up to 5 alarms  
- 支持单次响铃与指定日期循环响铃  
  One-time or specific-date repeating alarms  
- 自动按时间排序  
  Auto-sorted by alarm time  
- 闹钟响铃时蜂鸣器响，LED 闪烁  
  Buzzer sounds and LED blinks when triggered  

### ⏳ 倒计时 / Countdown
- 倒计时结束蜂鸣器与 LED 提示  
  Buzzer and LED notification on timeout  
- 倒计时过程中可切换界面，计时不中断  
  Timer continues running when switching pages  
- 支持暂停与复位  
  Supports pause and reset  

### ⌚ 秒表 / Stopwatch
- 最大时间 99:99:99  
  Max time: 99:99:99  
- 可在其他界面后台运行  
  Runs in background  
- 支持暂停与复位  
  Supports pause and reset  

### 🛠️ 时间设置 / Manual Time Setting
- 支持手动设置年月日时分秒  
  Manual setting of year, month, day, hour, minute, second  
- V1.0 仅实现手动设置，未来计划支持网络授时  
  Only manual setting in V1.0; network sync planned in future  

### 🗓️ 日历与阴历转换 / Calendar & Lunar Conversion
- 根据阳历自动计算对应阴历  
  Automatically convert solar to lunar date  
- 计算起始时间为 1900 年 1 月 31 日  
  Supports conversion starting from Jan 31, 1900  

---

## 🚧 V2.0 版本前瞻 / Future Plans (V2.0 - Unfinished)

### 🌐 网络自动授时 / Network Time Sync
- 计划使用 ESP8266（ESP01）模块 + AT 指令获取网络时间  
  Planned to use ESP8266 + AT commands for online time sync  
- 因时间关系未能实现  
  Not implemented due to time constraints  

### 🌤️ 天气功能 / Weather Display
- 通过 WiFi 模块获取天气数据  
  Planned weather display using WiFi module  
- 菜单中保留此项，但当前未启用  
  Option remains in menu but is non-functional  

### 📅 节假日与节气显示 / Holidays & Solar Terms
- 原计划通过网络同步显示节假日、节气  
  Planned to display holidays and solar terms via network  
- 暂未开发  
  Not implemented  

---

## 📌 开发说明 / Notes

- 当前版本：`V1.0`  
  Current version: `V1.0`
- 开发板型号：`STM32F103C8T6`  
  Board: `STM32F103C8T6`
- 网络功能与天气功能已不再更新，可删除菜单中的天气选项  
  Network/weather features are no longer updated; feel free to remove the weather option from the menu

---

## 🙌 欢迎参与 / Contribution

欢迎 fork、issue 或 PR 来改进此项目。  
Feel free to fork, open issues, or submit PRs to improve this project!

