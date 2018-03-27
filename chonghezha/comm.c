#include "stm8s.h"
#include "sysdefine.h"


unsigned int CRC_16(unsigned char *addr,unsigned char length)
{
	unsigned char *p_addr;
	unsigned char i,j;
	unsigned int temp;
	unsigned int temp1;
	temp1=0xffff;                      //预置CRC寄存器为0xffff
	p_addr=addr;
     for(i=0;i<length;i++)
	 {
	      temp1=*p_addr^temp1;        //八位数据和CRC寄存器相异或存到CRC寄存器中
          for(j=0;j<8;j++)
 		  {
			   if(temp1&0x0001)
      		   { 
			       temp1>>=1;            //CRC寄存器右移1位
			       temp1^=0xa001;    //最低位为1则与0xa001相异或
			   }
			   else
			   {
			       temp1>>=1;            //CRC寄存器右移1位
			   }
		  }
		  p_addr++;                //完成8位数据则继续下一个字节
	  }
	  temp=temp1&0x00ff;        //互换CRC寄存器高低字节
	  temp<<=8;                 
	  temp1=temp1&0xff00;
	  temp1>>=8;
      temp=temp|temp1; 
	  return temp;              //返回最终CRC校验码
}

unsigned char Decode(unsigned char *buffer,unsigned char len)
{
	unsigned int crc,crc1;
	unsigned char headpos=0;
	while(headpos < len)
	{
		if(buffer[headpos] == m_device_param_block.Device_Address)
		{
			break;
		}
		else
		{
			headpos++;
		}
	}
	if((headpos + 8) > len)
	{
		return 0;
	}

	crc = CRC_16(buffer + headpos,6);
	crc1 = buffer[headpos + 6] << 8;
	crc1 += buffer[headpos + 7];
	if(crc != crc1)
		return 0;
	m_device_param_block.FunCode = buffer[headpos + 1];
	switch(m_device_param_block.FunCode)
	{
	case 0x03:
		m_device_param_block.Reg_Address = buffer[headpos + 2] << 8;
		m_device_param_block.Reg_Address += buffer[headpos + 3];
		m_device_param_block.Reg_Len = buffer[headpos + 4] << 8;
		m_device_param_block.Reg_Len += buffer[headpos + 5];
		break;
	case 0x06:
		m_device_param_block.Reg_Address = buffer[headpos + 2] << 8;
		m_device_param_block.Reg_Address += buffer[headpos + 3];
		if(0x0010 == m_device_param_block.Reg_Address)
		{
			m_device_param_block.SingleRegisterdata = buffer[headpos + 4] << 8;
			m_device_param_block.SingleRegisterdata += buffer[headpos + 5];
			if((m_device_param_block.SingleRegisterdata >= 0) && (m_device_param_block.SingleRegisterdata <= 2))
			{
				m_device_param_block.RemoteCtrl_Status = m_device_param_block.SingleRegisterdata;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			return 0;
		}
		break;
	default:
		return 0;
	}

	return 1;
}

unsigned char Encode(unsigned char *buffer)
{
	unsigned char len=0;
	unsigned int reg_address;
	unsigned int crc;
	buffer[0] = m_device_param_block.Device_Address;
	buffer[1] = m_device_param_block.FunCode;
	switch(m_device_param_block.FunCode)
	{
	case 0x03:
		buffer[2] = m_device_param_block.Reg_Len*2;
		reg_address = m_device_param_block.Reg_Address;
		for(int i=0;i<m_device_param_block.Reg_Len;i++)
		{
			switch(reg_address+i)
			{
			case 0x0000:
				buffer[3+2*i] = (unsigned int)m_device_param_block.Voltage/256;
				buffer[3+2*i+1] = (unsigned int)m_device_param_block.Voltage%256;
				break;
			case 0x0001:
				buffer[3+2*i] = (unsigned int)(m_device_param_block.Current*10)/256;
				buffer[3+2*i+1] = (unsigned int)(m_device_param_block.Current*10)%256;
				break;
			case 0x0002:
				buffer[3+2*i] = (unsigned int)m_device_param_block.LeakCurrent/256;
				buffer[3+2*i+1] = (unsigned int)m_device_param_block.LeakCurrent%256;
				break;	
			case 0x0003:
				buffer[3+2*i] = (unsigned int)m_device_param_block.CloseCnt/256;
				buffer[3+2*i+1] = (unsigned int)m_device_param_block.CloseCnt%256;
				break;	
			case 0x0004:
				buffer[3+2*i] = (unsigned int)m_device_param_block.Touble_Reason/256;
				buffer[3+2*i+1] = (unsigned int)m_device_param_block.Touble_Reason%256;
				break;	
			case 0x0005:
				buffer[3+2*i] = (unsigned int)m_device_param_block.AllToubleCnt/256;
				buffer[3+2*i+1] = (unsigned int)m_device_param_block.AllToubleCnt%256;
				break;
			case 0x0006:
				buffer[3+2*i] = 0x00;
				if(m_device_param_block.RelayCtrl_Status == RelayON_Status)
				{
					buffer[3+2*i+1] = 0x01;
				}
				else
				{
					buffer[3+2*i+1] = 0x00;
				}
				break;	
			default:break;
			}
		}
		len = 3+m_device_param_block.Reg_Len*2;		
		break;
	case 0x06:
		buffer[2] = m_device_param_block.Reg_Address/256;
		buffer[3] = m_device_param_block.Reg_Address%256;
		buffer[4] = m_device_param_block.SingleRegisterdata/256;
		buffer[5] = m_device_param_block.SingleRegisterdata%256;
		len = 6;
		break;
	default:
		break;
			
	}
	crc = CRC_16(buffer,len);
	buffer[len] = crc/256;
	buffer[len+1] = crc%256;
	return len+2;
}

