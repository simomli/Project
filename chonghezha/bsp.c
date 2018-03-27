#include "stm8s.h"
#include "bsp.h"
#include "sysdefine.h"
#include "math.h"

//系统时钟初始化
void Sysclock_init(void)
{
	//使用内部HSI 16M 不分频
    CLK_HSICmd(ENABLE);										//开始内部高频RC
	CLK_SYSCLKConfig(CLK_PRESCALER_HSIDIV1);
	CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
}

void Key_GPIO_Init(void)
{
  	//Key1：PE0 输入上拉
	GPIO_Init(GPIOE,GPIO_PIN_0,GPIO_MODE_IN_PU_NO_IT);	
	//Key2：PD0 Key3：PD2 Key4：PD3 Key5：PD4输入上拉
	GPIO_Init(GPIOD,GPIO_PIN_0|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4,GPIO_MODE_IN_PU_NO_IT);
}

void LED_GPIO_Init(void)
{
  	//LED1:PA1,LED2:PA2,LED3:PA3,LED4:PA4 推挽高电平输出
  	GPIO_Init(GPIOA,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4,GPIO_MODE_OUT_PP_HIGH_FAST);
	//LED5:PE2,LED6:PE3 推挽高电平输出
  	GPIO_Init(GPIOE,GPIO_PIN_3,GPIO_MODE_OUT_PP_HIGH_FAST);
	
	GPIO_Init(GPIOD,GPIO_PIN_7,GPIO_MODE_OUT_PP_HIGH_FAST);
	//LED7:PG1,LED8:PG0 推挽高电平输出
  	GPIO_Init(GPIOG,GPIO_PIN_0|GPIO_PIN_1,GPIO_MODE_OUT_PP_HIGH_FAST);
	
	//LED_b~dp:PC1~7 推挽低电平输出
  	GPIO_Init(GPIOC,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7,GPIO_MODE_OUT_PP_LOW_FAST);	
	//LED_a:PE5 推挽低电平输出
  	GPIO_Init(GPIOE,GPIO_PIN_5,GPIO_MODE_OUT_PP_LOW_FAST);
	
	//W1:PE6 W2:PE7 推挽低电平输出
  	GPIO_Init(GPIOE,GPIO_PIN_6|GPIO_PIN_7,GPIO_MODE_OUT_PP_LOW_FAST);
	//W3:PB0 W4:PB1 推挽低电平输出
  	GPIO_Init(GPIOB,GPIO_PIN_0|GPIO_PIN_1,GPIO_MODE_OUT_PP_LOW_FAST);
	
}
//继电器GPIO初始化
void RelayCtrl_GPIO_Init(void)
{
	//PA5 PA6 推挽输出，初始低电平
  	GPIO_Init(GPIOA,GPIO_PIN_5|GPIO_PIN_6,GPIO_MODE_OUT_PP_LOW_FAST);	
}

void ADC1_GPIO_Init(void)
{
	//PB2:AIN2 PB3:AIN3 PB5:AIN5 PB6:AIN6
	GPIO_Init(GPIOB,GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_5|GPIO_PIN_6,GPIO_MODE_IN_FL_NO_IT); 	//AD模拟输入
}


void SelectADC_Channel(unsigned char channel)
{
	ADC1_Init(ADC1_CONVERSIONMODE_SINGLE,
			  channel,
			  ADC1_PRESSEL_FCPU_D2,
			  ADC1_EXTTRIG_TIM,
			  DISABLE,
			  ADC1_ALIGN_RIGHT,
			  ADC1_SCHMITTTRIG_CHANNEL0,
			  DISABLE);
}

void ADC_Config(void)
{
	ADC1_GPIO_Init();
	SelectADC_Channel(VrefChannel);
	ADC1_ClearITPendingBit(ADC1_IT_EOC);
	ADC1_ITConfig(ADC1_IT_EOCIE,ENABLE);	
	ADC1_StartConversion();
}

void TIM2_Config(void)
{
	TIM2_DeInit();
	TIM2_TimeBaseInit(TIM2_PRESCALER_16,1000);		//1ms
	TIM2_ClearFlag(TIM2_FLAG_UPDATE);
	TIM2_ITConfig(TIM2_IT_UPDATE,ENABLE);
	TIM2_Cmd(ENABLE);
}

void TIM3_Config(void)
{
	TIM3_DeInit();
	TIM3_TimeBaseInit(TIM3_PRESCALER_16,1000);		//1ms
	TIM3_ClearFlag(TIM3_FLAG_UPDATE);
	TIM3_ITConfig(TIM3_IT_UPDATE,ENABLE);
	TIM3_Cmd(ENABLE);
}

void TIM4_Config(void)
{
  /* TIM4 configuration:
   - TIM4CLK is set to 16 MHz, the TIM4 Prescaler is equal to 128 so the TIM1 counter
   clock used is 16 MHz / 128 = 125 000 Hz
  - With 125 000 Hz we can generate time base:
      max time base is 2.048 ms if TIM4_PERIOD = 255 --> (255 + 1) / 125000 = 2.048 ms
      min time base is 0.016 ms if TIM4_PERIOD = 1   --> (  1 + 1) / 125000 = 0.016 ms
  - In this example we need to generate a time base equal to 1 ms
   so TIM4_PERIOD = (0.001 * 125000 - 1) = 124 */

  /* Time base configuration */
  TIM4_TimeBaseInit(TIM4_PRESCALER_128, 80);
  /* Clear TIM4 update flag */
  TIM4_ClearFlag(TIM4_FLAG_UPDATE);
  /* Enable update interrupt */
  TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
  /* Enable TIM4 */
  TIM4_Cmd(ENABLE);
}

void ScanLED(unsigned char *datacode,unsigned char shanshuoflag,unsigned char databitcount,unsigned char uintbitcount)
{
	static unsigned char pos = 4;
	unsigned char temp = 0xFF;
	GPIO_WriteLow(GPIOE,GPIO_PIN_6);
	GPIO_WriteLow(GPIOE,GPIO_PIN_7);
	GPIO_WriteLow(GPIOB,GPIO_PIN_0);
	GPIO_WriteLow(GPIOB,GPIO_PIN_1);
	GPIO_WriteHigh(GPIOE,GPIO_PIN_5);
	GPIO_Write(GPIOC,0xFF);
	if(pos == 4)
	{
		if(uintbitcount==0)
		{
			if(shanshuoflag == 0)
			{
				temp = datacode[3];
			}
		}
		else
		{
			temp = datacode[3];
		}
	}
	else if(pos == 3)
	{
		if(uintbitcount>1)
		{
			temp = datacode[2];
		}
		else
		{
			if(shanshuoflag == 0)
			{
				temp = datacode[2];
			}
		}
	}
	else if(pos == 2)
	{
		if(uintbitcount>2)
		{
			temp = datacode[1];
		}
		else
		{
			if(shanshuoflag == 0)
			{
				temp = datacode[1];
			}	
		}		
	}
	else if(pos == 1)
	{
		if(shanshuoflag == 0)
		{
			temp = datacode[0];
		}
	}
	
	if(temp & 0x01)
		GPIO_WriteHigh(GPIOE,GPIO_PIN_5);
	else
		GPIO_WriteLow(GPIOE,GPIO_PIN_5);
	 
	GPIO_Write(GPIOC,temp & 0xFE);	
	
	if(pos == 1)
		GPIO_WriteHigh(GPIOB,GPIO_PIN_1);
	else if(pos == 2)
		GPIO_WriteHigh(GPIOB,GPIO_PIN_0);
	else if(pos == 3)
		GPIO_WriteHigh(GPIOE,GPIO_PIN_7);
	else if(pos == 4)
		GPIO_WriteHigh(GPIOE,GPIO_PIN_6);
	
	pos--;
	if((4-pos) == (databitcount+uintbitcount))
	{
		pos = 4;
	}
}

void TurnAllLedOff(void)
{
	memset(m_device_param_block.LedDispCode,0xFF,sizeof(m_device_param_block.LedDispCode));
	GPIO_WriteLow(GPIOE,GPIO_PIN_6);
	GPIO_WriteLow(GPIOE,GPIO_PIN_7);
	GPIO_WriteLow(GPIOB,GPIO_PIN_0);
	GPIO_WriteLow(GPIOB,GPIO_PIN_1);
	m_device_param_block.LedUpdateFlag = 1;
}

double GetADC_Vef(void)
{
	unsigned int vef_value = 0;
	//采样
	for(int i=0;i<10;i++)
	{
		ADC1_StartConversion();
		while(ADC1_GetFlagStatus(ADC1_FLAG_EOC) == 0);
		vef_value += ADC1_GetConversionValue();
		ADC1_ClearITPendingBit(ADC1_IT_EOC);
	}
	vef_value /= 10;
	return TL431_Value*1024/vef_value;
}

double Calc_ADC(unsigned int adcbuf[])
{
	double adcdoublevaule[SampleCnt];
	double volatgaevalue = 0.0;
	for(int i=1;i<SampleCnt;i++)
	{
		adcdoublevaule[i] = (adcbuf[i]*1.0/1024)*m_device_param_block.ADC_VEF;
		adcdoublevaule[i] = adcdoublevaule[i]*adcdoublevaule[i];
		volatgaevalue +=  adcdoublevaule[i];
	}
	volatgaevalue /= (SampleCnt-1);
	volatgaevalue = sqrt(volatgaevalue);
	return volatgaevalue;
}

unsigned char GetKeyValue(void)
{
	unsigned char key = 0;
	if(GPIO_ReadInputPin(KEY1_Port,KEY1_Pin) == 0)
		key |= 0x01;
	if(GPIO_ReadInputPin(KEY2_Port,KEY2_Pin) == 0)
		key |= 0x02;	
	if(GPIO_ReadInputPin(KEY3_Port,KEY3_Pin) == 0)
		key |= 0x04;
	if(GPIO_ReadInputPin(KEY4_Port,KEY4_Pin) == 0)
		key |= 0x08;
	if(GPIO_ReadInputPin(KEY5_Port,KEY5_Pin) == 0)
		key |= 0x10;
	return key;
}

//写EEPROM
unsigned char ProgramDeviceAddr(unsigned char *addrbuf,unsigned char bytenum)
{
	unsigned char rdatabuf[50];
	unsigned int wdataaddr;
	unsigned char ok=0;
	
	FLASH_DeInit();
	FLASH_Unlock(FLASH_MEMTYPE_DATA);					//先解锁
	KitWdt();
	wdataaddr = DataEEPROM_StartAddress;
	for(int i=0;i<bytenum;i++)
	{
		FLASH_ProgramByte(wdataaddr++,addrbuf[i]);  	//写数据
		FLASH_WaitForLastOperation(FLASH_MEMTYPE_DATA);
	}
	KitWdt();
	
	wdataaddr = DataEEPROM_StartAddress;
	for(int i=0;i<bytenum;i++)
	{
	  	rdatabuf[i] = FLASH_ReadByte(wdataaddr++);
		FLASH_WaitForLastOperation(FLASH_MEMTYPE_DATA);
		
	}
	if(Sum(rdatabuf,bytenum-1) == rdatabuf[bytenum-1])
	{
		ok = 1;
	}
	return ok;	
}

unsigned char Read_Device_Addr(unsigned char *addrbuf,unsigned char bytenum)
{
 
	unsigned int  rdataaddr;
	unsigned char rdatabuf[50];
	rdataaddr = DataEEPROM_StartAddress;
	for(int i=0;i<bytenum;i++)
	{
	  	rdatabuf[i] = FLASH_ReadByte(rdataaddr++);
	}
	if(Sum(rdatabuf,bytenum-1) == rdatabuf[bytenum-1])
	{
		memcpy(addrbuf,rdatabuf,bytenum);
		return 1;
	}
	else
		return 0;
}

void Relay_ON(void)
{
	Relay_Ctrl_2_ON();
	DelayMs(20);
	Relay_Ctrl_2_OFF();	
}

void Relay_OFF(void)
{
	Relay_Ctrl_1_ON();
	DelayMs(20);
	Relay_Ctrl_1_OFF();		
}

//IO口初始化及其他配置
void HardWare_Init(void)
{
	RelayCtrl_GPIO_Init();											//磁保持继电器控制IO初始化
	Relay_Ctrl_1_OFF();												//执行分闸动作
	Relay_Ctrl_2_OFF();
	Relay_OFF();
	GPIO_Init(GPIOB,GPIO_PIN_7,GPIO_MODE_OUT_PP_LOW_FAST);			//蜂鸣器控制IO初始化
	GPIO_Init(GPIOB,GPIO_PIN_4,GPIO_MODE_OUT_PP_LOW_FAST);			//“测试”按键控制漏电电流IO初始化
	Key_GPIO_Init();												//按键IO初始化
	LED_GPIO_Init();												//LED指示灯和数码管IO初始化
	
	ADC_Config();													//ADC采样配置
	TIM2_Config();													//定时器2  1ms定时器初始化	主要用来周期启动ADC采样
	TIM3_Config();													//定时器3  1ms定时器初始化	主要用来计数
	TIM4_Config();													//定时器4  1ms定时器初始化	主要用来扫描数码管
	
	Uart_Init();
}

void Wdt_Init(void)
{

  	IWDG_Enable();
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(IWDG_Prescaler_64);
	IWDG_SetReload(254);
	IWDG_ReloadCounter();

}

void KitWdt(void)
{
	IWDG_ReloadCounter();
}  


//串口初始化
void Uart_Init(void)
{
	//SYSCFG_REMAPPinConfig(REMAP_Pin_USART1TxRxPortA,ENABLE);		//端口重映射 USART1为PA2-TX、PA3-RX
	UART2_DeInit();
	GPIO_Init(GPIOD, GPIO_PIN_5, GPIO_MODE_OUT_OD_HIZ_FAST);
	GPIO_Init(GPIOD, GPIO_PIN_6, GPIO_MODE_IN_FL_NO_IT);
	CLK_PeripheralClockConfig (CLK_PERIPHERAL_UART2,ENABLE);		//开启USART时钟
	UART2_Init((unsigned long)9600,UART2_WORDLENGTH_8D,UART2_STOPBITS_1,UART2_PARITY_NO,UART2_SYNCMODE_CLOCK_DISABLE,UART2_MODE_TXRX_ENABLE);	//设置USART参数9600，8N1，接收发送
	UART2_ITConfig(UART2_IT_RXNE_OR,ENABLE);					//使能接收中断
	UART2_Cmd(ENABLE);									//使能USART  
	GPIO_Init(GPIOE,GPIO_PIN_2,GPIO_MODE_OUT_OD_LOW_FAST);
	GPIO_WriteLow(GPIOE,GPIO_PIN_2);
}

//串口发送数据
void Uart_Tx_Data(unsigned char bytebum,unsigned char *tdatabuf)
{
	unsigned char i;
	GPIO_WriteHigh(GPIOE,GPIO_PIN_2);
	for(i=0;i<bytebum;i++)
	{
	  	UART2_SendData8(tdatabuf[i]);															//发送1byte数据
		while(!UART2_GetFlagStatus(UART2_FLAG_TXE));											//等待发送完成
	}
	DelayMs(2);
	GPIO_WriteLow(GPIOE,GPIO_PIN_2);
}

