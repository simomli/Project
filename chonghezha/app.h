#ifndef _APP_H_
#define _APP_H_

unsigned char Sum(unsigned char *crcbuf,unsigned int bytelen);
void DealKey(void);
void Digital_Display(void);
void ADC_Sample_Manage(void);
void Event_Processing(void);
void Param_Init(void);
void SaveParam(void);
void Relay_ON(void);
void Relay_OFF(void);
void TurnAllLedOff(void);
void Communication(void);

#endif