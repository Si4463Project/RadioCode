#include "board.h"

extern __IO uint32_t receiveLength;  // HJI
extern __IO uint32_t packetSent;     // HJI
volatile TouchCoords touch_coords;

uint8_t rx_buffer[RX_BUFFER_LENGTH];
uint8_t rx_counter;

#define TX
//#define RX
//#define VCP_TEST

#ifdef TX
#define VIDEO
#endif


int main(void)
{
  //set up systick
  SysTick_Config(SystemCoreClock / 1000);
  configureGPIO(); //Just for discovery LEDs, PE8-PE15
  GPIO_SetBits(GPIOE, GPIO_Pin_8);
  
  delay(1000);
  
  #ifdef VIDEO
  rx_counter = 0;
  touch_coords.processed=1;
  touch_coords.p = PEN_UP;
//  SerialPort_Init(115200,1);
  AR1100Init();
  GPIO_SetBits(GPIOE, GPIO_Pin_9);
  TW88_Init();
  GPIO_SetBits(GPIOE, GPIO_Pin_10);
//  TW88_AddOSD_Win(0,0,0x3F,1);
  setupOSD();
  GPIO_SetBits(GPIOE, GPIO_Pin_11);
  #endif
  
  #if defined(TX) || defined(RX)
  configureSi4463();
  startupRF();
  modemInit();
  uint8_t cmdResponse[MAX_RESPONSE_LENGTH];
  uint8_t modem_status_data[2] = {GET_MODEM_STATUS,0x00}; //tx this to read back modem status info, make this a drv_Si function later TODO:
  
  uint8_t txData[12] = {0x56, 0x45, 0x32, 0x4E, 0x50, 0x51, 0, 0, 0, 0, 0, 0}; //six byte VE2NPQ, five bytes servo data 8bitx4=40bit, 1 byte rssi, 12 bytes total
  uint8_t rxData[sizeof(txData)] = {0};
  memcpy(rxData, txData, sizeof(txData));
  uint16_t servos[4] = {1500, 1500, 1000, 1500}; // AETR
  
  //move this to its own function
  uint8_t rxStartData[8] = {START_RX, 0x00, 0x00, 0x00, sizeof(rxData), 0x08, 0x03, 0x08}; //rx - channel0 - start immediately - datalength (2B) - timeout=rearmRX - valid=readymode - invalid=rearmRX
  #endif
  
  Set_System();
  Set_USBClock();
  USB_Interrupts_Config();
  USB_Init();
  
  
  

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
  uint16_t servosRX[4] = {1500, 1500, 1000, 1500};
//  uint32_t servotime = 0;
  uint8_t RXbool = 0;
  uint8_t hostrssi = 0, remoterssi = 0;
  uint32_t lastOSDupdate = 0;
  
  
  while(1)
  {
    // PROCESS TOUCHSCREEN HERE
//    if (touch_coords.p == PEN_UP){
////      touch_coords.processed = 1;
//      GPIO_SetBits(GPIOE, 0x0F00);
//      servos[0] = 1500;
//      servos[1] = 1500;
//      servos[2] = 1000;
//      servos[3] = 1500; // AETR
//      
//        //UPDATE OSD CHARS back to white
//    }
    if (touch_coords.p == PEN_DOWN)
    {
      GPIO_ResetBits(GPIOE, 0x0F00);
      uint16_t x = touch_coords.x;
      uint16_t y = touch_coords.y;
//      GPIO_SetBits(GPIOE, 0x0F00);
      if((x >= 0x0160) && (x <= 0x0280)) { // CENTER COLUMN
        if((y >= 0x0A00) && (y <= 0x0B40)) { // UP ARROW
          servos[ELE] = 1200;
          putOSDchar(0x42, 0x1D, 0x04); // red up arrow
          putOSDchar(0x4C, 0x1E, 0x07); // white down arrow
        } else if((y >= 0x06E0) && (y <= 0x0820)){ // DOWN ARROW
          servos[ELE] = 1800;
          putOSDchar(0x42, 0x1D, 0x07); // white up arrow
          putOSDchar(0x4C, 0x1E, 0x04); // red down arrow
        } else if((y >= 0x0820) && (y <= 0x09D0)){ // RESET CONTROLS
          servos[ELE] = 1500;
          servos[AIL] = 1500;
          servos[RUD] = 1500;
          putOSDchar(0x42, 0x1D, 0x07); // white up arrow
          putOSDchar(0x4C, 0x1E, 0x07); // white down arrow
          putOSDchar(0x45, 0x1B, 0x07); // white left arrow
          putOSDchar(0x49, 0x1C, 0x07); // white right arrow
          putOSDchar(0x54, 0x1B, 0x07); // white rudder left
          putOSDchar(0x58, 0x1C, 0x07); // white rudder right
        }
      }
      
      else if((x >= 0x0001) && (x <= 0x00E0)) { // LEFT COLUMN
        if((y >= 0x02E0) && (y <= 0x0470)) { // RUDDER LEFT
          servos[RUD] = 1200;
          putOSDchar(0x54, 0x1B, 0x04); // red rudder left
          putOSDchar(0x58, 0x1C, 0x07); // white rudder right
        } else if((y >= 0x0820) && (y <= 0x09D0)){ // AILERONS LEFT
          servos[AIL] = 1200;
          putOSDchar(0x45, 0x1B, 0x04); // red left arrow
          putOSDchar(0x49, 0x1C, 0x07); // white right arrow
        }
      }
      
      else if((x >= 0x02F0) && (x <= 0x0390)) { // RIGHT COLUMN
        if((y >= 0x02E0) && (y <= 0x0470)) { // RUDDER RIGHT
          servos[RUD] = 1800;
          putOSDchar(0x54, 0x1B, 0x07); // white rudder left
          putOSDchar(0x58, 0x1C, 0x04); // red rudder right
        } else if((y >= 0x0820) && (y <= 0x09D0)){ // AILERONS RIGHT
          servos[AIL] = 1800;
          putOSDchar(0x45, 0x1B, 0x07); // white left arrow
          putOSDchar(0x49, 0x1C, 0x04); // red right arrow
        }
      }
      
      else if((x >= 0x0D30) && (x <= 0x0E50)) { // THROTTLE COLUMN
        if((y >= 0x0510) && (y <= 0x0740)) { // MOTOR ON
          servos[THR] = 1300;
          putOSDchar(0x59, 0x1D, 0x04); // throttle up red
          putOSDchar(0x5A, 0x1E, 0x07); // throttle down white
        } else if ((y >= 0x02E0) && (y <= 0x0470)) { // MOTOR OFF
          putOSDchar(0x59, 0x1D, 0x07); // throttle up white
          putOSDchar(0x5A, 0x1E, 0x04); // throttle down red
          servos[THR] = 1000;
        }
      }
//      touch_coords.processed = 1;
    } 
    
    // Send data at 50Hz
    // RX state in gaps
    if((lastTX+50) < millis())
    {
      packServos(servos, txData);
      modemTxInit();
      clearInts();
      modemTX(txData, sizeof(txData), 0);
      
      // update LEDs
//      for(int i=0; i<4; i++)
//      {
//        if(servos[i] < 1500)
//          GPIO_ResetBits(GPIOE, GPIO_Pin_8<<i);
//        else
//          GPIO_SetBits(GPIOE, GPIO_Pin_8<<i);
//      }
//      
      // update servos
      //
      // CHANGE THIS TO BE UPDATED FROM TOUCHSCREEN
      //
//      if((servotime+1000) < millis()) // change servos every second
//      {
//        uint16_t newservo = ((servos[0] == 1000) ? 2000 : 1000);
//        for(int i=0; i<4; i++)
//        {
//          servos[i] = newservo;
//        }
//        servotime = millis();
//      }
//      
      // go back to RX mode
      modemRxInit();
      clearInts();
      Si4436_Cmd_Response(rxStartData, sizeof(rxStartData), cmdResponse, 0);
      
      lastTX = millis();
    }
    
    if(GPIO_ReadInputDataBit(nIRQ_GPIO,nIRQ) == Bit_RESET) // wait for RXing int
    {
      Si4436_Cmd_Response(modem_status_data, sizeof(modem_status_data), cmdResponse, 8);
      hostrssi = cmdResponse[2];
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
        remoterssi = rxData[11];
        
        //update ACKed LEDs
//        for(int i=0; i<4; i++)
//        {
//          if(servosRX[i] < 1500)
//            GPIO_ResetBits(GPIOE, GPIO_Pin_8<<(i+4));
//          else
//            GPIO_SetBits(GPIOE, GPIO_Pin_8<<(i+4));
//        }
        
      }
      RXbool = 1;
    }
    
    // UPDATE OSD HERE
    // feed RSSI data into OSD
    // only every second because i2c is fuckin slow
    if((lastOSDupdate+1000) < millis()) {
    putOSDrssi(0, (hostrssi/2)-130); // r=0 host, r=1 remote
    putOSDrssi(1,(remoterssi/2)-130); // r=0 host, r=1 remote
      lastOSDupdate = millis();
    }
    

  }
  #endif

  /** RX ***************************/
  #ifdef RX
  
  // enter RX mode
  modemRxInit();
  clearInts();
  Si4436_Cmd_Response(rxStartData, sizeof(rxStartData), cmdResponse, 0);
  
  uint8_t rssi = 0;
  uint8_t RXbool = 0;
  uint32_t ResponseNeeded = 0;

  while (1)
  {
    
    if(GPIO_ReadInputDataBit(nIRQ_GPIO,nIRQ) == Bit_RESET) // wait for int (going low)
      {
      Si4436_Cmd_Response(modem_status_data, sizeof(modem_status_data), cmdResponse, 8);
      rssi = cmdResponse[2];
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
        setPWM(servos);
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
      txData[11] = rssi;
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
  Shutdown (not used)
  **********************************/
  
  #if defined(TX) || defined(RX)
//  shutdownRF();
  #endif
  
  #ifdef TX
//  GPIOE->ODR |= 0xFF00; //all eight LEDs on to indicate thread complete
  #endif
  

}
