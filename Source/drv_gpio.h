
#pragma once

//#define LD3_ON GPIOE->ODR |= 0xFF00; //fix

void configureGPIO(void);
void DMA1_Channel1_IRQHandler(void);
