#include "board.h"

extern __IO uint32_t receiveLength;  // HJI
extern __IO uint32_t packetSent;     // HJI

#define TX
//#define RX
//#define VCP_TEST

int main(void)
{
  //set up systick
  SysTick_Config(SystemCoreClock / 1000);
  
  #if defined(TX) || defined(RX)
  configureSi4463();
  startupRF();
  modemInit();
  uint8_t dummyData[] = {0x80, 0x55, 0xAA, 0xFF, 0x01, 0x80, 0x55, 0xAA, 0xFF, 0x01, 0x80, 0x55, 0xAA, 0xFF, 0x01, 0x80, 0x55, 0xAA, 0xFF, 0x01};
  uint16_t pwmArray[4] = { 0 };
  #endif
  
  Set_System();
  Set_USBClock();
  USB_Interrupts_Config();
  USB_Init();
  
  
  configureGPIO(); //Just for discovery LEDs
  uint32_t time = 0;
  

  /**********************************
  Init TXRX
  **********************************/
  
  #ifdef TX
  modemTxInit();
  #endif
  
  #ifdef RX
  modemRxInit();
  pwmOutputInit();
  #endif




  /**********************************
  Loop
  **********************************/
  
  
  #ifdef TX
  time = millis()+50000;
  //uint32_t blink = 0;
  uint8_t mark = 1;
//  while(time>millis())
  while(1)
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
    delay(5);
    //delayMicroseconds(550);
  }
  #endif

  #ifdef RX
  uint8_t cmdResponse[MAX_RESPONSE_LENGTH];
  
  //enter RX mode data
  uint8_t rxStartData[8] = {START_RX, 0x00, 0x00, 0x00, sizeof(dummyData), 0x08, 0x03, 0x08}; //rx - channel0 - start immediately - datalength (2B) - timeout=rearmRX - valid=readymode - invalid=rearmRX
  Si4436_Cmd_Response(rxStartData, sizeof(rxStartData), cmdResponse, 0);
  
  uint8_t RXed[sizeof(dummyData)];
  uint8_t RXbool = 0;
  uint8_t modem_status_data[2] = {GET_MODEM_STATUS,0x00}; //tx this to read back modem status info, make this a drv_Si function later TODO:
  char rssiSend[] = {0x00};
  char cr[] = {0x00};
  while (1)
  {
    
    while(GPIO_ReadInputDataBit(nIRQ_GPIO,nIRQ)); //wait for int
    Si4436_Cmd_Response(modem_status_data, sizeof(modem_status_data), cmdResponse, 8);
    readRxFifo(RXed, sizeof(dummyData));
    clearFIFOs();
    clearInts();
    
    if(usbIsConfigured())
    {
      CDC_Send_DATA((uint8_t*)rssiSend, sizeof(rssiSend));
      while(packetSent);
      uint8_t tempchar[1] = { cmdResponse[2]} ;
      CDC_Send_DATA((uint8_t*)tempchar, sizeof(tempchar));
      while(packetSent);
      CDC_Send_DATA((uint8_t*)cr, sizeof(cr));
      while(packetSent);
    }
    
    //check RXd data is OK (pkt compare)
    RXbool = 1;
    for(int i=0; i< sizeof(dummyData); i++)
    {
      if((i != 1) & (RXed[i] != dummyData[i]))
        RXbool = 0;
    }
    
    if (RXbool) //packet checks out
    {
      GPIOE->ODR = (uint16_t)(RXed[1]<<8);
      pwmArray[0] = 1000+(RXed[1] * 8);
      setPWM(pwmArray);
    }
    RXbool = 1;
    
    //re-enter RX mode
    Si4436_Cmd_Response(rxStartData, sizeof(rxStartData), cmdResponse, 0);
    
  }
  #endif
  
  #ifdef VCP_TEST
  GPIO_SetBits(GPIOE, GPIO_Pin_9); //first led on
  while(1)
  {
    if(usbIsConfigured())
    {
      GPIO_SetBits(GPIOE, GPIO_Pin_10);
    } else {
      GPIO_ResetBits(GPIOE, GPIO_Pin_10);
    }
    
    if(usbIsConnected())
    {
      GPIO_SetBits(GPIOE, GPIO_Pin_12);
    } else {
      GPIO_ResetBits(GPIOE, GPIO_Pin_12);
    }
    
    if(receiveLength)
    {
      GPIO_SetBits(GPIOE, GPIO_Pin_14);
      
      char buf[receiveLength];
      CDC_Receive_DATA((uint8_t*)buf, receiveLength);
      char loopback1[] = {"Loopback: \r\n"};
      char loopback2[] = {"\r\n"};
      
      CDC_Send_DATA((uint8_t*)loopback1, sizeof(loopback1));
      while(packetSent);
      CDC_Send_DATA((uint8_t*)buf, sizeof(buf));
      while(packetSent);
      CDC_Send_DATA((uint8_t*)loopback2, sizeof(loopback2));
      while(packetSent);
      
      GPIO_ResetBits(GPIOE, GPIO_Pin_14);
    }
    
    delay(10);
  }
  #endif
  
  
  
  
  /**********************************
  Shutdown
  **********************************/
  
  #if defined(TX) || defined(RX)
  shutdownRF();
  #endif
  
  #ifdef TX
  GPIOE->ODR |= 0xFF00; //all eight LEDs on to indicate thread complete
  #endif
  

}
