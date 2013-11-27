#include "board.h"

uint32_t time=0;

int main(void)
{
  //set up systick
  SysTick_Config(SystemCoreClock / 1000);
  
  configureSPI();
  configureGPIO();
  
//   uint16_t thispin = 0x00;
//   uint8_t up = 0x01;
  
  while(1)
  {
//     thispin = 0x0100;
//     while (thispin != 0x0000) {
//       if(up&0x01)
//       {
//         GPIO_SetBits(GPIOE, thispin);
//         thispin = thispin << 1;
//       } else {
//         GPIO_ResetBits(GPIOE, thispin);
//         thispin = thispin << 1;
//       }
//       delay(100);
//     }
//     
//     up ^= 0x01;
    GPIO_SetBits(GPIOA, GPIO_Pin_4);
    SPI_I2S_SendData16(SPI1, 0xA5);
    while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE));
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY));
    GPIO_ResetBits(GPIOA, GPIO_Pin_4);
    GPIOE->ODR ^= 0xFF00;
    delay(100);
  }
}
