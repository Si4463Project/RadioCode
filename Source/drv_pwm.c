
#include "board.h"
// Using TIM2 outputs
// PD3 - Ch1 - Ailerons
// PD4 - Ch2 - Elevator
// PD7 - Ch3 - Throttle
// PD6 - Ch4 - Rudder

// Init functions
void pwmOutputInit(void)
{
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = 20000; //20ms = 50Hz
  TIM_TimeBaseStructure.TIM_Prescaler = (SystemCoreClock / 1000000) - 1; // 1MHz = 1ms resolution
  
  TIM_OCStructInit(&TIM_OCInitStructure);
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //goes low on match
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0; //no servo activity until set
  
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource3, GPIO_AF_2);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_2);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_2);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_2);
  
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Pin = ( GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_6 | GPIO_Pin_7 );
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_1; //low 1MHz speed
  
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  TIM_OC1Init(TIM2, &TIM_OCInitStructure);
  TIM_OC2Init(TIM2, &TIM_OCInitStructure);
  TIM_OC3Init(TIM2, &TIM_OCInitStructure);
  TIM_OC4Init(TIM2, &TIM_OCInitStructure);
  TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
  TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
  TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
  TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);
  TIM_Cmd(TIM2, ENABLE);
  GPIO_Init(GPIOD, &GPIO_InitStructure);
}

void setPWM(uint16_t * pwmArray)
{
  TIM2->CCR1 = pwmArray[0];
  TIM2->CCR2 = pwmArray[1];
  TIM2->CCR3 = pwmArray[2];
  TIM2->CCR4 = pwmArray[3];
}

void packServos(uint16_t * servos, uint8_t * pack)
{
  uint16_t temp[4];
  for (int i=0;i<4;i++)
  {
    temp[i] = servos[i]-1000;
  }
  pack[6] = (uint8_t)(temp[0] & 0xff);
  pack[7] = (uint8_t)(temp[1] & 0xff);
  pack[8] = (uint8_t)(temp[2] & 0xff);
  pack[9] = (uint8_t)(temp[3] & 0xff);
  pack[10] = (uint8_t)( ((temp[0] >> 8) & 3) | (((temp[1] >> 8) & 3) << 2) | (((temp[2] >> 8) & 3) << 4) | (((temp[3] >> 8) & 3) << 6) );
}

void unpackServos(uint16_t * servos, uint8_t * pack)
{
  uint16_t temp[4];
  temp[0] = (uint16_t)((uint16_t)pack[6] | (uint16_t)((pack[10] & 0x03) << 8));
  temp[1] = (uint16_t)((uint16_t)pack[7] | (uint16_t)((pack[10] & 0x0c) << 6));
  temp[2] = (uint16_t)((uint16_t)pack[8] | (uint16_t)((pack[10] & 0x30) << 4));
  temp[3] = (uint16_t)((uint16_t)pack[9] | (uint16_t)((pack[10] & 0xc0) << 2));
  
  for (int i=0;i<4;i++)
  {
    servos[i] = temp[i]+1000;
  }
}
