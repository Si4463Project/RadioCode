#ifndef __TW88__H
#define __TW88__H


#ifdef __cplusplus
 extern "C" {
#endif
	 
//#include "stm32303c_eval.h"

	 
	 
#define TW88_I2C_ADDR ((0x45 & 0x7F)<< 1)


#define TW88_CMD1 0x02 	 

	 

	 
//extern void Delay(__IO uint32_t nTime);	 
	 
	 
   
   
   
void I2CReset(I2C_TypeDef *,GPIO_TypeDef *,uint8_t,uint8_t);
	 
	 
uint8_t TW88Write(uint8_t, uint8_t);
uint8_t TW88Read(uint8_t, uint8_t *);

uint8_t TW88_Init(void);
uint8_t TW88_AddOSD_Win(uint8_t x, uint8_t y, uint8_t w, uint8_t h);

void ReadAllTW88Regs(void);







#ifdef __cplusplus
}
#endif

#endif




