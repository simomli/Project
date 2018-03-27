#ifndef _SYS_DEFINE_
#define _SYS_DEFINE_

#include "stm8s.h"
#include "bsp.h"
#include "math.h"
#include "string.h"
#include "app.h"
#include "comm.h"

#define 	TL431_Value			2.496
#define 	SampleCnt			20
#define		Voltage_K			74.99
#define		Current_K			1.2
#define		LeakCurrent_K		0.84
#define		ShortPressTime		20			//20*100ms = 2s
#define		LongPressTime		50			//50*100ms = 5s

#define 	OverVoltgaeLimitMax		280
#define		OverVoltgaeLimitMin		220
#define		UnderVoltgaeLimitMax	220
#define		UnderVoltgaeLimitMin	120
#define		OverCurrentLimitMax		630
#define		OverCurrentLimitMin		10
#define		LeakCurrentLimitMax		90
#define		leakCurrentLimitMin		10
#define		RecloseCntMax			5
#define		RecloseCntMin			0
#define		Device_AddressMax		247
#define		Device_AddressMin		1

#define		OverCurrentDelayTime	4
#define 	OverVoltgaeDelayTime	4
#define 	DownVoltageDelayTime	4
#define		RecloseCntClearDelayTime	30



#define     Maxbytenum              50


#define    Version	1



#define		VoltageChannel			ADC1_CHANNEL_5
#define		LeakCurrentChannel		ADC1_CHANNEL_6
#define		CurrentChannel			ADC1_CHANNEL_3
#define		VrefChannel				ADC1_CHANNEL_2

//MOUBUS 寄存器地址
//实时电压 			0x0000
//实时电流 			0x0001
//实时漏电电流		0x0002
//跳闸次数			0x0003
//跳闸类型			0x0004
//报警总数          0x0005
//开关状态			0x0006

//远程分合闸        0x0010


enum
{
	OFF = 0,
	ON = 1
};

enum
{
	KEY_NONE 	= 0x00,		//无按键
	KEY_SET	 	= 0x01,		//设置/切换   按键
	KEY_UP	 	= 0x02,		//消音/上	  按键
	KEY_DOWN 	= 0x04,		//下		  按键
	KEY_CLEAR 	= 0x08,		//查询/清零	  按键
	KEY_TEST	= 0x10		//测试		  按键
};

enum
{
	Normal_Status = 0x00,					//显示电压状态			
	ShowCurrent_Status = 0x01,				//显示电流状态
	ShowLeakCurrent_Status = 0x02,			//显示漏电流状态
	ShowAllTroubleCnt_Status =0x03,			//显示总故障次数状态
	ShowOverVoltageCnt_Status = 0x04,		//显示过压故障次数状态
	ShowUnderVoltageCnt_Status = 0x05,		//显示欠压故障次数状态
	ShowOverCurrentCnt_Status = 0x06,		//显示过流故障次数状态
	ShowLeakCurrentCnt_Status = 0x07,		//显示漏电电流次数状态
	ShutDownLED_Status = 0x0F,				//关闭数码显示
	
	SetOverVoltageLimit_Status = 0x10,		//设置过压设定值状态
	SetUnderVoltgaeLimit_Status = 0x11,		//设置欠压设定值状态
	SetOverCurrentLimit_Status = 0x12,		//设置过流设定值状态
	SetOverLeakCurrentLimit_Status = 0x13,	//设置漏电电流设置值状态
	SetRecloseCntLimit_Status = 0x14,		//设置重合闸次数状态
	SetDevice_Adderss_Status = 0x15			//设置通信地址状态
};

enum
{
	RelayOFF_Status = 0x00,				//分闸状态
	RelayReadyON_Status = 0x01,			//准备合闸状态
	RelayON_Status = 0x02,				//合闸状态
	RelayWaitON_Status = 0x03,			//等待重合闸状态
	RelayLock_Status = 0x04,			//自锁状态
	RelayRemoteOFF_Status = 0x05		//远程分闸状态
};

enum 
{
	RemoteCtrl_IDEL_Status = 0x00,		//远程控制空闲
	RemoteCtrl_OFF_Status = 0x01,		//远程控制分闸
	RemoteCtrl_ON_Status = 0x02			//远程控制合闸
};

typedef struct 
{
	//unsigned int LedDispValue;			//数码管显示数值
	unsigned char LedDispCode[4];
	unsigned char LedDatabitcount;
	unsigned char Unitbitcount;
	unsigned char LedTwinkleFlag;			//数码管闪烁标识
	//char Uint;								//数值单位
	unsigned char Device_Status;			//设备状态
	unsigned char RelayCtrl_Status;		//继电器控制状态
	unsigned int KeyDelayNs;				//按键延时秒数
	unsigned char Key_Value;				//按键值
	unsigned char Key_Value_Old;			
	unsigned char Key_Relase_Flag;		//按键释放标志
	unsigned int  OverVoltageLimit;		//过压上限设定值
	unsigned int  UnderVoltageLimit;		//欠压下限设定值
	unsigned int OverCurrentLimit;		//过流上限设定值
	unsigned int LeakCurrentLimit;		//漏电电流设定值
	unsigned char RecloseCntLimit;		//重合闸次数最大值
	unsigned char Device_Address;			//通信地址
	float ADC_VEF;							//ADC的参考电压
	unsigned int Adcbuf[SampleCnt];		//采样缓存
	unsigned char ADCPeriodSampleOkFlag;	//周期采样完成标识位
	unsigned char SampleChannel;			//采样通道		0：电压  1：电流  2：漏电流
	float Voltage;							//实时电压
	float Current;							//实时电流
	float LeakCurrent;						//实时漏电电流
	unsigned char SilencerFlag;			//消音标志位
	
	unsigned int AllToubleCnt;			//总故障次数
	unsigned int OverVoltageCnt;			//过压故障次数
	unsigned int UnderVoltageCnt;			//欠压故障次数
	unsigned int OverCurrentCnt;			//过流故障次数
	unsigned int LeakCurrentCnt;			//漏电故障次数
	unsigned char RecloseCnt;				//重合闸次数
	unsigned int CloseCnt;				//跳闸次数
	
	unsigned char Key_IDEL_Delay;			//按键空闲时间  秒为单位
	unsigned int Run_Timer;				//计时器  秒为单位
	unsigned int OverCurrent_DelayTime;	//过流计时器	
	unsigned char Touble_Flag;			//故障标识位 bit0:过压 bit1:欠压，bit2：过流，bit3：漏电
	unsigned char Touble_FlagOld;
	unsigned char Touble_Reason;			//故障原因
	unsigned char BeerONFlag;				//蜂鸣器延时
	unsigned char LedUpdateFlag;			//数码管显示更新标识位
	
	unsigned char ComRxBuf[Maxbytenum];	//串口缓存
	unsigned char ComTxBuf[Maxbytenum];	//串口缓存
	//unsigned char Buf_Head;				//串口缓存头指针
	//unsigned char	Buf_Hail;				//串口缓存尾指针
	unsigned char ComRevTimeout;
	unsigned char Lenth;					//数据长度
	unsigned int Reg_Address;				//寄存器首地址
	unsigned int Reg_Len;					//寄存器长度
	unsigned char FunCode;				//modbus通信功能码
	unsigned int SingleRegisterdata;		//单寄存器值
	unsigned char LeakCurrentTestFlag;	//漏电电流测试标识位
	unsigned char RemoteCtrl_Status;		//远程控制分合闸状态
}Device_Param_Block;


typedef struct
{
	unsigned int OverVoltageLimit;		//过压上限设定值
	unsigned int UnderVoltageLimit;		//欠压下限设定值
	unsigned int OverCurrentLimit;		//过流上限设定值
	unsigned int LeakCurrentLimit;		//漏电电流设定值
	unsigned char RecloseCntLimit;		//重合闸次数最大值
	unsigned char Device_Address;			//通信地址
	
	unsigned int AllToubleCnt;			//总故障次数
	unsigned int OverVoltageCnt;			//过压故障次数
	unsigned int UnderVoltageCnt;			//欠压故障次数
	unsigned int OverCurrentCnt;			//过流故障次数
	unsigned int LeakCurrentCnt;			//漏电故障次数
	unsigned int CloseCnt;				//跳闸次数
	unsigned char Touble_Reason;			//故障原因
	unsigned char CRC;					//RCR校验和
}PowerOff_Save_Param_Block;

extern Device_Param_Block m_device_param_block;
extern PowerOff_Save_Param_Block m_poweroff_save_param_block;

void DelayMs(unsigned char delay);
void SwitchAllLED(unsigned char status);
#endif