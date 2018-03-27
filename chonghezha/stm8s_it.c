/**
  ******************************************************************************
  * @file    stm8s_it.c
  * @author  MCD Application Team
  * @version V2.2.0
  * @date    30-September-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all peripherals interrupt service 
  *          routine.
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
#include "stm8s_it.h"
#include "bsp.h"
#include "sysdefine.h"

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/

#ifdef _COSMIC_
/**
  * @brief Dummy Interrupt routine
  * @par Parameters:
  * None
  * @retval
  * None
*/
INTERRUPT_HANDLER(NonHandledInterrupt, 25)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}
#endif /*_COSMIC_*/

/**
  * @brief TRAP Interrupt routine
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER_TRAP(TRAP_IRQHandler)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief Top Level Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TLI_IRQHandler, 0)

{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief Auto Wake Up Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(AWU_IRQHandler, 1)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief Clock Controller Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(CLK_IRQHandler, 2)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief External Interrupt PORTA Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI_PORTA_IRQHandler, 3)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief External Interrupt PORTB Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI_PORTB_IRQHandler, 4)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief External Interrupt PORTC Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI_PORTC_IRQHandler, 5)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief External Interrupt PORTD Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI_PORTD_IRQHandler, 6)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief External Interrupt PORTE Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI_PORTE_IRQHandler, 7)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

#if defined (STM8S903) || defined (STM8AF622x) 
/**
  * @brief External Interrupt PORTF Interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(EXTI_PORTF_IRQHandler, 8)
 {
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
 }
#endif /* (STM8S903) || (STM8AF622x) */

#if defined (STM8S208) || defined (STM8AF52Ax)
/**
  * @brief CAN RX Interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(CAN_RX_IRQHandler, 8)
 {
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
 }

/**
  * @brief CAN TX Interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(CAN_TX_IRQHandler, 9)
 {
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
 }
#endif /* (STM8S208) || (STM8AF52Ax) */

/**
  * @brief SPI Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(SPI_IRQHandler, 10)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief Timer1 Update/Overflow/Trigger/Break Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM1_UPD_OVF_TRG_BRK_IRQHandler, 11)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief Timer1 Capture/Compare Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM1_CAP_COM_IRQHandler, 12)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

#if defined (STM8S903) || defined (STM8AF622x)
/**
  * @brief Timer5 Update/Overflow/Break/Trigger Interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(TIM5_UPD_OVF_BRK_TRG_IRQHandler, 13)
 {
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
 }
 
/**
  * @brief Timer5 Capture/Compare Interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(TIM5_CAP_COM_IRQHandler, 14)
 {
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
 }

#else /* (STM8S208) || (STM8S207) || (STM8S105) || (STM8S103) || (STM8AF62Ax) || (STM8AF52Ax) || (STM8AF626x) */
/**
  * @brief Timer2 Update/Overflow/Break Interrupt routine.
  * @param  None
  * @retval None
  */
 unsigned char Time2count=0;

 INTERRUPT_HANDLER(TIM2_UPD_OVF_BRK_IRQHandler, 13)
 {
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
	TIM2_ClearFlag(TIM2_FLAG_UPDATE);
	ADC1_StartConversion();
	Time2count++;
	if(Time2count>10)
	{
		Time2count = 0;
	}
 }

/**
  * @brief Timer2 Capture/Compare Interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(TIM2_CAP_COM_IRQHandler, 14)
 {
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
 }
#endif /* (STM8S903) || (STM8AF622x) */

#if defined (STM8S208) || defined(STM8S207) || defined(STM8S007) || defined(STM8S105) || \
    defined(STM8S005) ||  defined (STM8AF62Ax) || defined (STM8AF52Ax) || defined (STM8AF626x)
/**
  * @brief Timer3 Update/Overflow/Break Interrupt routine.
  * @param  None
  * @retval None
  */
 unsigned int Time3count=0;
 unsigned int Time3count1=0;
 unsigned int Time3count2=0;
 unsigned int Time3count3=0;
 INTERRUPT_HANDLER(TIM3_UPD_OVF_BRK_IRQHandler, 15)
 {
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
	TIM3_ClearFlag(TIM3_FLAG_UPDATE);			//���жϱ�ʶ
	Time3count1++;								//��������1
	Time3count2++;
	Time3count3++;
	if(m_device_param_block.ComRevTimeout < 20)
	{
		m_device_param_block.ComRevTimeout++;
	}
	//�������죬��100ms��رշ��������ﵽ���°�������һ����Ч��
	if(m_device_param_block.BeerONFlag == 1)	
	{
		Time3count ++;
		//ʱ��ﵽ100ms���߰����Ѿ��ɿ�����رշ�����
		if((Time3count>100) || (m_device_param_block.Key_Value == KEY_NONE))
		{
			Beer_OFF();								//�رշ�����
			Time3count = 0;							//����������
			m_device_param_block.BeerONFlag = 0;	//���������ʶλ����
		}		
	}
	//�������¼�ʱ
	if(Time3count1>100)
	{
		Time3count1 = 0;
		if(m_device_param_block.Key_Value != KEY_NONE)
			m_device_param_block.KeyDelayNs++;
	}
	//LEDָʾ����˸������ 0.5s��ת
	if(Time3count3==500)
	{
		//�鿴ģʽ���ҷ�׼���غ�բģʽ
		if(((m_device_param_block.Device_Status & 0x1F) < 0x03)         || 
		    ((m_device_param_block.Device_Status == ShutDownLED_Status) || 
		    (m_device_param_block.RelayCtrl_Status == RelayWaitON_Status)))
		{
			//����й�ѹ���ϣ����ѹָʾ�Ʒ�ת
			if((m_device_param_block.Touble_Flag &0x01) == 0x01)
			{
				OverVoltage_LED_ON();
			}
			//�����Ƿѹ���ϣ���Ƿѹָʾ�Ʒ�ת
			if((m_device_param_block.Touble_Flag &0x02) == 0x02)
			{
				UnderVoltage_LED_ON();
			}
			//����й������ϣ������ָʾ�Ʒ�ת
			if((m_device_param_block.Touble_Flag &0x04) == 0x04)
			{
				OverCurrent_LED_ON();
			}
			//�����©����ϣ���©��ָʾ�Ʒ�ת
			if((m_device_param_block.Touble_Flag &0x08) == 0x08)
			{
				LeakCurrent_LED_ON();
			}			
		}
		//�й���,�����������������ת
		if((m_device_param_block.Touble_Flag != 0x00) && (m_device_param_block.SilencerFlag == 0))
			Beer_ON();
		if(m_device_param_block.Touble_Flag == 0x00)
			Beer_OFF();
		//û���κι��ϣ������ڲ鿴ģʽ�£����еƷ�ת
		if((m_device_param_block.Touble_Flag == 0) && ((m_device_param_block.Device_Status &0x10) == 0x00) && (m_device_param_block.RemoteCtrl_Status != RemoteCtrl_OFF_Status))
		{
			RunLED_Toggle();
		}
	}
	else if(Time3count3==515)
	{
		Beer_OFF();
	}
	else if(Time3count3==1000)
	{
		Time3count3 = 0;
		if((m_device_param_block.Touble_Flag == 0) && ((m_device_param_block.Device_Status &0x10) == 0x00) && (m_device_param_block.RemoteCtrl_Status != RemoteCtrl_OFF_Status))
			RunLED_OFF();
		if(((m_device_param_block.Device_Status & 0x1F) < 0x03)			|| 
		    ((m_device_param_block.Device_Status == ShutDownLED_Status) || 
		    (m_device_param_block.RelayCtrl_Status == RelayWaitON_Status)))
		{
			OverVoltage_LED_OFF();
			UnderVoltage_LED_OFF();
			OverCurrent_LED_OFF();
			LeakCurrent_LED_OFF();			
		}
	}
	//����ʱ�������
	if(Time3count2>1000)
	{
		Time3count2 = 0;
		m_device_param_block.Run_Timer++;		//����ʱ���1s
		m_device_param_block.OverCurrent_DelayTime ++;
		if(m_device_param_block.OverCurrent_DelayTime>100)
			m_device_param_block.OverCurrent_DelayTime = 0;
		if(m_device_param_block.Run_Timer>4000)	//����4000�룬������
		{
			m_device_param_block.Run_Timer = 0;
		}
		if(m_device_param_block.Key_Value == KEY_NONE)	//����û����������
		{
			m_device_param_block.Key_IDEL_Delay++;
		}
		else
		{
			m_device_param_block.Key_IDEL_Delay = 0;	//����
		}
	}
 }

/**
  * @brief Timer3 Capture/Compare Interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(TIM3_CAP_COM_IRQHandler, 16)
 {
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
 }
#endif /* (STM8S208) || (STM8S207) || (STM8S105) || (STM8AF62Ax) || (STM8AF52Ax) || (STM8AF626x) */

#if defined (STM8S208) || defined(STM8S207) || defined(STM8S007) || defined(STM8S103) || \
    defined(STM8S003) ||  defined (STM8AF62Ax) || defined (STM8AF52Ax) || defined (STM8S903)
/**
  * @brief UART1 TX Interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(UART1_TX_IRQHandler, 17)
 {
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
 }

/**
  * @brief UART1 RX Interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(UART1_RX_IRQHandler, 18)
 {
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
 }
#endif /* (STM8S208) || (STM8S207) || (STM8S103) || (STM8S903) || (STM8AF62Ax) || (STM8AF52Ax) */

#if defined(STM8AF622x)
/**
  * @brief UART4 TX Interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(UART4_TX_IRQHandler, 17)
 {
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
 }

/**
  * @brief UART4 RX Interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(UART4_RX_IRQHandler, 18)
 {
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
 }
#endif /* (STM8AF622x) */

/**
  * @brief I2C Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(I2C_IRQHandler, 19)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

#if defined(STM8S105) || defined(STM8S005) ||  defined (STM8AF626x)
/**
  * @brief UART2 TX interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(UART2_TX_IRQHandler, 20)
 {
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
 }

/**
  * @brief UART2 RX interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(UART2_RX_IRQHandler, 21)
 {
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
	unsigned char reveivedata;
	m_device_param_block.ComRevTimeout = 0;
	(void) UART2->SR;	//��оƬ����ǰ�����һֱ�д��������յ������OR��λ������һֱ���жϣ�
						//�ȶ�SR״̬�Ĵ����ٶ����ݼĴ����ſ����������ؼ������
	reveivedata = UART2_ReceiveData8();			//�������ݵ�������
	UART2_ClearITPendingBit(UART2_IT_RXNE);		//���жϱ�־
	m_device_param_block.Lenth++;
	if(m_device_param_block.Lenth < Maxbytenum)
	{
		if(m_device_param_block.Lenth >= 1)
		{
			m_device_param_block.ComRxBuf[m_device_param_block.Lenth - 1] = reveivedata;
		}
		else
		{
			m_device_param_block.ComRxBuf[0] = reveivedata;
		}
	}
	else
	{
		m_device_param_block.Lenth = 0;
	}
 }
#endif /* (STM8S105) || (STM8AF626x) */

#if defined(STM8S207) || defined(STM8S007) || defined(STM8S208) || defined (STM8AF52Ax) || defined (STM8AF62Ax)
/**
  * @brief UART3 TX interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(UART3_TX_IRQHandler, 20)
 {
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
 }

/**
  * @brief UART3 RX interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(UART3_RX_IRQHandler, 21)
 {
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
 }
#endif /* (STM8S208) || (STM8S207) || (STM8AF52Ax) || (STM8AF62Ax) */

#if defined(STM8S207) || defined(STM8S007) || defined(STM8S208) || defined (STM8AF52Ax) || defined (STM8AF62Ax)
/**
  * @brief ADC2 interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(ADC2_IRQHandler, 22)
 {
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
 }
#else /* STM8S105 or STM8S103 or STM8S903 or STM8AF626x or STM8AF622x */
/**
  * @brief ADC1 interrupt routine.
  * @par Parameters:
  * None
  * @retval 
  * None
  */
 INTERRUPT_HANDLER(ADC1_IRQHandler, 22)
 {
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
	 static unsigned char Conversioncnt=0;
	 ADC1_ClearITPendingBit(ADC1_IT_EOC);										//���жϱ�ʶλ
	 m_device_param_block.Adcbuf[Conversioncnt++] = ADC1_GetConversionValue();	//����AD����ֵ
	 if(Conversioncnt>19)														//����20��
	 {
		 Conversioncnt = 0;
		 m_device_param_block.ADCPeriodSampleOkFlag = 1;						//���ڲ�����־λ��1
		 TIM2_Cmd(DISABLE);														//ֹͣ��ʱ��2
	 }
 }
#endif /* (STM8S208) || (STM8S207) || (STM8AF52Ax) || (STM8AF62Ax) */

#if defined (STM8S903) || defined (STM8AF622x)
/**
  * @brief Timer6 Update/Overflow/Trigger Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM6_UPD_OVF_TRG_IRQHandler, 23)
 {
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
 }
#else /* STM8S208 or STM8S207 or STM8S105 or STM8S103 or STM8AF52Ax or STM8AF62Ax or STM8AF626x */
/**
  * @brief Timer4 Update/Overflow Interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(TIM4_UPD_OVF_IRQHandler, 23)
 {
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
	static unsigned int time4_count=0;
	static unsigned int time4_count1=0;
	static unsigned char ledtwinkleflag=0;
	
	TIM4_ClearFlag(TIM4_FLAG_UPDATE);			//���жϱ�־
	//ɨ�������
	ScanLED(m_device_param_block.LedDispCode,ledtwinkleflag,m_device_param_block.LedDatabitcount,m_device_param_block.Unitbitcount);
	if(m_device_param_block.LedTwinkleFlag == 1)			//��˸��ʶλ��1
	{
		time4_count++;										//��������1
		if(time4_count>500)									//500ms ������˸
		{
			time4_count = 0;
			ledtwinkleflag = ~ledtwinkleflag;				
		}		
	}
	else if(m_device_param_block.LedTwinkleFlag == 0)		//��˸��ʶλ����
	{
		ledtwinkleflag = 0;
	}
	time4_count1++;											//������1��1
	if(time4_count1>100)									//50ms
	{
		time4_count1 = 0;
		m_device_param_block.LedUpdateFlag = 1;				//�������ʾ���ָ��±�־λ��1
	}
 }
#endif /* (STM8S903) || (STM8AF622x)*/

/**
  * @brief Eeprom EEC Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EEPROM_EEC_IRQHandler, 24)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/