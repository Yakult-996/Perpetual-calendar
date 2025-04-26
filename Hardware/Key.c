#include "stm32f10x.h"                  // Device header
#include "Delay.h"

/**
  * 函    数：按键初始化
  * 参    数：无
  * 返 回 值：无
  */
void Key_Init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		//开启GPIOA的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);		//开启GPIOB的时钟
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);						//将PB10和PB11引脚初始化为上拉输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_5;
	GPIO_Init(GPIOA, &GPIO_InitStructure);						//将PA5、PA6和PA7引脚初始化为上拉输入
	
	//PB10 -----KEY_1
	//PB11 -----KEY_2
	//PA6  -----KEY_3
	//PA7  -----KEY_4
	//PA5  -----KEY_5
	
	
	//将KEY_5用作停止闹钟响铃的按键
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource5);

	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line5;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  // 下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;  // EXTI5~9 共用一个中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/**
  * 函    数：按键获取键码
  * 参    数：无
  * 返 回 值：按下按键的键码值，范围：0~2，返回0代表没有按键按下
  * 注意事项：此函数是阻塞式操作，当按键按住不放时，函数会卡住，直到按键松手
  */
uint8_t Key_GetNum(void)
{
	uint8_t KeyNum = 0;		//定义变量，默认键码值为0
	
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) == 0)			//读PB10输入寄存器的状态，如果为0，则代表按键1按下
	{
		Delay_ms(20);											//延时消抖
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) == 0);	//等待按键松手
		Delay_ms(20);											//延时消抖
		KeyNum = 1;												//置键码为1
	}
	
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == 0)			//读PB11输入寄存器的状态，如果为0，则代表按键2按下
	{
		Delay_ms(20);											//延时消抖
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == 0);	//等待按键松手
		Delay_ms(20);											//延时消抖
		KeyNum = 2;												//置键码为2
	}
	
	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) == 0)			//读PA6输入寄存器的状态，如果为0，则代表按键3按下
	{
		Delay_ms(20);											//延时消抖
		while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) == 0);	//等待按键松手
		Delay_ms(20);											//延时消抖
		KeyNum = 3;												//置键码为3
	}
	
	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7) == 0)			//读PA7输入寄存器的状态，如果为0，则代表按键4按下
	{
		Delay_ms(20);											//延时消抖
		while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7) == 0);	//等待按键松手
		Delay_ms(20);											//延时消抖
		KeyNum = 4;												//置键码为4
	}
	
//	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5) == 0)			//读PA5输入寄存器的状态，如果为0，则代表按键5按下
//	{
//		Delay_ms(20);											//延时消抖
//		while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5) == 0);	//等待按键松手
//		Delay_ms(20);											//延时消抖
//		KeyNum = 5;												//置键码为5
//	}
	
	return KeyNum;			//返回键码值，如果没有按键按下，所有if都不成立，则键码为默认值0
}

//void EXTI9_5_IRQHandler(void)
//{
//    if (EXTI_GetITStatus(EXTI_Line5) != RESET)
//    {
//        // 按下 KEY5


//        // 清除中断标志
//        EXTI_ClearITPendingBit(EXTI_Line5);
//    }
//}


