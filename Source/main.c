#include "board.h"

extern __IO uint32_t receiveLength;  // HJI
extern __IO uint32_t packetSent;     // HJI

//#define TX
#define RX
//#define VCP_TEST

int main(void)
{
  //set up systick
  SysTick_Config(SystemCoreClock / 1000);
  
  #if defined(TX) || defined(RX)
  configureSi4463();
  startupRF();
  modemInit();
  uint8_t cmdResponse[MAX_RESPONSE_LENGTH];
  uint8_t modem_status_data[2] = {GET_MODEM_STATUS,0x00}; //tx this to read back modem status info, make this a drv_Si function later TODO:
  
  uint8_t txData[11] = {0x56, 0x45, 0x32, 0x4E, 0x50, 0x51, 0, 0, 0, 0, 0}; //six byte VE2NPQ, five bytes servo data 10bitx4=40bit, 11 bytes total
  uint8_t rxData[sizeof(txData)] = {0};
  memcpy(rxData, txData, sizeof(txData));
  uint16_t servos[4] = {1500, 1500, 1500, 1500};
  
  //move this to its own function
  uint8_t rxStartData[8] = {START_RX, 0x00, 0x00, 0x00, sizeof(rxData), 0x08, 0x03, 0x08}; //rx - channel0 - start immediately - datalength (2B) - timeout=rearmRX - valid=readymode - invalid=rearmRX
  #endif
  
  Set_System();
  Set_USBClock();
  USB_Interrupts_Config();
  USB_Init();
  
  
  configureGPIO(); //Just for discovery LEDs, PE8-PE15

  /**********************************
  Init TXRX
  **********************************/
  
  #ifdef TX
//  modemTxInit();
  #endif
  
  #ifdef RX
//  modemRxInit();
  pwmOutputInit();
  #endif

  /**********************************
  Loop
  **********************************/
  
  /** TX ***************************/
  #ifdef TX
  
  uint32_t lastTX = 0;
  uint16_t servosRX[4] = {1500, 1500, 1500, 1500};
  uint32_t servotime = 0;
  uint8_t RXbool = 0;
  
  
  while(1)
  {
    // Send data at 50Hz
    // RX state in gaps
    if((lastTX+50) < millis())
    {
      packServos(servos, txData);
      modemTxInit();
      clearInts();
      modemTX(txData, sizeof(txData), 0);
      
      // update LEDs
      for(int i=0; i<4; i++)
      {
        if(servos[i] < 1500)
          GPIO_ResetBits(GPIOE, GPIO_Pin_8<<i);
        else
          GPIO_SetBits(GPIOE, GPIO_Pin_8<<i);
      }
      
      // update servos
      if((servotime+1000) < millis()) // change servos every second
      {
        uint16_t newservo = ((servos[0] == 1000) ? 2000 : 1000);
        for(int i=0; i<4; i++)
        {
          servos[i] = newservo;
        }
        servotime = millis();
      }
      
      // go back to RX mode
      modemRxInit();
      clearInts();
      Si4436_Cmd_Response(rxStartData, sizeof(rxStartData), cmdResponse, 0);
      
      lastTX = millis();
    }
    
    if(GPIO_ReadInputDataBit(nIRQ_GPIO,nIRQ) == Bit_RESET)//wait for int
    {
      Si4436_Cmd_Response(modem_status_data, sizeof(modem_status_data), cmdResponse, 8);
      readRxFifo(rxData, sizeof(rxData));
      clearFIFOs();
      clearInts();
      
    // check RXd header data is OK (pkt compare)
    RXbool = 1;
    for(int i=0; i< 6; i++)
    {
      if(rxData[i] != txData[i])
        RXbool = 0;
    }
    
    if (RXbool) // packet header checks out
    {
      unpackServos(servosRX, rxData);
      for(int i=0; i<4; i++)
      {
        if(servosRX[i] < 1500)
          GPIO_ResetBits(GPIOE, GPIO_Pin_8<<(i+4));
        else
          GPIO_SetBits(GPIOE, GPIO_Pin_8<<(i+4));
      }
      
    }
    RXbool = 1;
    
    // dont re-enter RX mode, already got pkt
    }
  }
  #endif

  /** RX ***************************/
  #ifdef RX
  
  // enter RX mode
  modemRxInit();
  clearInts();
  Si4436_Cmd_Response(rxStartData, sizeof(rxStartData), cmdResponse, 0);
  
  uint8_t RXbool = 0;
  uint32_t ResponseNeeded = 0;

  while (1)
  {
    
    if(GPIO_ReadInputDataBit(nIRQ_GPIO,nIRQ) == Bit_RESET) // wait for int (going low)
      {
      Si4436_Cmd_Response(modem_status_data, sizeof(modem_status_data), cmdResponse, 8);
      readRxFifo(rxData, sizeof(rxData));
      clearFIFOs();
      clearInts();
      
      // check RXd header data is OK (pkt compare)
      RXbool = 1;
      for(int i=0; i< 6; i++)
      {
        if(rxData[i] != txData[i])
          RXbool = 0;
      }
      
      if (RXbool) //packet header checks out
      {
        unpackServos(servos, rxData);
        
        // update LEDs
        for(int i=0; i<4; i++)
        {
          if(servos[i] < 1500)
            GPIO_ResetBits(GPIOE, GPIO_Pin_8<<i);
          else
            GPIO_SetBits(GPIOE, GPIO_Pin_8<<i);
        }
        
        ResponseNeeded = 1;
        
        // update servos
        
      }
      RXbool = 1;
      
      
      // re-enter RX mode
      modemRxInit();
      clearFIFOs();
      clearInts();
      Si4436_Cmd_Response(rxStartData, sizeof(rxStartData), cmdResponse, 0);
    }
      
    if(ResponseNeeded == 1)
    {
      packServos(servos, txData);
      clearFIFOs();
      clearInts();
      modemTxInit();
      modemTX(txData, sizeof(txData), 0);
      
      // re-enter RX mode
      modemRxInit();
      clearFIFOs();
      clearInts();
      Si4436_Cmd_Response(rxStartData, sizeof(rxStartData), cmdResponse, 0);

      ResponseNeeded = 0;
    }
    
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
