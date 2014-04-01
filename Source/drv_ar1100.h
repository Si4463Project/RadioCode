#ifndef AR1100_H
#define AR1100_H

#ifdef __cplusplus
 extern "C" {
#endif
	
	 
//#include "stm32f30x.h"
//#include "stm32303c_eval.h"

	 
#define MAX_BUFFSIZE          15
#define AR1100_PEN_DOWN_FLAG  0x81
#define AR1100_PEN_UP_FLAG    0x80
#define RX_BUFFER_LENGTH      (40)
	 
	 
typedef enum {PEN_DOWN=0, PEN_UP} pen_state;

typedef struct {
	uint16_t x;
	uint16_t y;
	pen_state p;
	uint8_t processed;
} TouchCoords;	


typedef struct {
	
	void (*onTouchEvent)(TouchCoords coords);
	
	
} TouchInitStr;



	 
static __IO uint8_t touch_buffer[MAX_BUFFSIZE];
//extern __IO TouchCoords touch_coords;




void AR1100Init(void);
void AR1100IRQSetup(FunctionalState newstatus);
void AR1100TouchEnable(void);
void AR1100TouchDisable(void);

void AR1100_WriteReg(uint8_t reg_addr, uint8_t regval);
void AR1100_ReadReg(uint8_t reg_addr, uint8_t *regval);

uint8_t AR1100_ReadData(uint8_t *buffer, const uint8_t sz);
	 
	 







#ifdef __cplusplus
 }
#endif






#endif
