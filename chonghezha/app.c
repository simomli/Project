#include "sysdefine.h"
										//30�룬5���ӣ�10���ӣ�30���ӣ�1Сʱ
const unsigned int RecloseDelayNs[5] = {30,300,600,1800,3600};
//const unsigned int RecloseDelayNs[5] = {5,10,15,1800,3600};

//0~9��������ܶ���� 0xC1:U  0xAB:m 0x88:A 0xBF:- 0xAB:n 0x87:T 0xA1:d
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


//��������
void DealKey(void)
{
	static unsigned int KeyDelayNs=0;
	static unsigned char LongPressRelaseFlag = 1;
	static unsigned char Clear_flag = 0;
	static unsigned char key_flag=0;
	static unsigned char key_temp_old=0;
	static unsigned char setbackflag=0;
	unsigned char key_temp;
	
	//������ܷǺ����Ҽ̵�����׼���غ�բ�Ҽ̵���������״̬�£��Ų�ѯ����
	if((m_device_param_block.Device_Status != ShutDownLED_Status)     && 
	   (m_device_param_block.RelayCtrl_Status != RelayReadyON_Status) && 
	   (m_device_param_block.RelayCtrl_Status != RelayLock_Status))
	{
		key_temp = GetKeyValue();				//��ȡ����ֵ
		if(key_temp != KEY_NONE)				//��ѯ���а�������
		{	
			if(key_flag == 0)					//����δ����
			{
				key_flag = 1;					//������־λ��1
				DelayMs(10);					//��ʱ10ms
				key_temp_old = key_temp;		//���水��ֵ
			}
			else if(key_flag ==1)				//�Ѿ�����
			{
				if(key_temp == key_temp_old)	//�˴ΰ���ֵ������ǰ����ֵһ������ʶͬһ������ֵ��Ч
				{
					m_device_param_block.Key_Value = key_temp;										//���水��ֵ
					if(m_device_param_block.Key_Value_Old != m_device_param_block.Key_Value)		//��ΰ���ֵ����һ�ΰ���ֵ��һ�������ʾ���µİ�����ѯ��
					{
						m_device_param_block.Key_Value_Old = m_device_param_block.Key_Value;		//��һ�εİ���ֵ����Ϊ��εİ���ֵ����ҪΪ��ʵ�ְ���һ��������������һ��
						if(m_device_param_block.SilencerFlag == 0)									//����������������죬ͬʱ�÷�������ʶλ��Ϊ��ʵ����һ���ٹص�
						{
							Beer_ON();
							m_device_param_block.BeerONFlag = 1;
						}
					}
					m_device_param_block.Key_Relase_Flag = 0;										//�а������£������ɿ���־λ����
				}
			}
		}
		//û�в�ѯ������
		else
		{
			m_device_param_block.Key_Relase_Flag = 1;												//�����ɿ���־λ��1
			key_flag = 0;																			//������־����
			m_device_param_block.Key_Value_Old = KEY_NONE;											//�ϰ���ֵ����											
		}		
	}
	else	//����״̬�£�����ֵ����Ϊ�ް���
	{
		m_device_param_block.Key_Value = KEY_NONE;
	}

	//����©������򿪣�2s��ر�
	if(m_device_param_block.LeakCurrentTestFlag == 1)
	{
		if(m_device_param_block.Run_Timer >= 2)													//��ʱ2s����ر�©���������
		{
			m_device_param_block.LeakCurrentTestFlag = 0;										//©��������Ա�ʶλ����
			m_device_param_block.Run_Timer = 0;													//��ʱ������
			LeakCurrent_Ctrl_OFF();																//�ر�©���·														
		}
	}
	//�а�������
	if(m_device_param_block.Key_Value != KEY_NONE)
	{
		m_device_param_block.Key_IDEL_Delay = 0;												//�����޲�����ʱ������
		//�̵���������ģʽ
		if(m_device_param_block.RelayCtrl_Status != RelayLock_Status)
		{
			//�ڲ鿴ģʽ
			if((m_device_param_block.Device_Status & 0x10) == 0x00)						
			{
				//���κβ鿴ģʽ�£��������ü�
				if(m_device_param_block.Key_Value == KEY_SET)							
				{	
					//�����ҳ����ɿ���־λΪ1�������Ĭ�����ù�ѹ�趨ֵģʽ
					//�Ӳ鿴ģʽת�����ù�ѹ���޲���ģʽ������Ҫ��բ�����еƳ�������ѹָʾ�Ƴ�������������ָʾ��Ϩ��
					if((m_device_param_block.KeyDelayNs>=LongPressTime) && 
					   (LongPressRelaseFlag == 1))												
					{
						m_device_param_block.Device_Status = SetOverVoltageLimit_Status;			//״̬����Ϊ���ù�ѹ����ģʽ
						m_device_param_block.Touble_Flag = 0;
						m_device_param_block.RelayCtrl_Status = RelayOFF_Status;
						m_device_param_block.Run_Timer = 0;
						TurnAllLedOff();															//���������ʾ
						LongPressRelaseFlag = 0;													//�����ɿ���ʶλ����
						OverVoltage_LED_ON();														//������ѹLEDָʾ�ƣ���������ָʾ��Ϩ��
						UnderVoltage_LED_OFF();
						OverCurrent_LED_OFF();
						LeakCurrent_LED_OFF();
						Trouble_LED_OFF();
						Relay_OFF();																//ִ����բ����
						RunLED_ON();																//���еƳ���
					}
					
					//�̰�SET��
					if((m_device_param_block.KeyDelayNs<=ShortPressTime) && 
					   (m_device_param_block.Key_Relase_Flag == 1))	
					{
						TurnAllLedOff();
						//��ǰ��ʾ��ѹģʽ���л�����ʾ����ģʽ
						if(m_device_param_block.Device_Status == Normal_Status)	
						{
							m_device_param_block.Device_Status = ShowCurrent_Status;							
						}
						////��ǰ��ʾ����ģʽ���л�����ʾ©�����ģʽ
						else if(m_device_param_block.Device_Status == ShowCurrent_Status)
						{
							m_device_param_block.Device_Status = ShowLeakCurrent_Status;
						}
						//��ǰ��ʾ©�����ģʽ���л�����ʾ��ѹģʽ
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
				
				//���κβ鿴ģʽ�£����������£�δ���������������������ģ��ָ�
				if(m_device_param_block.Key_Value == KEY_UP)
				{
					//�̰�
					if((m_device_param_block.KeyDelayNs<=ShortPressTime) && 
					   (m_device_param_block.Key_Relase_Flag == 1))	
					{
						//����Ѿ���������򿪣���Ӧ��LEDָʾ��Ϩ��
						if(m_device_param_block.SilencerFlag == 1)						
						{
							m_device_param_block.SilencerFlag = 0;
							Silencer_LED_OFF();
						}
						//���δ����������������Ӧ��LEDָʾ�Ƶ���
						else if(m_device_param_block.SilencerFlag == 0)					
						{
							m_device_param_block.SilencerFlag = 1;
							Silencer_LED_ON();	
							Beer_OFF();
						}
					}
				}
				//���κβ鿴ģʽ�£����/��ѯ������ �̰����ڹ�ѹ���ϣ�Ƿѹ���ϣ��������ϣ�©������л���ʾ���ϴ�������������������ϼ�¼
				
				if(m_device_param_block.Key_Value == KEY_CLEAR)
				{
					//�̰���������ѯ״̬
					if((m_device_param_block.KeyDelayNs<=ShortPressTime) && 
					   (m_device_param_block.Key_Relase_Flag == 1))	
					{
						//�ر����й��϶�Ӧ��ָʾ��
						OverVoltage_LED_OFF();
						UnderVoltage_LED_OFF();
						OverCurrent_LED_OFF();
						LeakCurrent_LED_OFF();
						//��ǰ����ʾ�ܹ��ϴ���ģʽ,�������ʾ��ѹ���ϴ���
						if(m_device_param_block.Device_Status == ShowAllTroubleCnt_Status)					
						{
							m_device_param_block.Device_Status = ShowOverVoltageCnt_Status;
							TurnAllLedOff();
							OverVoltage_LED_ON();
						}
						//��ǰ����ʾ��ѹ���ϴ����������Ƿѹ���ϴ���
						else if(m_device_param_block.Device_Status == ShowOverVoltageCnt_Status)				
						{
							m_device_param_block.Device_Status = ShowUnderVoltageCnt_Status;
							TurnAllLedOff();
							UnderVoltage_LED_ON();						
						}
						//��ǰ����ʾǷѹ���ϴ����������������ϴ���
						else if(m_device_param_block.Device_Status == ShowUnderVoltageCnt_Status)				
						{
							m_device_param_block.Device_Status = ShowOverCurrentCnt_Status;
							TurnAllLedOff();
							OverCurrent_LED_ON();							
						}
						//��ǰ����ʾ�������ϴ����������©��������ϴ���
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
					//����5s���������й��ϴ���
					if(m_device_param_block.KeyDelayNs>=LongPressTime)				
					{
						if(Clear_flag == 0)
						{
							m_device_param_block.LeakCurrentCnt = 0;
							m_device_param_block.OverCurrentCnt = 0;
							m_device_param_block.OverVoltageCnt = 0;
							m_device_param_block.UnderVoltageCnt = 0;
							m_device_param_block.AllToubleCnt = 0;
							Beer_ON();												//�������
							m_device_param_block.BeerONFlag = 1;
							SaveParam();											//�������
							Clear_flag = 1;						
						}
					}
				}
				//���κβ鿴ģʽ�£�����©�簴������
				if(m_device_param_block.Key_Value == KEY_TEST)
				{
					//�̰�
					if((m_device_param_block.KeyDelayNs<=ShortPressTime) && 
					   (m_device_param_block.Key_Relase_Flag == 1))	
					{
						LeakCurrent_Ctrl_ON();									//��©���·
						m_device_param_block.LeakCurrentTestFlag = 1;			//����©�������ʶλ��1
						m_device_param_block.Run_Timer = 0;						//��ʱ������
					}						
				}
			}

			//������ģʽ�£��̰����ü���ģʽ�ڹ�ѹ��Ƿѹ��������©�磬��բ��������ֵ��ͨ�ŵ�ַ�л�
			//�������ü�����ص�����ģʽ��׼����ʼ�غ�բ
			//����ģʽ�£�ֻ�����ü����ϼ����¼������ã��̰����¼����ֱ��1��1�������������Ӽ�
			else if((m_device_param_block.Device_Status &0x10) == 0x10)							
			{
				//���ð���
				if(m_device_param_block.Key_Value == KEY_SET)
				{
					//�������ü�����������ģʽ���������
					if((m_device_param_block.KeyDelayNs>=LongPressTime) && (LongPressRelaseFlag == 1))
					{
						m_device_param_block.Device_Status = Normal_Status;			//���ص�����ģʽ
						setbackflag = 1;
						
						m_device_param_block.Touble_Flag = 0;
						TurnAllLedOff();											//���������ʾ
						LongPressRelaseFlag = 0;
						OverVoltage_LED_OFF();										//Ϩ������LED��
						UnderVoltage_LED_OFF();
						OverCurrent_LED_OFF();
						LeakCurrent_LED_OFF();
						m_device_param_block.Run_Timer = 0;							//��ʱ������
						SaveParam();												//�������
					}
				}
				
				//���ù�ѹ�趨ֵģʽ,�̰����ü������л�������Ƿѹ�趨ֵģʽ���̰����¼���������Ӽ����������������Ӽ�
				if(m_device_param_block.Device_Status == SetOverVoltageLimit_Status)			
				{
					//�л�/���ð�������
					if(m_device_param_block.Key_Value == KEY_SET)
					{
						//�̰�,���������Ƿѹ�趨ֵģʽ
						if((m_device_param_block.KeyDelayNs<=ShortPressTime) && 
						   (m_device_param_block.Key_Relase_Flag == 1))	
						{
							m_device_param_block.Device_Status = SetUnderVoltgaeLimit_Status;	//����Ƿѹ�趨ֵģʽ
							TurnAllLedOff();													//���������ʾ
							UnderVoltage_LED_ON();												//Ƿѹָʾ�Ƴ���
							OverVoltage_LED_OFF();												//��ѹָʾ��Ϩ��
						}
					}
					//UP��
					else if(m_device_param_block.Key_Value == KEY_UP)							
					{
						//��ѹ�趨ֵС�ڹ�ѹ���ֵ
						if(m_device_param_block.OverVoltageLimit<OverVoltgaeLimitMax)
						{
							//�̰������趨ֵ��1
							if((m_device_param_block.KeyDelayNs<=ShortPressTime) && 
							   (m_device_param_block.Key_Relase_Flag == 1))
							{
								m_device_param_block.OverVoltageLimit++; 
							}
							//��������������
							else if((m_device_param_block.KeyDelayNs-KeyDelayNs)/2)
							{
								m_device_param_block.OverVoltageLimit ++; 
								KeyDelayNs = m_device_param_block.KeyDelayNs;
							}	
						}	
					}
					//down��
					else if(m_device_param_block.Key_Value == KEY_DOWN)						
					{
						//��ѹ�趨ֵ���ڹ�ѹ��Сֵ
						if(m_device_param_block.OverVoltageLimit>OverVoltgaeLimitMin)
						{
							//�̰������趨ֵ��1
							if((m_device_param_block.KeyDelayNs<=ShortPressTime) && 
							   (m_device_param_block.Key_Relase_Flag == 1))
							{
								m_device_param_block.OverVoltageLimit--; 
							}
							//���������趨ֵ������1
							else if((m_device_param_block.KeyDelayNs-KeyDelayNs)/2)
							{
								m_device_param_block.OverVoltageLimit -- ; 
								KeyDelayNs = m_device_param_block.KeyDelayNs;
							}	
						}					
					}
				}
				//����Ƿѹ�趨ֵģʽ������ͬ��
				else if(m_device_param_block.Device_Status == SetUnderVoltgaeLimit_Status)			
				{
					if(m_device_param_block.Key_Value == KEY_SET)
					{
						//�̰�,��������ù����趨ֵģʽ
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
				//���ù����趨ֵģʽ������ͬ��
				else if(m_device_param_block.Device_Status == SetOverCurrentLimit_Status)			
				{
					if(m_device_param_block.Key_Value == KEY_SET)
					{
						//�̰�,���������©������趨ֵģʽ
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
				//����©������趨ֵģʽ������ͬ��
				else if(m_device_param_block.Device_Status == SetOverLeakCurrentLimit_Status)			
				{
					if(m_device_param_block.Key_Value == KEY_SET)
					{
						//�̰�,����������غ�բ�����趨ֵģʽ
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
				//�����غ�բ����ģʽ������ͬ��
				else if(m_device_param_block.Device_Status == SetRecloseCntLimit_Status)			
				{
					if(m_device_param_block.Key_Value == KEY_SET)
					{
						//�̰�,���������ͨ�ŵ�ַ�趨ֵģʽ
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
				//����ͨ�ŵ�ַ��ģʽ������ͬ��
				else if(m_device_param_block.Device_Status == SetDevice_Adderss_Status)			
				{
					if(m_device_param_block.Key_Value == KEY_SET)
					{
						//�̰�,��������ù�ѹ�趨ֵģʽ
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

	//�ް�������
	else if(m_device_param_block.Key_Value == KEY_NONE)
	{
		//�鿴ģʽ
		if(((m_device_param_block.Device_Status & 0x10) == 0x00) && (m_device_param_block.RelayCtrl_Status != RelayLock_Status))
		{
			//����û����10s���򷵻ص�����ģʽ
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
	//��������Ѿ��ͷ�
	if(m_device_param_block.Key_Relase_Flag == 1)
	{
		//���������ó����ͷű�־λ
		if(m_device_param_block.KeyDelayNs>=LongPressTime)
		{
			LongPressRelaseFlag = 1;
			Clear_flag = 0;
		}

		if(setbackflag == 1)
		{
			setbackflag = 0;
			m_device_param_block.RelayCtrl_Status = RelayReadyON_Status;
			m_device_param_block.Run_Timer = 0;							//��ʱ������
			m_device_param_block.Touble_Flag = 0;
		}
		//����ֵ����
		m_device_param_block.Key_Value = KEY_NONE;
		KeyDelayNs = 0;
		m_device_param_block.KeyDelayNs = 0;
	}
}

//�������ʾ
void Digital_Display(void)
{
	unsigned int temp;
	unsigned char i=0;
	switch(m_device_param_block.Device_Status)
	{
	case Normal_Status:
		if(m_device_param_block.LedUpdateFlag == 1)								//�����������ʾ���±�־λ��1 Ϊ�˱���Ƶ���ļ���
		{
			temp = (unsigned int)m_device_param_block.Voltage;				//ȡ��ѹ��������
			while(temp/10)
			{
				m_device_param_block.LedDispCode[2-i] = LedCode[(temp % 10)];	//ȡ��
				temp /= 10;
				i++;
				if(i>=2)
					break;
			}
			m_device_param_block.LedDispCode[2-i] = LedCode[(temp % 10)];
			m_device_param_block.LedDispCode[3] = LedCode[10];					//��ѹ��λ
			i++;
			m_device_param_block.LedDatabitcount = i;							//����λ����
			m_device_param_block.LedUpdateFlag = 0;								
		}
		m_device_param_block.Unitbitcount = 1;									//��λ��ʾ1λ
		m_device_param_block.LedTwinkleFlag = 0;								//������ʾ��ѹģʽ��LED����˸
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
			//���ڻ��ߵ���2λ����ֻ��ʾ2λ
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
		//����ģʽ������up��down�������ſ�����LED����˸
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
			//���ڻ��ߵ���2λ����ֻ��ʾ2λ
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

//ADC��������
//��ѹ����������10��ȡƽ��ֵ
//©���������3��ȡƽ��ֵ
void ADC_Sample_Manage(void)
{
	static unsigned char samplecnt[3];
	static double adcvaluetemp[3];
	double voltage;
	//���ڲ������
	if(m_device_param_block.ADCPeriodSampleOkFlag == 1)
	{	
		//������ѹͨ��
		if(m_device_param_block.SampleChannel == 0)
		{
			samplecnt[0]++;																			//������������1
			adcvaluetemp[0] += Calc_ADC((unsigned int *)m_device_param_block.Adcbuf)*Voltage_K;		//����õ���ѹֵ														
			
			m_device_param_block.SampleChannel = 1;
			if(samplecnt[0]>=10)																	//����10�Σ�����ƽ��ֵ
			{
				samplecnt[0] = 0;
				voltage = adcvaluetemp[0]/10;
				if(voltage<180.0)
					voltage *= 1.06;
				m_device_param_block.Voltage = voltage;
				adcvaluetemp[0] = 0.0;
			}
			memset(m_device_param_block.Adcbuf,0x00,sizeof(m_device_param_block.Adcbuf));
			SelectADC_Channel(CurrentChannel);														//�л�����������ͨ��
		}
		//��������ͨ��
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
		//����©�����ͨ��
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
		TIM2_Cmd(ENABLE);															//������ʱ��2���������ڲ���
	}	
}

//�¼������߼�
//��⵽��ѹ��ѹ���������Ӧ��ָʾ�ƣ�����ں�բ״̬�£����ù��ϱ�ʶ����Ӧ�Ĺ��ϼ�������1
//��⵽��ѹǷѹ���������Ӧ��ָʾ�ƣ�����ں�բ״̬�£����ù��ϱ�ʶ����Ӧ�Ĺ��ϼ�������1
//��⵽��ѹ��������Ϩ���ѹ��Ƿѹָʾ��
//������3s���������趨ֵ��С�������趨ֵ������ں�բ״̬�£��ù��ϱ�ʶ����Ӧ�Ĺ��ϼ�������1
//��⵽��������3�����趨ֵ������ں�բ״̬�£����ù��ϱ�ʶ����Ӧ�Ĺ��ϼ�������1
//��⵽©����������趨ֵ������ں�բ״̬�£����ù��ϱ�ʶ����Ӧ�Ĺ��ϼ�������1
//Ϊ�˰�ȫ������ڷ�բ״̬�£����Ҳ��⵽©��������ߵ���ֵ���򷢳�����������
//��⵽���Ϻ󣬶�Ӧ��ָʾ���ǳ���������˸��

//��⵽�κι��Ϻ�����ں�բ״̬�£�����բ�������������������ָʾ�ƣ�����ڷ�բ״̬�£�����β���������30s��һֱ�ǹ�ѹ�����غ�բ����ֱ������
//�����⵽û�й��Ϸ���������ں�բ״̬�£���ִ���κζ���������ڷ�բ״̬�£�����ʱ��Ӧ��բ������ʱ������º�բ��

//��բǰ��ÿ��2s��ʾ��ѹ��Ƿѹ��������©�������������Ӧ��ָʾ�ƣ��ص���ʾͨ��״̬����ʾ��ѹֵ������ж�Ӧ�Ĺ��ϣ���ָʾ�ơ���������1hzƵ�ʷ�ת�����еƳ���

//�������ý���ʱ���������޹��ϣ����еƳ�������������״̬�����벻�õ�����״̬��������Ӧ��ָʾ�ƣ�����������
//�����ý��淵�ص�ͨ����ʾ���棬������ִ�к�բǰ�����̡�

//�¼�����
void Event_Processing(void)
{
	static unsigned char overcurrentflag=0;
	unsigned char touble_flag = 0x00;
	unsigned char shortcircuitflag = 0;
	
	//��⵽��ѹ������ѹ�趨ֵ
	if((unsigned int)m_device_param_block.Voltage > m_device_param_block.OverVoltageLimit)
	{
		touble_flag |= 0x01;
	}
	//��⵽��ѹ����Ƿѹ�趨ֵ
	if((unsigned int)m_device_param_block.Voltage < m_device_param_block.UnderVoltageLimit)
	{
		touble_flag |= 0x02;
	}

	//��⵽�������ڹ����趨ֵ ����ֵ�Ŵ�10��������Ϊ�����趨ֵΪ�˷�����ʾС���㣬�趨ֵ�Ŵ���10��
	if((unsigned int)(m_device_param_block.Current*10) > m_device_param_block.OverCurrentLimit)
	{
		//��⵽����С��3�����趨ֵ�����������3s������բ
		if((unsigned int)(m_device_param_block.Current * 10) < (3 * m_device_param_block.OverCurrentLimit))
		{
			//��һ�μ�⵽�����������ʱ���������ù�����ʶλ
			if(overcurrentflag == 0)
			{
				overcurrentflag = 1;
				m_device_param_block.OverCurrent_DelayTime = 0;
			}
			else if(overcurrentflag == 1)										
			{
				if(m_device_param_block.OverCurrent_DelayTime >= OverCurrentDelayTime)		//����ʱ�䳬��
				{
					touble_flag |= 0x04;
				}	
			}
		}
		//����3���趨ֵ����ֱ����բ
		else
		{	
			touble_flag |= 0x04;
			shortcircuitflag = 1;
		}
	}
	//��������
	else
	{
		overcurrentflag = 0;	
		m_device_param_block.OverCurrent_DelayTime = 0;
	}
		
	//��⵽©����������趨ֵ������բ����Ӧ�Ĺ��ϼ�������1���ܹ��ϼ�������1
	if((unsigned int)m_device_param_block.LeakCurrent > m_device_param_block.LeakCurrentLimit)
	{
		touble_flag |= 0x08;
	}		
	//��բ״̬��,��⵽���ϲŴ���
	if(m_device_param_block.RelayCtrl_Status == RelayON_Status)
	{
		//�й���
		if(touble_flag != 0x00)
		{
			m_device_param_block.Touble_Flag = touble_flag;
			if(m_device_param_block.Touble_Flag & 0x01)
			{
				m_device_param_block.OverVoltageCnt++;					//��ѹ��������1
				m_device_param_block.AllToubleCnt++;					//�ܹ��ϼ�������1
			}
			
			if(m_device_param_block.Touble_Flag & 0x02)
			{
				m_device_param_block.UnderVoltageCnt++;					//Ƿѹ��������1
				m_device_param_block.AllToubleCnt++;					//�ܹ��ϼ�������1				
			}
			
			if(m_device_param_block.Touble_Flag & 0x04)
			{
				m_device_param_block.OverCurrentCnt++;					//������������1
				m_device_param_block.AllToubleCnt++;					//�ܹ��ϼ�������1				
			}
			if(m_device_param_block.Touble_Flag & 0x08)
			{
				m_device_param_block.LeakCurrentCnt++;					//©���������1
				m_device_param_block.AllToubleCnt++;					//�ܹ��ϼ�������1				
			}
			Relay_OFF();												//ִ����բ����
			m_device_param_block.Run_Timer = 0;							//����ʱ������    ����ʱ������������ظ����õ�Σ��
			SaveParam();												//��������
			Trouble_LED_ON();											//����ָʾ����
			RunLED_ON();												//�й��Ϸ���ʱ�����еƳ���
			m_device_param_block.Touble_FlagOld = m_device_param_block.Touble_Flag;
			m_device_param_block.Touble_Reason = m_device_param_block.Touble_Flag;
			
			if(shortcircuitflag == 0)									//�Ƕ�·���ϣ����ȥ�ȴ��غ�բ״̬
				m_device_param_block.RelayCtrl_Status = RelayWaitON_Status;
			
			else if(shortcircuitflag == 1)								//��·���ϣ������������״̬
			{
				m_device_param_block.RelayCtrl_Status = RelayLock_Status;
				m_device_param_block.Device_Status = ShutDownLED_Status;
			}
			m_device_param_block.RemoteCtrl_Status = RemoteCtrl_IDEL_Status;				//Զ�̿�����Ϊ����
		}
		else
		{
			m_device_param_block.Touble_Reason = 0x00;
			//�޹���
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
			Trouble_LED_OFF();											//����ָʾ����
			if(((m_device_param_block.Device_Status & 0x1F) < 0x03))
			{
				UnderVoltage_LED_OFF();
				OverVoltage_LED_OFF();
				LeakCurrent_LED_OFF();
				OverCurrent_LED_OFF();				
			}
			
			if(m_device_param_block.RemoteCtrl_Status == RemoteCtrl_OFF_Status)		//Զ�̷�բ
			{
				Relay_OFF();														//ִ����բ����
				m_device_param_block.Run_Timer = 0;									//����ʱ������    ����ʱ������������ظ����õ�Σ��
				RunLED_ON();														//���еƳ���
				m_device_param_block.RelayCtrl_Status = RelayRemoteOFF_Status;
			}
		}
	}
	
	//׼���غ�բ״̬�£����Թ���������
	else if(m_device_param_block.RelayCtrl_Status == RelayReadyON_Status)
	{
		RunLED_ON();												//���еƳ���
		
		if(m_device_param_block.Run_Timer < 2)
		{
			Trouble_LED_OFF();										//����ָʾ����
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
			m_device_param_block.Run_Timer = 0;							//������ʱ������
			m_device_param_block.Device_Status = Normal_Status;			//�л���������ʾģʽ
			//TurnAllLedOff();											//���������ʾ
			LeakCurrent_LED_OFF();										//�ر�©�����ָʾ��			
			m_device_param_block.Touble_Flag = touble_flag;
			//���κι���
			if(m_device_param_block.Touble_Flag == 0)
			{
				Relay_ON();												//ִ�к�բ����
				m_device_param_block.RelayCtrl_Status = RelayON_Status;
			}
			else
			{
				Trouble_LED_ON();
				m_device_param_block.RelayCtrl_Status = RelayWaitON_Status;
			}			
		}
		m_device_param_block.Key_IDEL_Delay = 0;						//��������ʱ������		
	}	
			
	//��ʱ�ȴ��غ�բ
	else if(m_device_param_block.RelayCtrl_Status == RelayWaitON_Status)
	{
		if((touble_flag & 0x08) == 0x08)
		{
			m_device_param_block.Touble_Flag |= 0x08;
		}
		if(m_device_param_block.RecloseCnt < m_device_param_block.RecloseCntLimit)
		{
			//��ʱʱ��ﵽ��Ӧ��բ������ʱ�䣬���բ
			if(m_device_param_block.Run_Timer >= RecloseDelayNs[m_device_param_block.RecloseCnt])
			{
				m_device_param_block.RelayCtrl_Status = RelayReadyON_Status;
				m_device_param_block.Touble_Flag = 0;
				m_device_param_block.Device_Status = Normal_Status;								//�л���������ʾģʽ
				TurnAllLedOff();																//���������ʾ
				m_device_param_block.Run_Timer = 0;												//��ʱ������					
				m_device_param_block.RecloseCnt++;												//�غ�բ������������1
				overcurrentflag = 0;
				
			}
			
			if(m_device_param_block.RemoteCtrl_Status == RemoteCtrl_ON_Status)					//�ȴ��غ�բʱ��Զ�̺�բ�������׼����բ״̬
			{
				m_device_param_block.RelayCtrl_Status = RelayReadyON_Status;					//�̵��������л���׼����բ״̬
				m_device_param_block.RemoteCtrl_Status = RemoteCtrl_IDEL_Status;				//Զ�̿�����Ϊ����
				m_device_param_block.Touble_Flag = 0;
				m_device_param_block.Device_Status = Normal_Status;								//�л���������ʾģʽ
				TurnAllLedOff();																//���������ʾ
				m_device_param_block.Run_Timer = 0;												//��ʱ������
				overcurrentflag = 0;
			}
		}
		//��բ���������趨ֵ�����غ�բ�豸��������Ҫ���¶ϵ�����
		else 
		{
			if(touble_flag != 0x00)
				m_device_param_block.Touble_Flag = touble_flag;
			m_device_param_block.RelayCtrl_Status = RelayLock_Status;
			m_device_param_block.Device_Status = ShutDownLED_Status;				
		}			
	}
	else if(m_device_param_block.RelayCtrl_Status == RelayRemoteOFF_Status)			//Զ�̷�բ״̬��ֻ�еȴ�Զ�̺�բ����
	{
		if(m_device_param_block.RemoteCtrl_Status == RemoteCtrl_ON_Status)
		{
			m_device_param_block.RelayCtrl_Status = RelayReadyON_Status;
			m_device_param_block.RemoteCtrl_Status = RemoteCtrl_IDEL_Status;
			m_device_param_block.Run_Timer = 0;	
			m_device_param_block.Device_Status = Normal_Status;	
		}
	}
	else if(m_device_param_block.RelayCtrl_Status == RelayLock_Status)					//����״̬
	{
		if(m_device_param_block.RemoteCtrl_Status == RemoteCtrl_ON_Status)
		{
			m_device_param_block.RelayCtrl_Status = RelayReadyON_Status;
			m_device_param_block.RemoteCtrl_Status = RemoteCtrl_IDEL_Status;
			m_device_param_block.Touble_Flag = 0;
			m_device_param_block.Device_Status = Normal_Status;								//�л���������ʾģʽ
			TurnAllLedOff();																//���������ʾ
			m_device_param_block.Run_Timer = 0;												//��ʱ������					
			m_device_param_block.RecloseCnt = 0;											//�غ�բ��������������
		}		
	}
}

//������ʼ��
void Param_Init(void)
{
	m_device_param_block.RecloseCnt = 0;
	m_device_param_block.Voltage = 220.0;
	m_device_param_block.Current = 0.0;
	m_device_param_block.LeakCurrent = 0.0;
	m_device_param_block.Device_Status = 0x00;
	m_device_param_block.Run_Timer = 0;						//���м�ʱ������
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
			//������ȷ
			if(Decode(m_device_param_block.ComRxBuf,m_device_param_block.Lenth))
			{
				m_device_param_block.Lenth = 0;
				len = Encode(m_device_param_block.ComTxBuf);		//����
				Uart_Tx_Data(len,m_device_param_block.ComTxBuf);	//����
			}
			//�������
			else
			{
				m_device_param_block.Lenth = 0;
			}
		}
	}
}
