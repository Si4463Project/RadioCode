#include "board.h"

uint32_t time=0;

//#define TX
#define RX

int main(void)
{
  //set up systick
  SysTick_Config(SystemCoreClock / 1000);
  
  configureSi4463();
  configureGPIO(); //Just for discovery LEDs
  
  uint32_t time;
  
  startupRF();
  modemInit();
  
  uint8_t dummyData[5] = {0x80, 0x55, 0xAA, 0xFF, 0x01};
  //uint8_t dummyData[1] = {0x00};
  
  /**********************************
  Init TXRX
  **********************************/
  
  #ifdef TX
  modemTxInit();
  //delay(1000);
  //modemInitDirect();
  //setRFState(TX_MODE);
  #endif
  
  #ifdef RX
  modemRxInit();
  uint8_t mark = 1;
  //GPIOE->ODR |= 0x0F00;
  #endif

  /**********************************
  Loop
  **********************************/
  
  
  #ifdef TX
  time = millis()+50000;
  //uint32_t blink = 0;
  uint8_t mark = 1;
  while(time>millis())
  {
    dummyData[1] = mark;
    modemTX(dummyData, sizeof(dummyData), 0); //tx on hopchannel = 0
    clearFIFOs();
    //if (millis() > (blink + 500))
    //{
      //if (blink == 0)
      //{
        //GPIOE->ODR |= 0xAA00;
      //}
    GPIOE->ODR = (uint16_t)(mark<<8);
    mark = mark << 1;
    if (mark == 0)
      mark = 1;
      //blink = millis();
    //}
    delay(50);
    //delayMicroseconds(550);
  }
  #endif

  #ifdef RX
  uint8_t cmdResponse[MAX_RESPONSE_LENGTH];
  
  //enter RX mode data
  uint8_t rxStartData[8] = {START_RX, 0x00, 0x00, 0x00, sizeof(dummyData), 0x08, 0x03, 0x08}; //rx - channel0 - start immediately - datalength (2B) - timeout=rearmRX - valid=readymode - invalid=rearmRX
  Si4436_Cmd_Response(rxStartData, sizeof(rxStartData), cmdResponse, 0);
  
  uint8_t RXcount = 0;
  uint8_t RXed[sizeof(dummyData)];
  uint8_t RXbool = 0;
  uint8_t modem_status_data[2] = {GET_MODEM_STATUS,0x00}; //tx this to read back modem status info, make this a drv_Si function later TODO:
  
  while (RXcount < 8)
  {
    //getRFState();
    //delay(100);
    
    while(GPIO_ReadInputDataBit(nIRQ_GPIO,nIRQ)); //wait for int
    Si4436_Cmd_Response(modem_status_data, sizeof(modem_status_data), cmdResponse, 8);
    readRxFifo(RXed, sizeof(dummyData));
    clearFIFOs();
    clearInts();
    
    //check RXd data is OK
    RXbool = 1;
    for(int i=0; i< sizeof(dummyData); i++)
    {
      if((i != 1) & (RXed[i] != dummyData[i]))
        RXbool = 0;
    }
    
    if (RXbool)
    {
      GPIOE->ODR = (uint16_t)(RXed[1]<<8);
      //GPIOE->ODR |= (0x0100 << RXcount); //turn on another LED
      //RXcount++;
    }
    RXbool = 1;
    
    //re-enter RX mode
    Si4436_Cmd_Response(rxStartData, sizeof(rxStartData), cmdResponse, 0);
    
//    //if (0x10 & getFRR(4))
//    //{
//      RXcount++;
//      clearFIFOs();
//      GPIOE->ODR |= (0x0100 << RXcount); //turn on another LED
//    //}
    
  }
  #endif
  
  
  
  /**********************************
  Shutdown
  **********************************/
  
  
  shutdownRF();
  #ifdef TX
  GPIOE->ODR |= 0xFF00; //all eight LEDs on to indicate thread complete
  #endif
}
