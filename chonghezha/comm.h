#ifndef _COMM_H_
#define _COMM_H_

unsigned int CRC_16(unsigned char *addr,unsigned char length);
unsigned char Decode(unsigned char *buffer,unsigned char len);
unsigned char Encode(unsigned char *buffer);
//void Communication(void);


#endif