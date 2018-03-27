/**
  ******************************************************************************
  * @file    Project/main.c 
  * @author  MCD Application Team
  * @version V2.2.0
  * @date    30-September-2014
  * @brief   Main program body
   ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 


/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "sysdefine.h"


Device_Param_Block m_device_param_block;
PowerOff_Save_Param_Block m_poweroff_save_param_block;

/* Private defines -----------------------------------------------------------*/	  
	  
/* Private function prototypes -----------------------------------------------*/


/* Private functions ---------------------------------------------------------*/

//毫秒延时函数
void DelayMs(unsigned char delay)
{
	unsigned int i;
	while(delay--)
	{
		i=1600;
		while(--i);
	}
}

//点亮所有LED灯
void SwitchAllLED(unsigned char status)
{
	if(status == ON)
	{
		OverVoltage_LED_ON();
		UnderVoltage_LED_ON();
		OverCurrent_LED_ON();
		LeakCurrent_LED_ON();
		Trouble_LED_ON();
		Silencer_LED_ON();
		RunLED_ON();
		Communication_ON();		
	}
	else if(status == OFF)
	{
		Beer_OFF();
		OverVoltage_LED_OFF();
		UnderVoltage_LED_OFF();
		OverCurrent_LED_OFF();
		LeakCurrent_LED_OFF();
		Trouble_LED_OFF();
		Silencer_LED_OFF();
		RunLED_OFF();
		Communication_OFF();		
	}
}


void main(void)
{
	Sysclock_init();								//时钟初始化
	disableInterrupts();							//屏蔽所有中断	
	HardWare_Init();								//硬件IO及定时器，ADC初始化
	
	Param_Init();									//参数初始化
	//在这里增加读取保存的数据及参数
	if(!Read_Device_Addr((unsigned char *)&m_poweroff_save_param_block,sizeof(m_poweroff_save_param_block)))	//读保存的参数失败，则初始化保存的参数
	{
		m_device_param_block.UnderVoltageLimit = 160;
		m_device_param_block.OverVoltageLimit = 270;
		m_device_param_block.OverCurrentLimit = 300;	//放大了10倍，主要是配合小数点
		m_device_param_block.LeakCurrentLimit = 30;
		m_device_param_block.UnderVoltageCnt = 0;
		m_device_param_block.OverVoltageCnt = 0;
		m_device_param_block.OverCurrentCnt = 0;
		m_device_param_block.LeakCurrentCnt = 0;
		m_device_param_block.AllToubleCnt = 0;
		m_device_param_block.Device_Address = 1;
		m_device_param_block.RecloseCntLimit = 5;
		m_device_param_block.CloseCnt = 0;
		m_device_param_block.Touble_Reason = 0;
		SaveParam();
	}
	else
	{
		m_device_param_block.UnderVoltageLimit = m_poweroff_save_param_block.UnderVoltageLimit;
		m_device_param_block.OverVoltageLimit = m_poweroff_save_param_block.OverVoltageLimit;
		m_device_param_block.OverCurrentLimit = m_poweroff_save_param_block.OverCurrentLimit;
		m_device_param_block.LeakCurrentLimit = m_poweroff_save_param_block.LeakCurrentLimit;
		m_device_param_block.UnderVoltageCnt = m_poweroff_save_param_block.UnderVoltageCnt;
		m_device_param_block.OverVoltageCnt = m_poweroff_save_param_block.OverVoltageCnt;
		m_device_param_block.OverCurrentCnt = m_poweroff_save_param_block.OverCurrentCnt;
		m_device_param_block.LeakCurrentCnt = m_poweroff_save_param_block.LeakCurrentCnt;
		m_device_param_block.AllToubleCnt = m_poweroff_save_param_block.AllToubleCnt;
		m_device_param_block.Device_Address = m_poweroff_save_param_block.Device_Address;
		m_device_param_block.RecloseCntLimit = m_poweroff_save_param_block.RecloseCntLimit;	
		m_device_param_block.CloseCnt = m_poweroff_save_param_block.CloseCnt;
		m_device_param_block.Touble_Reason = m_poweroff_save_param_block.Touble_Reason;		
	}
	
	SwitchAllLED(ON);								//打开所有LED
	for(int i=0;i<8;i++)							//延时2s
	{
		DelayMs(250);			
	}
	m_device_param_block.ADC_VEF = GetADC_Vef();	//获得AD参考电压

	SelectADC_Channel(VoltageChannel);				//选择通道
  	
	SwitchAllLED(OFF);								//关闭所有LED	
	Wdt_Init();
	m_device_param_block.RelayCtrl_Status = RelayReadyON_Status;
	enableInterrupts();								//中断使能
	
	while (1)
	{
		ADC_Sample_Manage();
		Event_Processing();
		DealKey();
		Digital_Display();
		Communication();
		KitWdt();
	}
}

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
