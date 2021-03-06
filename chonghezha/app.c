#include "sysdefine.h"
										//30秒，5分钟，10分钟，30分钟，1小时
const unsigned int RecloseDelayNs[5] = {30,300,600,1800,3600};
//const unsigned int RecloseDelayNs[5] = {5,10,15,1800,3600};

//0~9共阳数码管段码表 0xC1:U  0xAB:m 0x88:A 0xBF:- 0xAB:n 0x87:T 0xA1:d
const unsigned char LedCode[] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xC1,0xAB,0x88,0xBF,0xAB,0x87,0xA1};

unsigned char Sum(unsigned char *crcbuf,unsigned int bytelen)
{
	unsigned int i;
	unsigned char sum=0;
	for(i=0;i<bytelen;i++)
	{
		sum += crcbuf[i];
	}
	return sum;
}


//按键处理
void DealKey(void)
{
	static unsigned int KeyDelayNs=0;
	static unsigned char LongPressRelaseFlag = 1;
	static unsigned char Clear_flag = 0;
	static unsigned char key_flag=0;
	static unsigned char key_temp_old=0;
	static unsigned char setbackflag=0;
	unsigned char key_temp;
	
	//在数码管非黑屏且继电器非准备重合闸且继电器非自锁状态下，才查询按键
	if((m_device_param_block.Device_Status != ShutDownLED_Status)     && 
	   (m_device_param_block.RelayCtrl_Status != RelayReadyON_Status) && 
	   (m_device_param_block.RelayCtrl_Status != RelayLock_Status))
	{
		key_temp = GetKeyValue();				//读取按键值
		if(key_temp != KEY_NONE)				//查询到有按键按下
		{	
			if(key_flag == 0)					//按键未消抖
			{
				key_flag = 1;					//消抖标志位置1
				DelayMs(10);					//延时10ms
				key_temp_old = key_temp;		//保存按键值
			}
			else if(key_flag ==1)				//已经消抖
			{
				if(key_temp == key_temp_old)	//此次按键值和消抖前按键值一样，标识同一个按键值有效
				{
					m_device_param_block.Key_Value = key_temp;										//保存按键值
					if(m_device_param_block.Key_Value_Old != m_device_param_block.Key_Value)		//这次按键值和上一次按键值不一样，则表示有新的按键查询到
					{
						m_device_param_block.Key_Value_Old = m_device_param_block.Key_Value;		//上一次的按键值更新为这次的按键值，主要为了实现按下一个键，蜂鸣器响一下
						if(m_device_param_block.SilencerFlag == 0)									//不消音，则蜂鸣器响，同时置蜂鸣器标识位，为了实现响一下再关掉
						{
							Beer_ON();
							m_device_param_block.BeerONFlag = 1;
						}
					}
					m_device_param_block.Key_Relase_Flag = 0;										//有按键按下，按键松开标志位清零
				}
			}
		}
		//没有查询到按键
		else
		{
			m_device_param_block.Key_Relase_Flag = 1;												//按键松开标志位置1
			key_flag = 0;																			//消抖标志清零
			m_device_param_block.Key_Value_Old = KEY_NONE;											//老按键值清零											
		}		
	}
	else	//黑屏状态下，按键值都归为无按键
	{
		m_device_param_block.Key_Value = KEY_NONE;
	}

	//测试漏电电流打开，2s后关闭
	if(m_device_param_block.LeakCurrentTestFlag == 1)
	{
		if(m_device_param_block.Run_Timer >= 2)													//延时2s，则关闭漏电电流产生
		{
			m_device_param_block.LeakCurrentTestFlag = 0;										//漏电电流测试标识位清零
			m_device_param_block.Run_Timer = 0;													//延时器清零
			LeakCurrent_Ctrl_OFF();																//关闭漏电电路														
		}
	}
	//有按键按下
	if(m_device_param_block.Key_Value != KEY_NONE)
	{
		m_device_param_block.Key_IDEL_Delay = 0;												//按键无操作延时器清零
		//继电器非自锁模式
		if(m_device_param_block.RelayCtrl_Status != RelayLock_Status)
		{
			//在查看模式
			if((m_device_param_block.Device_Status & 0x10) == 0x00)						
			{
				//在任何查看模式下，按下设置键
				if(m_device_param_block.Key_Value == KEY_SET)							
				{	
					//长按且长按松开标志位为1，则进入默认设置过压设定值模式
					//从查看模式转到设置过压上限参数模式，则需要跳闸，运行灯常亮，过压指示灯常亮，其他三个指示灯熄灭
					if((m_device_param_block.KeyDelayNs>=LongPressTime) && 
					   (LongPressRelaseFlag == 1))												
					{
						m_device_param_block.Device_Status = SetOverVoltageLimit_Status;			//状态更改为设置过压上限模式
						m_device_param_block.Touble_Flag = 0;
						m_device_param_block.RelayCtrl_Status = RelayOFF_Status;
						m_device_param_block.Run_Timer = 0;
						TurnAllLedOff();															//清数码管显示
						LongPressRelaseFlag = 0;													//长按松开标识位清零
						OverVoltage_LED_ON();														//点亮过压LED指示灯，其他三个指示灯熄灭
						UnderVoltage_LED_OFF();
						OverCurrent_LED_OFF();
						LeakCurrent_LED_OFF();
						Trouble_LED_OFF();
						Relay_OFF();																//执行跳闸动作
						RunLED_ON();																//运行灯常亮
					}
					
					//短按SET键
					if((m_device_param_block.KeyDelayNs<=ShortPressTime) && 
					   (m_device_param_block.Key_Relase_Flag == 1))	
					{
						TurnAllLedOff();
						//当前显示电压模式则切换到显示电流模式
						if(m_device_param_block.Device_Status == Normal_Status)	
						{
							m_device_param_block.Device_Status = ShowCurrent_Status;							
						}
						////当前显示电流模式则切换到显示漏电电流模式
						else if(m_device_param_block.Device_Status == ShowCurrent_Status)
						{
							m_device_param_block.Device_Status = ShowLeakCurrent_Status;
						}
						//当前显示漏电电流模式则切换到显示电压模式
						else if(m_device_param_block.Device_Status == ShowLeakCurrent_Status)
						{
							m_device_param_block.Device_Status = Normal_Status;	
						}
						else if((m_device_param_block.Device_Status == ShowAllTroubleCnt_Status) 	||
								 (m_device_param_block.Device_Status == ShowOverVoltageCnt_Status) 	||
								 (m_device_param_block.Device_Status == ShowUnderVoltageCnt_Status) ||
								 (m_device_param_block.Device_Status == ShowOverCurrentCnt_Status) 	||
								 (m_device_param_block.Device_Status == ShowLeakCurrentCnt_Status))
						{
							m_device_param_block.Device_Status = Normal_Status;	
						}
					}					
				}
				
				//在任何查看模式下，消音键按下，未消音的则消音，已消音的，恢复
				if(m_device_param_block.Key_Value == KEY_UP)
				{
					//短按
					if((m_device_param_block.KeyDelayNs<=ShortPressTime) && 
					   (m_device_param_block.Key_Relase_Flag == 1))	
					{
						//如果已经消音，则打开，对应的LED指示灯熄灭
						if(m_device_param_block.SilencerFlag == 1)						
						{
							m_device_param_block.SilencerFlag = 0;
							Silencer_LED_OFF();
						}
						//如果未消音，则消音，对应的LED指示灯点亮
						else if(m_device_param_block.SilencerFlag == 0)					
						{
							m_device_param_block.SilencerFlag = 1;
							Silencer_LED_ON();	
							Beer_OFF();
						}
					}
				}
				//在任何查看模式下，清除/查询键按下 短按则在过压故障，欠压故障，过流故障，漏电故障切换显示故障次数，长按，则清除故障记录
				
				if(m_device_param_block.Key_Value == KEY_CLEAR)
				{
					//短按，则进入查询状态
					if((m_device_param_block.KeyDelayNs<=ShortPressTime) && 
					   (m_device_param_block.Key_Relase_Flag == 1))	
					{
						//关闭所有故障对应的指示灯
						OverVoltage_LED_OFF();
						UnderVoltage_LED_OFF();
						OverCurrent_LED_OFF();
						LeakCurrent_LED_OFF();
						//当前在显示总故障次数模式,则进入显示过压故障次数
						if(m_device_param_block.Device_Status == ShowAllTroubleCnt_Status)					
						{
							m_device_param_block.Device_Status = ShowOverVoltageCnt_Status;
							TurnAllLedOff();
							OverVoltage_LED_ON();
						}
						//当前在显示过压故障次数，则进入欠压故障次数
						else if(m_device_param_block.Device_Status == ShowOverVoltageCnt_Status)				
						{
							m_device_param_block.Device_Status = ShowUnderVoltageCnt_Status;
							TurnAllLedOff();
							UnderVoltage_LED_ON();						
						}
						//当前在显示欠压故障次数，则进入过流故障次数
						else if(m_device_param_block.Device_Status == ShowUnderVoltageCnt_Status)				
						{
							m_device_param_block.Device_Status = ShowOverCurrentCnt_Status;
							TurnAllLedOff();
							OverCurrent_LED_ON();							
						}
						//当前在显示过流故障次数，则进入漏电电流故障次数
						else if(m_device_param_block.Device_Status == ShowOverCurrentCnt_Status)				
						{
							m_device_param_block.Device_Status = ShowLeakCurrentCnt_Status;
							TurnAllLedOff();
							LeakCurrent_LED_ON();
						}
						else
						{
							m_device_param_block.Device_Status = ShowAllTroubleCnt_Status;
							TurnAllLedOff();
							OverVoltage_LED_ON();
							UnderVoltage_LED_ON();
							OverCurrent_LED_ON();
							LeakCurrent_LED_ON();						
						}
					}
					//长按5s，则清所有故障次数
					if(m_device_param_block.KeyDelayNs>=LongPressTime)				
					{
						if(Clear_flag == 0)
						{
							m_device_param_block.LeakCurrentCnt = 0;
							m_device_param_block.OverCurrentCnt = 0;
							m_device_param_block.OverVoltageCnt = 0;
							m_device_param_block.UnderVoltageCnt = 0;
							m_device_param_block.AllToubleCnt = 0;
							Beer_ON();												//响蜂鸣器
							m_device_param_block.BeerONFlag = 1;
							SaveParam();											//保存参数
							Clear_flag = 1;						
						}
					}
				}
				//在任何查看模式下，测试漏电按键按下
				if(m_device_param_block.Key_Value == KEY_TEST)
				{
					//短按
					if((m_device_param_block.KeyDelayNs<=ShortPressTime) && 
					   (m_device_param_block.Key_Relase_Flag == 1))	
					{
						LeakCurrent_Ctrl_ON();									//打开漏电电路
						m_device_param_block.LeakCurrentTestFlag = 1;			//测试漏电电流标识位置1
						m_device_param_block.Run_Timer = 0;						//延时器清零
					}						
				}
			}

			//在设置模式下，短按设置键，模式在过压，欠压，过流，漏电，合闸次数上限值及通信地址切换
			//长按设置键，则回到正常模式，准备开始重合闸
			//设置模式下，只有设置键，上键，下键起作用，短按上下键，分别加1减1，长按则连续加减
			else if((m_device_param_block.Device_Status &0x10) == 0x10)							
			{
				//设置按键
				if(m_device_param_block.Key_Value == KEY_SET)
				{
					//长按设置键，进入正常模式并保存参数
					if((m_device_param_block.KeyDelayNs>=LongPressTime) && (LongPressRelaseFlag == 1))
					{
						m_device_param_block.Device_Status = Normal_Status;			//返回到正常模式
						setbackflag = 1;
						
						m_device_param_block.Touble_Flag = 0;
						TurnAllLedOff();											//清数码管显示
						LongPressRelaseFlag = 0;
						OverVoltage_LED_OFF();										//熄灭所有LED灯
						UnderVoltage_LED_OFF();
						OverCurrent_LED_OFF();
						LeakCurrent_LED_OFF();
						m_device_param_block.Run_Timer = 0;							//延时器清零
						SaveParam();												//保存参数
					}
				}
				
				//设置过压设定值模式,短按设置键，则切换到设置欠压设定值模式，短按上下键，则参数加减，长按，则连续加减
				if(m_device_param_block.Device_Status == SetOverVoltageLimit_Status)			
				{
					//切换/设置按键按下
					if(m_device_param_block.Key_Value == KEY_SET)
					{
						//短按,则进入设置欠压设定值模式
						if((m_device_param_block.KeyDelayNs<=ShortPressTime) && 
						   (m_device_param_block.Key_Relase_Flag == 1))	
						{
							m_device_param_block.Device_Status = SetUnderVoltgaeLimit_Status;	//设置欠压设定值模式
							TurnAllLedOff();													//清数码管显示
							UnderVoltage_LED_ON();												//欠压指示灯常亮
							OverVoltage_LED_OFF();												//过压指示灯熄灭
						}
					}
					//UP键
					else if(m_device_param_block.Key_Value == KEY_UP)							
					{
						//过压设定值小于过压最大值
						if(m_device_param_block.OverVoltageLimit<OverVoltgaeLimitMax)
						{
							//短按，则设定值加1
							if((m_device_param_block.KeyDelayNs<=ShortPressTime) && 
							   (m_device_param_block.Key_Relase_Flag == 1))
							{
								m_device_param_block.OverVoltageLimit++; 
							}
							//长按，在连续加
							else if((m_device_param_block.KeyDelayNs-KeyDelayNs)/2)
							{
								m_device_param_block.OverVoltageLimit ++; 
								KeyDelayNs = m_device_param_block.KeyDelayNs;
							}	
						}	
					}
					//down键
					else if(m_device_param_block.Key_Value == KEY_DOWN)						
					{
						//过压设定值大于过压最小值
						if(m_device_param_block.OverVoltageLimit>OverVoltgaeLimitMin)
						{
							//短按，则设定值减1
							if((m_device_param_block.KeyDelayNs<=ShortPressTime) && 
							   (m_device_param_block.Key_Relase_Flag == 1))
							{
								m_device_param_block.OverVoltageLimit--; 
							}
							//长按，则设定值连续减1
							else if((m_device_param_block.KeyDelayNs-KeyDelayNs)/2)
							{
								m_device_param_block.OverVoltageLimit -- ; 
								KeyDelayNs = m_device_param_block.KeyDelayNs;
							}	
						}					
					}
				}
				//设置欠压设定值模式，操作同上
				else if(m_device_param_block.Device_Status == SetUnderVoltgaeLimit_Status)			
				{
					if(m_device_param_block.Key_Value == KEY_SET)
					{
						//短按,则进入设置过流设定值模式
						if((m_device_param_block.KeyDelayNs<=ShortPressTime) && 
						   (m_device_param_block.Key_Relase_Flag == 1))	
						{
							m_device_param_block.Device_Status = SetOverCurrentLimit_Status;
							TurnAllLedOff();
							OverCurrent_LED_ON();
							UnderVoltage_LED_OFF();
						}
					}
					else if(m_device_param_block.Key_Value == KEY_UP)
					{
						if(m_device_param_block.UnderVoltageLimit<UnderVoltgaeLimitMax)
						{
							if((m_device_param_block.KeyDelayNs<=ShortPressTime) && 
							   (m_device_param_block.Key_Relase_Flag == 1))
							{
								m_device_param_block.UnderVoltageLimit++; 
							}
							else if((m_device_param_block.KeyDelayNs-KeyDelayNs)/2)
							{
								m_device_param_block.UnderVoltageLimit ++;
								KeyDelayNs = m_device_param_block.KeyDelayNs;
							}	
						}	
					}
					else if(m_device_param_block.Key_Value == KEY_DOWN)
					{
						if(m_device_param_block.UnderVoltageLimit>UnderVoltgaeLimitMin)
						{
							if((m_device_param_block.KeyDelayNs<=ShortPressTime) && 
							   (m_device_param_block.Key_Relase_Flag == 1))
							{
								m_device_param_block.UnderVoltageLimit--; 
							}
							else if((m_device_param_block.KeyDelayNs-KeyDelayNs)/2)
							{
									m_device_param_block.UnderVoltageLimit --; 
									KeyDelayNs = m_device_param_block.KeyDelayNs;
							}	
						}					
					}
				}
				//设置过流设定值模式，操作同上
				else if(m_device_param_block.Device_Status == SetOverCurrentLimit_Status)			
				{
					if(m_device_param_block.Key_Value == KEY_SET)
					{
						//短按,则进入设置漏电电流设定值模式
						if((m_device_param_block.KeyDelayNs<=ShortPressTime) && 
						   (m_device_param_block.Key_Relase_Flag == 1))	
						{
							m_device_param_block.Device_Status = SetOverLeakCurrentLimit_Status;
							TurnAllLedOff();
							LeakCurrent_LED_ON();
							OverCurrent_LED_OFF();
						}
					}
					else if(m_device_param_block.Key_Value == KEY_UP)
					{
						if(m_device_param_block.OverCurrentLimit<OverCurrentLimitMax)
						{
							if((m_device_param_block.KeyDelayNs<=ShortPressTime) && 
							   (m_device_param_block.Key_Relase_Flag == 1))
							{
								m_device_param_block.OverCurrentLimit++; 
							}
							else if((m_device_param_block.KeyDelayNs-KeyDelayNs)/4)
							{
								m_device_param_block.OverCurrentLimit += 11;
								if(m_device_param_block.OverCurrentLimit > OverCurrentLimitMax)
								{
									m_device_param_block.OverCurrentLimit = OverCurrentLimitMax;
								}
								KeyDelayNs = m_device_param_block.KeyDelayNs;
							}	
						}	
					}
					else if(m_device_param_block.Key_Value == KEY_DOWN)
					{
						if(m_device_param_block.OverCurrentLimit>OverCurrentLimitMin)
						{
							if((m_device_param_block.KeyDelayNs<=ShortPressTime) && 
							   (m_device_param_block.Key_Relase_Flag == 1))
							{
								m_device_param_block.OverCurrentLimit--; 
							}
							else if((m_device_param_block.KeyDelayNs-KeyDelayNs)/4)
							{
								if(m_device_param_block.OverCurrentLimit < 11)
								{
									m_device_param_block.OverCurrentLimit = OverCurrentLimitMin;
								}
								m_device_param_block.OverCurrentLimit -= 11;
								if(m_device_param_block.OverCurrentLimit < OverCurrentLimitMin)
								{
									m_device_param_block.OverCurrentLimit = OverCurrentLimitMin;
								}
								KeyDelayNs = m_device_param_block.KeyDelayNs;
							}	
						}					
					}
				}
				//设置漏电电流设定值模式，操作同上
				else if(m_device_param_block.Device_Status == SetOverLeakCurrentLimit_Status)			
				{
					if(m_device_param_block.Key_Value == KEY_SET)
					{
						//短按,则进入设置重合闸次数设定值模式
						if((m_device_param_block.KeyDelayNs<=ShortPressTime) && 
						   (m_device_param_block.Key_Relase_Flag == 1))	
						{
							m_device_param_block.Device_Status = SetRecloseCntLimit_Status;
							TurnAllLedOff();
							LeakCurrent_LED_OFF();
						}
					}
					else if(m_device_param_block.Key_Value == KEY_UP)
					{
						if(m_device_param_block.LeakCurrentLimit<LeakCurrentLimitMax)
						{
							if((m_device_param_block.KeyDelayNs<=ShortPressTime) && 
							   (m_device_param_block.Key_Relase_Flag == 1))
							{
								m_device_param_block.LeakCurrentLimit++; 
							}
							else if((m_device_param_block.KeyDelayNs-KeyDelayNs)/2)
							{
								m_device_param_block.LeakCurrentLimit++; 
								KeyDelayNs = m_device_param_block.KeyDelayNs;
							}	
						}	
					}
					else if(m_device_param_block.Key_Value == KEY_DOWN)
					{
						if(m_device_param_block.LeakCurrentLimit>leakCurrentLimitMin)
						{
							if((m_device_param_block.KeyDelayNs<=ShortPressTime) && 
							   (m_device_param_block.Key_Relase_Flag == 1))
							{
								m_device_param_block.LeakCurrentLimit--; 
							}
							else if((m_device_param_block.KeyDelayNs-KeyDelayNs)/2)
							{
								m_device_param_block.LeakCurrentLimit--; 
								KeyDelayNs = m_device_param_block.KeyDelayNs;
							}	
						}					
					}
				}
				//设置重合闸次数模式，操作同上
				else if(m_device_param_block.Device_Status == SetRecloseCntLimit_Status)			
				{
					if(m_device_param_block.Key_Value == KEY_SET)
					{
						//短按,则进入设置通信地址设定值模式
						if((m_device_param_block.KeyDelayNs<=ShortPressTime) && 
						   (m_device_param_block.Key_Relase_Flag == 1))	
						{
							m_device_param_block.Device_Status = SetDevice_Adderss_Status;
							TurnAllLedOff();
						}
					}
					else if(m_device_param_block.Key_Value == KEY_UP)
					{
						if(m_device_param_block.RecloseCntLimit<RecloseCntMax)
						{
							if((m_device_param_block.KeyDelayNs<=ShortPressTime) && 
							   (m_device_param_block.Key_Relase_Flag == 1))
							{
								m_device_param_block.RecloseCntLimit++; 
							}
							else if((m_device_param_block.KeyDelayNs-KeyDelayNs)/2)
							{
								m_device_param_block.RecloseCntLimit++; 
								KeyDelayNs = m_device_param_block.KeyDelayNs;
							}	
						}	
					}
					else if(m_device_param_block.Key_Value == KEY_DOWN)
					{
						if(m_device_param_block.RecloseCntLimit>RecloseCntMin)
						{
							if((m_device_param_block.KeyDelayNs<=ShortPressTime) && 
							   (m_device_param_block.Key_Relase_Flag == 1))
							{
								m_device_param_block.RecloseCntLimit--; 
							}
							else if((m_device_param_block.KeyDelayNs-KeyDelayNs)/2)
							{
								m_device_param_block.RecloseCntLimit--; 
								KeyDelayNs = m_device_param_block.KeyDelayNs;
							}	
						}					
					}
				}
				//设置通信地址数模式，操作同上
				else if(m_device_param_block.Device_Status == SetDevice_Adderss_Status)			
				{
					if(m_device_param_block.Key_Value == KEY_SET)
					{
						//短按,则进入设置过压设定值模式
						if((m_device_param_block.KeyDelayNs<=ShortPressTime) && 
						   (m_device_param_block.Key_Relase_Flag == 1))	
						{
							m_device_param_block.Device_Status = SetOverVoltageLimit_Status;
							TurnAllLedOff();
							OverVoltage_LED_ON();
						}
					}
					else if(m_device_param_block.Key_Value == KEY_UP)
					{
						if(m_device_param_block.Device_Address<Device_AddressMax)
						{
							if((m_device_param_block.KeyDelayNs<=ShortPressTime) && 
							   (m_device_param_block.Key_Relase_Flag == 1))
							{
								m_device_param_block.Device_Address++; 
							}
							else if((m_device_param_block.KeyDelayNs-KeyDelayNs)/2)
							{
								m_device_param_block.Device_Address++; 
								KeyDelayNs = m_device_param_block.KeyDelayNs;
							}	
						}	
					}
					else if(m_device_param_block.Key_Value == KEY_DOWN)
					{
						if(m_device_param_block.Device_Address>Device_AddressMin)
						{
							if((m_device_param_block.KeyDelayNs<=ShortPressTime) && 
							   (m_device_param_block.Key_Relase_Flag == 1))
							{
								m_device_param_block.Device_Address--; 
							}
							else if((m_device_param_block.KeyDelayNs-KeyDelayNs)/2)
							{
								m_device_param_block.Device_Address--; 
								KeyDelayNs = m_device_param_block.KeyDelayNs;
							}	
						}					
					}
				}
			}			
		}
	}

	//无按键按下
	else if(m_device_param_block.Key_Value == KEY_NONE)
	{
		//查看模式
		if(((m_device_param_block.Device_Status & 0x10) == 0x00) && (m_device_param_block.RelayCtrl_Status != RelayLock_Status))
		{
			//按键没操作10s，则返回到正常模式
			if(m_device_param_block.Key_IDEL_Delay>=10)
			{
				m_device_param_block.Key_IDEL_Delay = 0;
				if(m_device_param_block.Device_Status != Normal_Status)
				{
					m_device_param_block.Device_Status = Normal_Status;
					TurnAllLedOff();
					OverVoltage_LED_OFF();
					UnderVoltage_LED_OFF();
					OverCurrent_LED_OFF();
					LeakCurrent_LED_OFF();						
				}
			}
		}
	}
	//如果按键已经释放
	if(m_device_param_block.Key_Relase_Flag == 1)
	{
		//长按，则置长按释放标志位
		if(m_device_param_block.KeyDelayNs>=LongPressTime)
		{
			LongPressRelaseFlag = 1;
			Clear_flag = 0;
		}

		if(setbackflag == 1)
		{
			setbackflag = 0;
			m_device_param_block.RelayCtrl_Status = RelayReadyON_Status;
			m_device_param_block.Run_Timer = 0;							//延时器清零
			m_device_param_block.Touble_Flag = 0;
		}
		//按键值清零
		m_device_param_block.Key_Value = KEY_NONE;
		KeyDelayNs = 0;
		m_device_param_block.KeyDelayNs = 0;
	}
}

//数码管显示
void Digital_Display(void)
{
	unsigned int temp;
	unsigned char i=0;
	switch(m_device_param_block.Device_Status)
	{
	case Normal_Status:
		if(m_device_param_block.LedUpdateFlag == 1)								//数码管数据显示更新标志位置1 为了避免频繁的计算
		{
			temp = (unsigned int)m_device_param_block.Voltage;				//取电压整数部分
			while(temp/10)
			{
				m_device_param_block.LedDispCode[2-i] = LedCode[(temp % 10)];	//取余
				temp /= 10;
				i++;
				if(i>=2)
					break;
			}
			m_device_param_block.LedDispCode[2-i] = LedCode[(temp % 10)];
			m_device_param_block.LedDispCode[3] = LedCode[10];					//电压单位
			i++;
			m_device_param_block.LedDatabitcount = i;							//数据位个数
			m_device_param_block.LedUpdateFlag = 0;								
		}
		m_device_param_block.Unitbitcount = 1;									//单位显示1位
		m_device_param_block.LedTwinkleFlag = 0;								//正常显示电压模式不LED不闪烁
		break;
		
	case ShowCurrent_Status:
		if(m_device_param_block.LedUpdateFlag == 1)
		{
			temp = (unsigned int)(m_device_param_block.Current*10);
			m_device_param_block.LedDispCode[2] = LedCode[(temp % 10)];
			temp /= 10;
			m_device_param_block.LedDispCode[1] = LedCode[(temp % 10)]&0x7F;
			temp /= 10;
			if(temp>0)
			{
				m_device_param_block.LedDispCode[0] = LedCode[temp];
				m_device_param_block.LedDatabitcount = 3;
			}
			else
			{
				m_device_param_block.LedDatabitcount = 2;
			}
			m_device_param_block.LedDispCode[3] = LedCode[12];			
		}
		
		m_device_param_block.Unitbitcount = 1;
		m_device_param_block.LedTwinkleFlag = 0;
		break;
		
	case ShowLeakCurrent_Status:
		if(m_device_param_block.LedUpdateFlag == 1)
		{
			temp = (unsigned int)m_device_param_block.LeakCurrent;
			//大于或者等于2位，则只显示2位
			if((temp/10) > 0)
			{
				m_device_param_block.LedDispCode[1] = LedCode[(temp % 10)];
				temp /= 10;
				m_device_param_block.LedDispCode[0] = LedCode[(temp % 10)];
				m_device_param_block.LedDatabitcount = 2;
			}
			else
			{
				m_device_param_block.LedDispCode[1] = LedCode[(temp % 10)];
				m_device_param_block.LedDatabitcount = 1;
			}
			m_device_param_block.LedDispCode[2] = LedCode[11];
			m_device_param_block.LedDispCode[3] = LedCode[12];			
		}
		m_device_param_block.Unitbitcount = 2;
		m_device_param_block.LedTwinkleFlag = 0;
		break;
		
	case ShowAllTroubleCnt_Status:
		if(m_device_param_block.LedUpdateFlag == 1)
		{
			temp = m_device_param_block.AllToubleCnt;
			while(temp/10)
			{
				m_device_param_block.LedDispCode[3-i] = LedCode[(temp % 10)];
				temp /= 10;
				i++;
				if(i>=3)
					break;
			}
			m_device_param_block.LedDispCode[3-i] = LedCode[(temp % 10)];
			i++;
			m_device_param_block.LedDatabitcount = i;			
		}

		m_device_param_block.Unitbitcount = 0;
		m_device_param_block.LedTwinkleFlag = 0;
		break;
		
	case ShowOverVoltageCnt_Status:
		if(m_device_param_block.LedUpdateFlag == 1)
		{
			temp = m_device_param_block.OverVoltageCnt;
			while(temp/10)
			{
				m_device_param_block.LedDispCode[3-i] = LedCode[(temp % 10)];
				temp /= 10;
				i++;
				if(i>=3)
					break;
			}
			m_device_param_block.LedDispCode[3-i] = LedCode[(temp % 10)];
			i++;
			m_device_param_block.LedDatabitcount = i;
		}
		m_device_param_block.Unitbitcount = 0;
		m_device_param_block.LedTwinkleFlag = 0;
		break;
		
	case ShowUnderVoltageCnt_Status:
		if(m_device_param_block.LedUpdateFlag == 1)
		{
			temp = m_device_param_block.UnderVoltageCnt;
			while(temp/10)
			{
				m_device_param_block.LedDispCode[3-i] = LedCode[(temp % 10)];
				temp /= 10;
				i++;
				if(i>=3)
					break;
			}
			m_device_param_block.LedDispCode[3-i] = LedCode[(temp % 10)];
			i++;
			m_device_param_block.LedDatabitcount = i;						
		}
		m_device_param_block.Unitbitcount = 0;
		m_device_param_block.LedTwinkleFlag = 0;
		break;
		
	case ShowOverCurrentCnt_Status:
		if(m_device_param_block.LedUpdateFlag == 1)
		{
			temp = m_device_param_block.OverCurrentCnt;
			while(temp/10)
			{
				m_device_param_block.LedDispCode[3-i] = LedCode[(temp % 10)];
				temp /= 10;
				i++;
				if(i>=3)
					break;
			}
			m_device_param_block.LedDispCode[3-i] = LedCode[(temp % 10)];
			i++;
			m_device_param_block.LedDatabitcount = i;		
		}
		m_device_param_block.Unitbitcount = 0;
		m_device_param_block.LedTwinkleFlag = 0;		
		break;		
		
	case ShowLeakCurrentCnt_Status:
		if(m_device_param_block.LedUpdateFlag == 1)
		{
			temp = m_device_param_block.LeakCurrentCnt;
			while(temp/10)
			{
				m_device_param_block.LedDispCode[3-i] = LedCode[(temp % 10)];
				temp /= 10;
				i++;
				if(i>=3)
					break;
			}
			m_device_param_block.LedDispCode[3-i] = LedCode[(temp % 10)];
			i++;
			m_device_param_block.LedDatabitcount = i;					
		}
		m_device_param_block.Unitbitcount = 0;
		m_device_param_block.LedTwinkleFlag = 0;	
		break;	
		
	case ShutDownLED_Status:
		m_device_param_block.LedDispCode[0] = 0xFF;
		m_device_param_block.LedDispCode[1] = 0xFF;
		m_device_param_block.LedDispCode[2] = 0xFF;
		m_device_param_block.LedDispCode[3] = 0xFF;
		m_device_param_block.LedDatabitcount = 4;
		m_device_param_block.Unitbitcount = 0;
		m_device_param_block.LedTwinkleFlag = 0;
		break;
		
	case SetOverVoltageLimit_Status:
		if(m_device_param_block.LedUpdateFlag == 1)
		{
			temp = (unsigned int)m_device_param_block.OverVoltageLimit;
			while(temp/10)
			{
				m_device_param_block.LedDispCode[2-i] = LedCode[(temp % 10)];
				temp /= 10;
				i++;
				if(i>=2)
					break;
			}
			m_device_param_block.LedDispCode[2-i] = LedCode[(temp % 10)];
			m_device_param_block.LedDispCode[3] = LedCode[10];
			i++;
			m_device_param_block.LedDatabitcount = i;
		}
		m_device_param_block.Unitbitcount = 1;
		
		if(((m_device_param_block.Key_Value == KEY_UP) || (m_device_param_block.Key_Value == KEY_DOWN)) && (m_device_param_block.Key_Relase_Flag ==0))
			m_device_param_block.LedTwinkleFlag = 0;
		else
			m_device_param_block.LedTwinkleFlag = 1;
		break;
		
	case SetUnderVoltgaeLimit_Status:
		if(m_device_param_block.LedUpdateFlag == 1)
		{
			temp = (unsigned int)m_device_param_block.UnderVoltageLimit;
			while(temp/10)
			{
				m_device_param_block.LedDispCode[2-i] = LedCode[(temp % 10)];
				temp /= 10;
				i++;
				if(i>=2)
					break;
			}
			m_device_param_block.LedDispCode[2-i] = LedCode[(temp % 10)];
			m_device_param_block.LedDispCode[3] = LedCode[10];
			i++;
			m_device_param_block.LedDatabitcount = i;			
		}
		m_device_param_block.Unitbitcount = 1;
		//设置模式，长按up和down键，不放开，则LED不闪烁
		if(((m_device_param_block.Key_Value == KEY_UP) || (m_device_param_block.Key_Value == KEY_DOWN)) && (m_device_param_block.Key_Relase_Flag ==0))
			m_device_param_block.LedTwinkleFlag = 0;
		else
			m_device_param_block.LedTwinkleFlag = 1;
		break;	
		
	case SetOverCurrentLimit_Status:
		if(m_device_param_block.LedUpdateFlag == 1)
		{
			temp = m_device_param_block.OverCurrentLimit;
			m_device_param_block.LedDispCode[2] = LedCode[(temp % 10)];
			temp /= 10;
			m_device_param_block.LedDispCode[1] = LedCode[(temp % 10)]&0x7F;
			temp /= 10;
			if(temp>0)
			{
				m_device_param_block.LedDispCode[0] = LedCode[temp];
				m_device_param_block.LedDatabitcount = 3;
			}
			else
			{
				m_device_param_block.LedDatabitcount = 2;
			}
			m_device_param_block.Unitbitcount = 1;			
		}	
		m_device_param_block.LedDispCode[3] = LedCode[12];
			
		if(((m_device_param_block.Key_Value == KEY_UP) || (m_device_param_block.Key_Value == KEY_DOWN)) && (m_device_param_block.Key_Relase_Flag ==0))
			m_device_param_block.LedTwinkleFlag = 0;
		else
			m_device_param_block.LedTwinkleFlag = 1;
		break;		
		
	case SetOverLeakCurrentLimit_Status:
		if(m_device_param_block.LedUpdateFlag == 1)
		{
			temp = (unsigned int)m_device_param_block.LeakCurrentLimit;
			//大于或者等于2位，则只显示2位
			if((temp/10) > 0)
			{
				m_device_param_block.LedDispCode[1] = LedCode[(temp % 10)];
				temp /= 10;
				m_device_param_block.LedDispCode[0] = LedCode[(temp % 10)];
				m_device_param_block.LedDatabitcount = 2;
			}
			else
			{
				m_device_param_block.LedDispCode[1] = LedCode[(temp % 10)];
				m_device_param_block.LedDatabitcount = 1;
			}
			m_device_param_block.LedDispCode[2] = LedCode[11];
			m_device_param_block.LedDispCode[3] = LedCode[12];		
		}
		m_device_param_block.Unitbitcount = 2;		
		if(((m_device_param_block.Key_Value == KEY_UP) || (m_device_param_block.Key_Value == KEY_DOWN)) && (m_device_param_block.Key_Relase_Flag ==0))
			m_device_param_block.LedTwinkleFlag = 0;
		else
			m_device_param_block.LedTwinkleFlag = 1;
		break;		
		
	case SetRecloseCntLimit_Status:
		if(m_device_param_block.LedUpdateFlag == 1)
		{
			m_device_param_block.LedDispCode[0] = LedCode[m_device_param_block.RecloseCntLimit];		
		}
		m_device_param_block.LedDispCode[1] = LedCode[13];
		m_device_param_block.LedDispCode[2] = LedCode[14];
		m_device_param_block.LedDispCode[3] = LedCode[15];
		m_device_param_block.LedDatabitcount = 1;
		m_device_param_block.Unitbitcount = 3;	
		if(((m_device_param_block.Key_Value == KEY_UP) || (m_device_param_block.Key_Value == KEY_DOWN)) && (m_device_param_block.Key_Relase_Flag ==0))
			m_device_param_block.LedTwinkleFlag = 0;
		else
			m_device_param_block.LedTwinkleFlag = 1;
		break;
		
	case SetDevice_Adderss_Status:
		if(m_device_param_block.LedUpdateFlag == 1)
		{
			temp = (unsigned int)m_device_param_block.Device_Address;
			while(temp/10)
			{
				m_device_param_block.LedDispCode[2-i] = LedCode[(temp % 10)];
				temp /= 10;
				i++;
				if(i>=2)
					break;
			}
			m_device_param_block.LedDispCode[2-i] = LedCode[(temp % 10)];
			m_device_param_block.LedDispCode[3] = LedCode[16];
			i++;
			m_device_param_block.LedDatabitcount = i;			
		}
		m_device_param_block.Unitbitcount = 1;		
		if(((m_device_param_block.Key_Value == KEY_UP) || (m_device_param_block.Key_Value == KEY_DOWN)) && (m_device_param_block.Key_Relase_Flag ==0))
			m_device_param_block.LedTwinkleFlag = 0;
		else
			m_device_param_block.LedTwinkleFlag = 1;
		break;	
	default:break;
	}
}

//ADC采样管理
//电压、电流采样10次取平均值
//漏电电流采样3次取平均值
void ADC_Sample_Manage(void)
{
	static unsigned char samplecnt[3];
	static double adcvaluetemp[3];
	double voltage;
	//周期采样完成
	if(m_device_param_block.ADCPeriodSampleOkFlag == 1)
	{	
		//采样电压通道
		if(m_device_param_block.SampleChannel == 0)
		{
			samplecnt[0]++;																			//采样计数器加1
			adcvaluetemp[0] += Calc_ADC((unsigned int *)m_device_param_block.Adcbuf)*Voltage_K;		//计算得到电压值														
			
			m_device_param_block.SampleChannel = 1;
			if(samplecnt[0]>=10)																	//采样10次，计算平均值
			{
				samplecnt[0] = 0;
				voltage = adcvaluetemp[0]/10;
				if(voltage<180.0)
					voltage *= 1.06;
				m_device_param_block.Voltage = voltage;
				adcvaluetemp[0] = 0.0;
			}
			memset(m_device_param_block.Adcbuf,0x00,sizeof(m_device_param_block.Adcbuf));
			SelectADC_Channel(CurrentChannel);														//切换到采样电流通道
		}
		//采样电流通道
		else if(m_device_param_block.SampleChannel == 1)
		{
			samplecnt[1]++;
			adcvaluetemp[1] += Calc_ADC((unsigned int *)m_device_param_block.Adcbuf)*20*Current_K;	
			
			m_device_param_block.SampleChannel = 2;
			if(samplecnt[1]>=20)
			{
				samplecnt[1]=0;
				m_device_param_block.Current = (adcvaluetemp[1]/20)*1.414;
				adcvaluetemp[1] = 0.0;
			}
			memset(m_device_param_block.Adcbuf,0x00,sizeof(m_device_param_block.Adcbuf));
			SelectADC_Channel(LeakCurrentChannel);
		}
		//采样漏电电流通道
		else if(m_device_param_block.SampleChannel == 2)
		{
			samplecnt[2]++;
			adcvaluetemp[2] += Calc_ADC((unsigned int *)m_device_param_block.Adcbuf)*200*LeakCurrent_K;
			
			m_device_param_block.SampleChannel = 0;	
			if(samplecnt[2]>=5)
			{
				samplecnt[2]=0;
				m_device_param_block.LeakCurrent = (adcvaluetemp[2]/5)*1.414;
				adcvaluetemp[2] = 0.0;
			}	
			memset(m_device_param_block.Adcbuf,0x00,sizeof(m_device_param_block.Adcbuf));
			SelectADC_Channel(VoltageChannel);
		}
		m_device_param_block.ADCPeriodSampleOkFlag = 0;
		TIM2_Cmd(ENABLE);															//开启定时器2，启动周期采样
	}	
}

//事件处理逻辑
//检测到电压过压，则点亮对应的指示灯，如果在合闸状态下，则置故障标识，对应的故障计数器加1
//检测到电压欠压，则点亮对应的指示灯，如果在合闸状态下，则置故障标识，对应的故障计数器加1
//检测到电压正常，则熄灭过压和欠压指示灯
//检测持续3s电流大于设定值，小于三倍设定值，如果在合闸状态下，置故障标识，对应的故障计数器加1
//检测到电流大于3倍的设定值，如果在合闸状态下，则置故障标识，对应的故障计数器加1
//检测到漏电电流大于设定值，如果在合闸状态下，则置故障标识，对应的故障计数器加1
//为了安全起见，在分闸状态下，如果也检测到漏电电流或者电流值，则发出蜂鸣器报警
//检测到故障后，对应的指示灯是长亮还是闪烁？

//检测到任何故障后，如果在合闸状态下，则跳闸，响蜂鸣器，点亮故障指示灯，如果在分闸状态下，则如何操作？比如30s后，一直是过压，还重合闸还是直接锁死
//如果检测到没有故障发生，如果在合闸状态下，则不执行任何动作，如果在分闸状态下，则延时对应分闸次数的时间后重新合闸。

//合闸前，每隔2s显示过压、欠压、过流，漏电电流，点亮对应的指示灯，回到显示通用状态，显示电压值，如果有对应的故障，则指示灯、蜂鸣器以1hz频率反转，运行灯常亮

//进入设置界面时，不管有无故障，运行灯常亮，进入设置状态，进入不用的设置状态，常亮对应的指示灯，蜂鸣器不响
//从设置界面返回到通用显示界面，则重新执行合闸前的流程。

//事件处理
void Event_Processing(void)
{
	static unsigned char overcurrentflag=0;
	unsigned char touble_flag = 0x00;
	unsigned char shortcircuitflag = 0;
	
	//检测到电压超过过压设定值
	if((unsigned int)m_device_param_block.Voltage > m_device_param_block.OverVoltageLimit)
	{
		touble_flag |= 0x01;
	}
	//检测到电压低于欠压设定值
	if((unsigned int)m_device_param_block.Voltage < m_device_param_block.UnderVoltageLimit)
	{
		touble_flag |= 0x02;
	}

	//检测到电流大于过流设定值 电流值放大10倍，是因为过流设定值为了方便显示小数点，设定值放大了10倍
	if((unsigned int)(m_device_param_block.Current*10) > m_device_param_block.OverCurrentLimit)
	{
		//检测到电流小于3倍的设定值，则如果持续3s，则跳闸
		if((unsigned int)(m_device_param_block.Current * 10) < (3 * m_device_param_block.OverCurrentLimit))
		{
			//第一次检测到过流，则清计时计数器，置过流标识位
			if(overcurrentflag == 0)
			{
				overcurrentflag = 1;
				m_device_param_block.OverCurrent_DelayTime = 0;
			}
			else if(overcurrentflag == 1)										
			{
				if(m_device_param_block.OverCurrent_DelayTime >= OverCurrentDelayTime)		//持续时间超过
				{
					touble_flag |= 0x04;
				}	
			}
		}
		//大于3倍设定值，则直接跳闸
		else
		{	
			touble_flag |= 0x04;
			shortcircuitflag = 1;
		}
	}
	//电流正常
	else
	{
		overcurrentflag = 0;	
		m_device_param_block.OverCurrent_DelayTime = 0;
	}
		
	//检测到漏电电流大于设定值，则跳闸，对应的故障计数器加1，总故障计数器加1
	if((unsigned int)m_device_param_block.LeakCurrent > m_device_param_block.LeakCurrentLimit)
	{
		touble_flag |= 0x08;
	}		
	//合闸状态下,检测到故障才处理
	if(m_device_param_block.RelayCtrl_Status == RelayON_Status)
	{
		//有故障
		if(touble_flag != 0x00)
		{
			m_device_param_block.Touble_Flag = touble_flag;
			if(m_device_param_block.Touble_Flag & 0x01)
			{
				m_device_param_block.OverVoltageCnt++;					//过压计数器加1
				m_device_param_block.AllToubleCnt++;					//总故障计数器加1
			}
			
			if(m_device_param_block.Touble_Flag & 0x02)
			{
				m_device_param_block.UnderVoltageCnt++;					//欠压计数器加1
				m_device_param_block.AllToubleCnt++;					//总故障计数器加1				
			}
			
			if(m_device_param_block.Touble_Flag & 0x04)
			{
				m_device_param_block.OverCurrentCnt++;					//过流计数器加1
				m_device_param_block.AllToubleCnt++;					//总故障计数器加1				
			}
			if(m_device_param_block.Touble_Flag & 0x08)
			{
				m_device_param_block.LeakCurrentCnt++;					//漏电计数器加1
				m_device_param_block.AllToubleCnt++;					//总故障计数器加1				
			}
			Relay_OFF();												//执行跳闸动作
			m_device_param_block.Run_Timer = 0;							//运行时间清零    运行时间计数器会有重复利用的危险
			SaveParam();												//保存数据
			Trouble_LED_ON();											//故障指示灯亮
			RunLED_ON();												//有故障发生时，运行灯常亮
			m_device_param_block.Touble_FlagOld = m_device_param_block.Touble_Flag;
			m_device_param_block.Touble_Reason = m_device_param_block.Touble_Flag;
			
			if(shortcircuitflag == 0)									//非短路故障，则进去等待重合闸状态
				m_device_param_block.RelayCtrl_Status = RelayWaitON_Status;
			
			else if(shortcircuitflag == 1)								//短路故障，则黑屏，自锁状态
			{
				m_device_param_block.RelayCtrl_Status = RelayLock_Status;
				m_device_param_block.Device_Status = ShutDownLED_Status;
			}
			m_device_param_block.RemoteCtrl_Status = RemoteCtrl_IDEL_Status;				//远程控制置为空闲
		}
		else
		{
			m_device_param_block.Touble_Reason = 0x00;
			//无故障
			if(m_device_param_block.Touble_FlagOld & 0x0C)
			{
				if(m_device_param_block.Run_Timer > RecloseCntClearDelayTime)
				{
					m_device_param_block.RecloseCnt = 0;
					m_device_param_block.Touble_FlagOld = 0;
					m_device_param_block.Run_Timer = 0;
				}				
			}
			else
			{
				m_device_param_block.Touble_FlagOld = 0;
				m_device_param_block.RecloseCnt = 0;
				m_device_param_block.Run_Timer = 0;
			}
			m_device_param_block.Touble_Flag = 0;
			Trouble_LED_OFF();											//故障指示灯灭
			if(((m_device_param_block.Device_Status & 0x1F) < 0x03))
			{
				UnderVoltage_LED_OFF();
				OverVoltage_LED_OFF();
				LeakCurrent_LED_OFF();
				OverCurrent_LED_OFF();				
			}
			
			if(m_device_param_block.RemoteCtrl_Status == RemoteCtrl_OFF_Status)		//远程分闸
			{
				Relay_OFF();														//执行跳闸动作
				m_device_param_block.Run_Timer = 0;									//运行时间清零    运行时间计数器会有重复利用的危险
				RunLED_ON();														//运行灯常亮
				m_device_param_block.RelayCtrl_Status = RelayRemoteOFF_Status;
			}
		}
	}
	
	//准备重合闸状态下，不对故障做处理
	else if(m_device_param_block.RelayCtrl_Status == RelayReadyON_Status)
	{
		RunLED_ON();												//运行灯常亮
		
		if(m_device_param_block.Run_Timer < 2)
		{
			Trouble_LED_OFF();										//故障指示灯灭
			OverVoltage_LED_OFF();
			UnderVoltage_LED_OFF();
			OverCurrent_LED_OFF();
			LeakCurrent_LED_OFF();
		}
		
		if((m_device_param_block.Run_Timer >= 2) && (m_device_param_block.Run_Timer < 4))
		{
			OverVoltage_LED_ON();
		}
		else if((m_device_param_block.Run_Timer >= 4) && (m_device_param_block.Run_Timer < 6))
		{
			OverVoltage_LED_OFF();
			UnderVoltage_LED_ON();			
		}
		else if((m_device_param_block.Run_Timer >= 6) && (m_device_param_block.Run_Timer < 8))
		{
			m_device_param_block.Device_Status = ShowCurrent_Status;
			//TurnAllLedOff();
			UnderVoltage_LED_OFF();
			OverCurrent_LED_ON();			
		}
		else if((m_device_param_block.Run_Timer >= 8) && (m_device_param_block.Run_Timer < 10))
		{
			m_device_param_block.Device_Status = ShowLeakCurrent_Status;
			//TurnAllLedOff();
			OverCurrent_LED_OFF();
			LeakCurrent_LED_ON();			
		}
		else if(m_device_param_block.Run_Timer >= 10)
		{
			m_device_param_block.Run_Timer = 0;							//运行延时器清零
			m_device_param_block.Device_Status = Normal_Status;			//切换到正常显示模式
			//TurnAllLedOff();											//清数码管显示
			LeakCurrent_LED_OFF();										//关闭漏电电流指示灯			
			m_device_param_block.Touble_Flag = touble_flag;
			//无任何故障
			if(m_device_param_block.Touble_Flag == 0)
			{
				Relay_ON();												//执行合闸动作
				m_device_param_block.RelayCtrl_Status = RelayON_Status;
			}
			else
			{
				Trouble_LED_ON();
				m_device_param_block.RelayCtrl_Status = RelayWaitON_Status;
			}			
		}
		m_device_param_block.Key_IDEL_Delay = 0;						//按键空闲时间清零		
	}	
			
	//延时等待重合闸
	else if(m_device_param_block.RelayCtrl_Status == RelayWaitON_Status)
	{
		if((touble_flag & 0x08) == 0x08)
		{
			m_device_param_block.Touble_Flag |= 0x08;
		}
		if(m_device_param_block.RecloseCnt < m_device_param_block.RecloseCntLimit)
		{
			//延时时间达到相应合闸次数的时间，则合闸
			if(m_device_param_block.Run_Timer >= RecloseDelayNs[m_device_param_block.RecloseCnt])
			{
				m_device_param_block.RelayCtrl_Status = RelayReadyON_Status;
				m_device_param_block.Touble_Flag = 0;
				m_device_param_block.Device_Status = Normal_Status;								//切换到正常显示模式
				TurnAllLedOff();																//清数码管显示
				m_device_param_block.Run_Timer = 0;												//延时器清零					
				m_device_param_block.RecloseCnt++;												//重合闸次数计数器加1
				overcurrentflag = 0;
				
			}
			
			if(m_device_param_block.RemoteCtrl_Status == RemoteCtrl_ON_Status)					//等待重合闸时，远程合闸，则进入准备合闸状态
			{
				m_device_param_block.RelayCtrl_Status = RelayReadyON_Status;					//继电器控制切换到准备合闸状态
				m_device_param_block.RemoteCtrl_Status = RemoteCtrl_IDEL_Status;				//远程控制置为空闲
				m_device_param_block.Touble_Flag = 0;
				m_device_param_block.Device_Status = Normal_Status;								//切换到正常显示模式
				TurnAllLedOff();																//清数码管显示
				m_device_param_block.Run_Timer = 0;												//延时器清零
				overcurrentflag = 0;
			}
		}
		//合闸次数超过设定值，则重合闸设备自锁，需要重新断电重启
		else 
		{
			if(touble_flag != 0x00)
				m_device_param_block.Touble_Flag = touble_flag;
			m_device_param_block.RelayCtrl_Status = RelayLock_Status;
			m_device_param_block.Device_Status = ShutDownLED_Status;				
		}			
	}
	else if(m_device_param_block.RelayCtrl_Status == RelayRemoteOFF_Status)			//远程分闸状态，只有等待远程合闸命令
	{
		if(m_device_param_block.RemoteCtrl_Status == RemoteCtrl_ON_Status)
		{
			m_device_param_block.RelayCtrl_Status = RelayReadyON_Status;
			m_device_param_block.RemoteCtrl_Status = RemoteCtrl_IDEL_Status;
			m_device_param_block.Run_Timer = 0;	
			m_device_param_block.Device_Status = Normal_Status;	
		}
	}
	else if(m_device_param_block.RelayCtrl_Status == RelayLock_Status)					//自锁状态
	{
		if(m_device_param_block.RemoteCtrl_Status == RemoteCtrl_ON_Status)
		{
			m_device_param_block.RelayCtrl_Status = RelayReadyON_Status;
			m_device_param_block.RemoteCtrl_Status = RemoteCtrl_IDEL_Status;
			m_device_param_block.Touble_Flag = 0;
			m_device_param_block.Device_Status = Normal_Status;								//切换到正常显示模式
			TurnAllLedOff();																//清数码管显示
			m_device_param_block.Run_Timer = 0;												//延时器清零					
			m_device_param_block.RecloseCnt = 0;											//重合闸次数计数器清零
		}		
	}
}

//参数初始化
void Param_Init(void)
{
	m_device_param_block.RecloseCnt = 0;
	m_device_param_block.Voltage = 220.0;
	m_device_param_block.Current = 0.0;
	m_device_param_block.LeakCurrent = 0.0;
	m_device_param_block.Device_Status = 0x00;
	m_device_param_block.Run_Timer = 0;						//运行计时器清零
	m_device_param_block.LedDispCode[3] = LedCode[10];
	m_device_param_block.LedDispCode[2] = LedCode[0];
	m_device_param_block.LedDispCode[1] = LedCode[2];
	m_device_param_block.LedDispCode[0] = LedCode[2];
	m_device_param_block.LedDatabitcount = 3;
	m_device_param_block.Unitbitcount = 1;
	m_device_param_block.RelayCtrl_Status = RelayOFF_Status;
	m_device_param_block.Touble_FlagOld = 0x00;
	m_device_param_block.RemoteCtrl_Status = RemoteCtrl_IDEL_Status;
	m_device_param_block.ComRevTimeout = 0;
}

void SaveParam(void)
{
	m_poweroff_save_param_block.UnderVoltageLimit = m_device_param_block.UnderVoltageLimit;
	m_poweroff_save_param_block.OverVoltageLimit = m_device_param_block.OverVoltageLimit;
	m_poweroff_save_param_block.OverCurrentLimit = m_device_param_block.OverCurrentLimit;
	m_poweroff_save_param_block.LeakCurrentLimit = m_device_param_block.LeakCurrentLimit;
	m_poweroff_save_param_block.UnderVoltageCnt = m_device_param_block.UnderVoltageCnt;
	m_poweroff_save_param_block.OverVoltageCnt = m_device_param_block.OverVoltageCnt;
	m_poweroff_save_param_block.OverCurrentCnt = m_device_param_block.OverCurrentCnt;
	m_poweroff_save_param_block.LeakCurrentCnt = m_device_param_block.LeakCurrentCnt;
	m_poweroff_save_param_block.AllToubleCnt = m_device_param_block.AllToubleCnt;
	m_poweroff_save_param_block.Device_Address = m_device_param_block.Device_Address;
	m_poweroff_save_param_block.RecloseCntLimit = m_device_param_block.RecloseCntLimit;
	m_poweroff_save_param_block.CloseCnt = m_device_param_block.CloseCnt;
	m_poweroff_save_param_block.Touble_Reason = m_device_param_block.Touble_Reason;	
	m_poweroff_save_param_block.CRC = Sum((unsigned char *)&m_poweroff_save_param_block,(unsigned int)(&m_poweroff_save_param_block.CRC)-(unsigned int)(&m_poweroff_save_param_block));
	ProgramDeviceAddr((unsigned char *)&m_poweroff_save_param_block,sizeof(m_poweroff_save_param_block));
}

void Communication(void)
{
	unsigned char len;
	if(m_device_param_block.ComRevTimeout > 10)
	{
		if(m_device_param_block.Lenth >= 8)
		{
			//解码正确
			if(Decode(m_device_param_block.ComRxBuf,m_device_param_block.Lenth))
			{
				m_device_param_block.Lenth = 0;
				len = Encode(m_device_param_block.ComTxBuf);		//编码
				Uart_Tx_Data(len,m_device_param_block.ComTxBuf);	//发送
			}
			//解码出错
			else
			{
				m_device_param_block.Lenth = 0;
			}
		}
	}
}

