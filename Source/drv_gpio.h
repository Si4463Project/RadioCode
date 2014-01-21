
#ifndef DRV_GPIO_H
#define DRV_GPIO_H

//#define LD3_ON GPIOE->ODR |= 0xFF00; //fix

void configureGPIO(void);
void DMA1_Channel1_IRQHandler(void);

#endif
