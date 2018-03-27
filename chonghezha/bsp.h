#ifndef _BSP_H_
#define _BSP_H_
#include "stm8s.h"
#include "sysdefine.h"

#define	BlockNum 0
#define DataEEPROM_StartAddress	0x4000



#define 	OverVoltage_Pin			GPIO_PIN_1		//过压
#define 	OverVoltage_Port 		GPIOG

#define 	UnderVoltage_Pin		GPIO_PIN_0		//欠压
#define 	UnderVoltage_Port 		GPIOG

#define 	OverCurrent_Pin			GPIO_PIN_1		//过流
#define 	OverCurrent_Port 		GPIOA

#define 	LeakCurrent_Pin			GPIO_PIN_2		//漏电
#define 	LeakCurrent_Port 		GPIOA

#define 	Trouble_Pin				GPIO_PIN_3		//故障
#define 	Trouble_Port 			GPIOA

#define 	Silencer_Pin			GPIO_PIN_4		//消音
#define 	Silencer_Port 			GPIOA

//#if Version
#define 	RunLED_Pin				GPIO_PIN_7		//运行
#define 	RunLED_Port 			GPIOD

//#else

//#define 	RunLED_Pin				GPIO_PIN_2		//运行
//#define 	RunLED_Port 			GPIOE

//#endif

#define 	Communication_Pin		GPIO_PIN_3		//通讯
#define 	Communication_Port 		GPIOE

#define 	Relay_Ctrl_1_Pin		GPIO_PIN_5		//磁保持继电器1
#define 	Relay_Ctrl_1_Port 		GPIOA

#define 	Relay_Ctrl_2_Pin		GPIO_PIN_6		//磁保持继电器1
#define 	Relay_Ctrl_2_Port 		GPIOA

#define 	Beer_Pin				GPIO_PIN_7		//蜂鸣器
#define		BeerPort				GPIOB

#define		LeakCurrent_Ctrl_Pin	GPIO_PIN_4		//漏电测试控制
#define		LeakCurrent_Ctrl_Port	GPIOB


#define		KEY1_Pin				GPIO_PIN_0		//KEY1
#define		KEY1_Port 				GPIOE

#define		KEY2_Pin				GPIO_PIN_0		//KEY2
#define		KEY2_Port 				GPIOD

#define		KEY3_Pin				GPIO_PIN_2		//KEY3
#define		KEY3_Port 				GPIOD

#define		KEY4_Pin				GPIO_PIN_3		//KEY4
#define		KEY4_Port 				GPIOD

#define		KEY5_Pin				GPIO_PIN_4		//KEY5
#define		KEY5_Port 				GPIOD


#define		Beer_ON()					GPIO_WriteHigh(BeerPort,Beer_Pin)
#define		Beer_OFF()					GPIO_WriteLow(BeerPort,Beer_Pin)
#define		Beer_Toggle()				GPIO_WriteReverse(BeerPort,Beer_Pin)


#define		OverVoltage_LED_ON()		GPIO_WriteLow(OverVoltage_Port,OverVoltage_Pin)
#define		OverVoltage_LED_OFF()		GPIO_WriteHigh(OverVoltage_Port,OverVoltage_Pin)
#define		OverVoltage_LED_Toggle()	GPIO_WriteReverse(OverVoltage_Port,OverVoltage_Pin)

#define		UnderVoltage_LED_ON()		GPIO_WriteLow(UnderVoltage_Port,UnderVoltage_Pin)
#define		UnderVoltage_LED_OFF()		GPIO_WriteHigh(UnderVoltage_Port,UnderVoltage_Pin)
#define		UnderVoltage_LED_Toggle()	GPIO_WriteReverse(UnderVoltage_Port,UnderVoltage_Pin)

#define		OverCurrent_LED_ON()		GPIO_WriteLow(OverCurrent_Port,OverCurrent_Pin)
#define		OverCurrent_LED_OFF()		GPIO_WriteHigh(OverCurrent_Port,OverCurrent_Pin)
#define		OverCurrent_LED_Toggle()	GPIO_WriteReverse(OverCurrent_Port,OverCurrent_Pin)

#define		LeakCurrent_LED_ON()		GPIO_WriteLow(LeakCurrent_Port,LeakCurrent_Pin)
#define		LeakCurrent_LED_OFF()		GPIO_WriteHigh(LeakCurrent_Port,LeakCurrent_Pin)
#define		LeakCurrent_LED_Toggle()	GPIO_WriteReverse(LeakCurrent_Port,LeakCurrent_Pin)

#define		Trouble_LED_ON()			GPIO_WriteLow(Trouble_Port,Trouble_Pin)
#define		Trouble_LED_OFF()			GPIO_WriteHigh(Trouble_Port,Trouble_Pin)
#define		Trouble_LED_Toggle()		GPIO_WriteReverse(Trouble_Port,Trouble_Pin)

#define		Silencer_LED_ON()			GPIO_WriteLow(Silencer_Port,Silencer_Pin)
#define		Silencer_LED_OFF()			GPIO_WriteHigh(Silencer_Port,Silencer_Pin)
#define		Silencer_LED_Toggle()		GPIO_WriteReverse(Silencer_Port,Silencer_Pin)

#define		RunLED_ON()					GPIO_WriteLow(RunLED_Port,RunLED_Pin)
#define		RunLED_OFF()				GPIO_WriteHigh(RunLED_Port,RunLED_Pin)
#define		RunLED_Toggle()				GPIO_WriteReverse(RunLED_Port,RunLED_Pin)

#define		Communication_ON()			GPIO_WriteLow(Communication_Port,Communication_Pin)
#define		Communication_OFF()			GPIO_WriteHigh(Communication_Port,Communication_Pin)
#define		Communication_Toggle()		GPIO_WriteReverse(Communication_Port,Communication_Pin)

#define		Relay_Ctrl_1_ON()			GPIO_WriteHigh(Relay_Ctrl_1_Port,Relay_Ctrl_1_Pin)
#define		Relay_Ctrl_1_OFF()			GPIO_WriteLow(Relay_Ctrl_1_Port,Relay_Ctrl_1_Pin)

#define		Relay_Ctrl_2_ON()			GPIO_WriteHigh(Relay_Ctrl_2_Port,Relay_Ctrl_2_Pin)
#define		Relay_Ctrl_2_OFF()			GPIO_WriteLow(Relay_Ctrl_2_Port,Relay_Ctrl_2_Pin)

#define		LeakCurrent_Ctrl_ON()		GPIO_WriteHigh(LeakCurrent_Ctrl_Port,LeakCurrent_Ctrl_Pin)
#define		LeakCurrent_Ctrl_OFF()		GPIO_WriteLow(LeakCurrent_Ctrl_Port,LeakCurrent_Ctrl_Pin)




void Sysclock_init(void);
void Key_GPIO_Init(void);
void LED_GPIO_Init(void);
void ScanLED(unsigned char *datacode,unsigned char shanshuoflag,unsigned char databitcount,unsigned char uintbitcount);
void TIM4_Config(void);
void TIM2_Config(void);
void TIM3_Config(void);
void SelectADC_Channel(unsigned char channel);
void ADC1_GPIO_Init(void);
void ADC_Config(void);
double GetADC_Vef(void);
double Calc_ADC(unsigned int adcbuf[]);
unsigned char GetKeyValue(void);
unsigned char ProgramDeviceAddr(unsigned char *addrbuf,unsigned char bytenum);
unsigned char Read_Device_Addr(unsigned char *addrbuf,unsigned char bytenum);
void Wdt_Init(void);
void KitWdt(void);
void Uart_Init(void);
void Uart_Tx_Data(unsigned char bytebum,unsigned char *tdatabuf);

void HardWare_Init(void);


#endif