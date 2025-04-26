/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "MyRTC.h"
#include "LED.h"
#include "Buzzer.h"
#include "Encoder.h"
#include "Alarm.h"
#include "CountDownTimer.h"
#include "StopWatch.h"
#include "Menu.h"
#include "SolarToLunar.h"

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/



//定时器2中断函数，定时器2的定时频率为1Hz
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		solarToLunar(MyRTC_Time[0], MyRTC_Time[1], MyRTC_Time[2], &g_lunarDate);
		//当进入一级菜单时，倒计时十秒无操作，自动返回
		if (is_Lv_1_Menu_Open == 1)
		{
			MenuAutoBackSeconds -=1;
		}
		
		if (AlarmCheck() == 1)
		{
			AlarmTriggeredFlag = 1;
		}
		//如果闹钟触发
		if (AlarmTriggeredFlag)
		{
			AlarmTriggeredFlag = 0;		//清空触发状态，准备蜂鸣器响铃和LED1闪烁
			BuzzerActive = 1;			//蜂鸣器进入触发状态
			BuzzerStage = 0;			//蜂鸣器阶段重置
		}
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}

//定时器3中断函数，定时器3的定时频率为10Hz，优先级高于定时器2

uint8_t CountDownN = 0;
void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
	{
		//秒表相关
		if (StopWatch_isStart == 1)
		{
			StopWatch_deciSecond ++;
		}
		
		MyRTC_ReadTime();
		//倒计时的时间倒数
		if (CountDownTimer_StartFlag == 1)
		{
			CountDownN++;
			//以10hz计数，每计10个数，倒计时时间-1s
			if (CountDownN >= 10)
			{
				CountDownN = 0;
				//若倒计时没有计到0，则继续倒计时
				if (CountDown_Seconds > 0)
					CountDown_Seconds--;
				//若倒计时计到0，则触发响铃
				else if (CountDown_Seconds == 0)
				{
                    ResetTimer();
                    BuzzerActive = 1;  // 启动蜂鸣器提醒
					BuzzerStage = 0;			//蜂鸣器阶段重置
                }
			}
		}
		//响铃
		if (BuzzerActive == 1)
		{
			//响0.1s停0.1s再响0.1s停0.7s，实现响铃的效果
			if (BuzzerStage == 0 || BuzzerStage == 2)
			{
				Buzzer_ON();
				LED1_ON();
			}
			if (BuzzerStage == 1 || BuzzerStage == 3)
			{
				Buzzer_OFF();
				LED1_OFF();
			}

			BuzzerStage ++;
			if (BuzzerStage > 9)
				BuzzerStage = 0;		//循环下一秒
		}
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}

void EXTI9_5_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line5) != RESET)
    {
        // 按下 KEY5，响铃中则关闭
        if (BuzzerActive == 1)
        {
            Buzzer_OFF();
            LED1_OFF();
            BuzzerActive = 0;
        }

        // 清除中断标志
        EXTI_ClearITPendingBit(EXTI_Line5);
    }
}



/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
