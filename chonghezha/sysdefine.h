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

//MOUBUS �Ĵ�����ַ
//ʵʱ��ѹ 			0x0000
//ʵʱ���� 			0x0001
//ʵʱ©�����		0x0002
//��բ����			0x0003
//��բ����			0x0004
//��������          0x0005
//����״̬			0x0006

//Զ�̷ֺ�բ        0x0010


enum
{
	OFF = 0,
	ON = 1
};

enum
{
	KEY_NONE 	= 0x00,		//�ް���
	KEY_SET	 	= 0x01,		//����/�л�   ����
	KEY_UP	 	= 0x02,		//����/��	  ����
	KEY_DOWN 	= 0x04,		//��		  ����
	KEY_CLEAR 	= 0x08,		//��ѯ/����	  ����
	KEY_TEST	= 0x10		//����		  ����
};

enum
{
	Normal_Status = 0x00,					//��ʾ��ѹ״̬			
	ShowCurrent_Status = 0x01,				//��ʾ����״̬
	ShowLeakCurrent_Status = 0x02,			//��ʾ©����״̬
	ShowAllTroubleCnt_Status =0x03,			//��ʾ�ܹ��ϴ���״̬
	ShowOverVoltageCnt_Status = 0x04,		//��ʾ��ѹ���ϴ���״̬
	ShowUnderVoltageCnt_Status = 0x05,		//��ʾǷѹ���ϴ���״̬
	ShowOverCurrentCnt_Status = 0x06,		//��ʾ�������ϴ���״̬
	ShowLeakCurrentCnt_Status = 0x07,		//��ʾ©���������״̬
	ShutDownLED_Status = 0x0F,				//�ر�������ʾ
	
	SetOverVoltageLimit_Status = 0x10,		//���ù�ѹ�趨ֵ״̬
	SetUnderVoltgaeLimit_Status = 0x11,		//����Ƿѹ�趨ֵ״̬
	SetOverCurrentLimit_Status = 0x12,		//���ù����趨ֵ״̬
	SetOverLeakCurrentLimit_Status = 0x13,	//����©���������ֵ״̬
	SetRecloseCntLimit_Status = 0x14,		//�����غ�բ����״̬
	SetDevice_Adderss_Status = 0x15			//����ͨ�ŵ�ַ״̬
};

enum
{
	RelayOFF_Status = 0x00,				//��բ״̬
	RelayReadyON_Status = 0x01,			//׼����բ״̬
	RelayON_Status = 0x02,				//��բ״̬
	RelayWaitON_Status = 0x03,			//�ȴ��غ�բ״̬
	RelayLock_Status = 0x04,			//����״̬
	RelayRemoteOFF_Status = 0x05		//Զ�̷�բ״̬
};

enum 
{
	RemoteCtrl_IDEL_Status = 0x00,		//Զ�̿��ƿ���
	RemoteCtrl_OFF_Status = 0x01,		//Զ�̿��Ʒ�բ
	RemoteCtrl_ON_Status = 0x02			//Զ�̿��ƺ�բ
};

typedef struct 
{
	//unsigned int LedDispValue;			//�������ʾ��ֵ
	unsigned char LedDispCode[4];
	unsigned char LedDatabitcount;
	unsigned char Unitbitcount;
	unsigned char LedTwinkleFlag;			//�������˸��ʶ
	//char Uint;								//��ֵ��λ
	unsigned char Device_Status;			//�豸״̬
	unsigned char RelayCtrl_Status;		//�̵�������״̬
	unsigned int KeyDelayNs;				//������ʱ����
	unsigned char Key_Value;				//����ֵ
	unsigned char Key_Value_Old;			
	unsigned char Key_Relase_Flag;		//�����ͷű�־
	unsigned int  OverVoltageLimit;		//��ѹ�����趨ֵ
	unsigned int  UnderVoltageLimit;		//Ƿѹ�����趨ֵ
	unsigned int OverCurrentLimit;		//���������趨ֵ
	unsigned int LeakCurrentLimit;		//©������趨ֵ
	unsigned char RecloseCntLimit;		//�غ�բ�������ֵ
	unsigned char Device_Address;			//ͨ�ŵ�ַ
	float ADC_VEF;							//ADC�Ĳο���ѹ
	unsigned int Adcbuf[SampleCnt];		//��������
	unsigned char ADCPeriodSampleOkFlag;	//���ڲ�����ɱ�ʶλ
	unsigned char SampleChannel;			//����ͨ��		0����ѹ  1������  2��©����
	float Voltage;							//ʵʱ��ѹ
	float Current;							//ʵʱ����
	float LeakCurrent;						//ʵʱ©�����
	unsigned char SilencerFlag;			//������־λ
	
	unsigned int AllToubleCnt;			//�ܹ��ϴ���
	unsigned int OverVoltageCnt;			//��ѹ���ϴ���
	unsigned int UnderVoltageCnt;			//Ƿѹ���ϴ���
	unsigned int OverCurrentCnt;			//�������ϴ���
	unsigned int LeakCurrentCnt;			//©����ϴ���
	unsigned char RecloseCnt;				//�غ�բ����
	unsigned int CloseCnt;				//��բ����
	
	unsigned char Key_IDEL_Delay;			//��������ʱ��  ��Ϊ��λ
	unsigned int Run_Timer;				//��ʱ��  ��Ϊ��λ
	unsigned int OverCurrent_DelayTime;	//������ʱ��	
	unsigned char Touble_Flag;			//���ϱ�ʶλ bit0:��ѹ bit1:Ƿѹ��bit2��������bit3��©��
	unsigned char Touble_FlagOld;
	unsigned char Touble_Reason;			//����ԭ��
	unsigned char BeerONFlag;				//��������ʱ
	unsigned char LedUpdateFlag;			//�������ʾ���±�ʶλ
	
	unsigned char ComRxBuf[Maxbytenum];	//���ڻ���
	unsigned char ComTxBuf[Maxbytenum];	//���ڻ���
	//unsigned char Buf_Head;				//���ڻ���ͷָ��
	//unsigned char	Buf_Hail;				//���ڻ���βָ��
	unsigned char ComRevTimeout;
	unsigned char Lenth;					//���ݳ���
	unsigned int Reg_Address;				//�Ĵ����׵�ַ
	unsigned int Reg_Len;					//�Ĵ�������
	unsigned char FunCode;				//modbusͨ�Ź�����
	unsigned int SingleRegisterdata;		//���Ĵ���ֵ
	unsigned char LeakCurrentTestFlag;	//©��������Ա�ʶλ
	unsigned char RemoteCtrl_Status;		//Զ�̿��Ʒֺ�բ״̬
}Device_Param_Block;


typedef struct
{
	unsigned int OverVoltageLimit;		//��ѹ�����趨ֵ
	unsigned int UnderVoltageLimit;		//Ƿѹ�����趨ֵ
	unsigned int OverCurrentLimit;		//���������趨ֵ
	unsigned int LeakCurrentLimit;		//©������趨ֵ
	unsigned char RecloseCntLimit;		//�غ�բ�������ֵ
	unsigned char Device_Address;			//ͨ�ŵ�ַ
	
	unsigned int AllToubleCnt;			//�ܹ��ϴ���
	unsigned int OverVoltageCnt;			//��ѹ���ϴ���
	unsigned int UnderVoltageCnt;			//Ƿѹ���ϴ���
	unsigned int OverCurrentCnt;			//�������ϴ���
	unsigned int LeakCurrentCnt;			//©����ϴ���
	unsigned int CloseCnt;				//��բ����
	unsigned char Touble_Reason;			//����ԭ��
	unsigned char CRC;					//RCRУ���
}PowerOff_Save_Param_Block;

extern Device_Param_Block m_device_param_block;
extern PowerOff_Save_Param_Block m_poweroff_save_param_block;

void DelayMs(unsigned char delay);
void SwitchAllLED(unsigned char status);
#endif