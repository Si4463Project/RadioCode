#include "board.h"

uint32_t time=0;

int main(void)
{
  //set up systick
  SysTick_Config(SystemCoreClock / 1000);
  
  configureSi4463();
  configureGPIO(); //Just for discovery LEDs

  uint8_t dummyData[4] = {0x00, 0x55, 0xAA, 0xFF};
  //uint8_t dummyData[1] = {0x00};
  
  uint32_t time;
  
  startupRF();
  modemInit();
  //modemInitDirect();
  
  //setRFState(TX_MODE);
  
  time = millis()+5000;
  while(time>millis())
  {
    modemTX(dummyData, sizeof(dummyData), 0); //tx on hopchannel = 0
    //delay(1);
    //delayMicroseconds(550);
  }

  shutdownRF();

  GPIOE->ODR ^= 0xFF00; //all eight LEDs on to indicate thread complete
}
