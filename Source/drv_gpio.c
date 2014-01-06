
#include "board.h"

uint16_t DMA_outputMask;

void configureGPIO(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /*******GPIOE LEDs for discovery board********/
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE);
  //RCC_AHBPeriph_DMA1
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Pin = 0xFF00;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  
  GPIO_ResetBits(GPIOE, 0xFF00);
  GPIO_Init(GPIOE, &GPIO_InitStructure);

}

