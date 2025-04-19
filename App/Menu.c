#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"

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

/*时间待机显示主页*/
void HomePage(void)
{
	
}

void Menu(void)
{
	AboutMessage();
}
