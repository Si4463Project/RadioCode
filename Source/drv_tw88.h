#ifndef __TW88__H
#define __TW88__H


//#ifdef __cplusplus
// extern "C" {
//#endif
	 
//#include "stm32303c_eval.h"

	 
	 
#define TW88_I2C_ADDR ((0x45 & 0x7F)<< 1)


#define TW88_CMD1 0x02 	 

#define AILLEFTLOC 0x45
#define AILLEFTCHR 0x1B
#define AILRIGHTLOC 0x49
#define AILRIGHTCHR 0x1C





   
   
   
   
//extern void Delay(__IO uint32_t nTime);	 
	 
	 
   
   
   
void I2CReset(I2C_TypeDef *,GPIO_TypeDef *,uint8_t,uint8_t);
	 
	 
uint8_t TW88Write(uint8_t, uint8_t);
uint8_t TW88Read(uint8_t, uint8_t *);

uint8_t TW88_Init(void);
//uint8_t TW88_AddOSD_Win(uint8_t x, uint8_t y, uint8_t w, uint8_t h);

void ReadAllTW88Regs(void);

void setupOSD(void);
void putOSDrssi(uint8_t r, int8_t rssi); // r=0 host, r=1 remote
void putOSDfailsafe(uint8_t on, uint8_t setup);
void putOSDchar(uint8_t y, char c, uint8_t color);





//#ifdef __cplusplus
//}
//#endif

#endif




