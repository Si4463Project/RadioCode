
#include "board.h"

void configureSi4463(void)
{
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

//  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef SPI_InitStruct;
  SPI_I2S_DeInit(SPI1);

// Init SPI pins
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7; // SCK,MISO,MOSI
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(SPI1_GPIO, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; // NSS
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(SPI1_GPIO, GPIO_PinSource5, GPIO_AF_5); //SCK
  GPIO_PinAFConfig(SPI1_GPIO, GPIO_PinSource6, GPIO_AF_5); //MISO
  GPIO_PinAFConfig(SPI1_GPIO, GPIO_PinSource7, GPIO_AF_5); //MOSI
  
  
// Init SPI
  SPI_I2S_DeInit(SPI1);
  SPI_StructInit(&SPI_InitStruct);
  SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; //9MHz
  SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
  SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
  SPI_Init(SPI1, &SPI_InitStruct);
  SPI_RxFIFOThresholdConfig(SPI1, SPI_RxFIFOThreshold_QF);
  SPI_Cmd(SPI1, ENABLE);
  
// Init other GPIOs
  
  //nIRQ        - PA4 pin14 - input
  //gpio1 (CTS) - PA3 pin13 - input
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  //SDN         - PB1 pin19 - output
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  //TODO: NVIC for nIRQ
  
  spi1_sdnmode_enable(); //shut down Si4463 by default
    
  spi1_NSS_disable();
}

void spi1_NSS_enable(void)
{
  GPIO_ResetBits(NSS_GPIO, NSS);
}

void spi1_NSS_disable(void)
{
  GPIO_SetBits(NSS_GPIO, NSS);
}

void spi1_sdnmode_enable(void) //pin high, shut down Si4463
{
  GPIO_SetBits(SDN_GPIO, SDN);
}

void spi1_sdnmode_disable(void) //pin low, start up Si4463
{
  GPIO_ResetBits(SDN_GPIO, SDN);
}

uint8_t spi1Transfer(uint8_t send)
{
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
  SPI_SendData8(SPI1, send);
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
  return SPI_ReceiveData8(SPI1);
}

void Si4436_Cmd_Response(uint8_t *Tx, uint8_t txLength, uint8_t *Rx, uint8_t rxLength)
{
  uint8_t thisbyte;
  
  //Wait until CTS pin goes high, and Si4463 is ready to respond to commands.
  //Hopefully this doesn't stay here forever, maybe add a timeout. Some commands (like POWER_UP)
  //can take a few milliseconds. TODO
  while(!GPIO_ReadInputDataBit(CTS_GPIO,CTS))
  {
//    getFRR();
//    delayMicroseconds(100);
  }
  
  spi1_NSS_enable();
  for(int i=0; i < txLength; i++)
  {
    thisbyte = Tx[i];
    spi1Transfer(thisbyte);
  }
  spi1_NSS_disable();
  
  //wait for CTS again
  while(!GPIO_ReadInputDataBit(CTS_GPIO,CTS))
  {
//    getFRR();
//    delayMicroseconds(100);
  }
  
  spi1_NSS_enable();
  spi1Transfer(READ_CMD_BUFF);
  spi1Transfer(0x00); //first byte always 0xFF, discard
  for(int i=0; i< rxLength; i++)
  {
    Rx[i] = spi1Transfer(0x00);
  }
  spi1_NSS_disable();
  delayMicroseconds(10);
}

void Si4436_set_property(uint8_t group, uint8_t property, uint8_t * data, uint8_t dataLength)
{
  uint8_t txData[dataLength + 4];
  uint8_t rxData;
  txData[0] = SET_PROPERTY;
  txData[1] = group;
  txData[2] = dataLength;
  txData[3] = property;
  for(uint8_t i = 0; i<dataLength;i++)
  {
    txData[4+i] = data[i];
  }
  Si4436_Cmd_Response(txData, sizeof(txData), &rxData, 0);
}

void Si4436_get_property(uint8_t group, uint8_t property, uint8_t * rxData, uint8_t dataLength)
{
  uint8_t txData[4];
  txData[0] = GET_PROPERTY;
  txData[1] = group;
  txData[2] = dataLength;
  txData[3] = property;
  Si4436_Cmd_Response(txData, sizeof(txData), rxData, dataLength);
}

uint32_t getFRR(void)
{
  uint32_t returnval;
  
  spi1_NSS_enable();

  spi1Transfer(FRR_A_READ);
  for(uint8_t i = 0; i < 4; i++)
  {
    returnval |= (uint32_t)(spi1Transfer(0x00));
    returnval = returnval << 8;
  }
  
  spi1_NSS_disable();
  
  return returnval;
}

void loadTxFifo(uint8_t *txData, uint8_t dataLength)
{
  spi1_NSS_enable();
  spi1Transfer(WRITE_TX_FIFO);
  for(uint8_t i = 0; i < dataLength; i++)
  {
    spi1Transfer(txData[i]);
  }
  spi1_NSS_disable();
}

void clearInts(void)
{
  uint8_t send[GET_INT_STATUS_tx_length] = {GET_INT_STATUS, 0x00, 0x00, 0x00};
  uint8_t rx[GET_INT_STATUS_rx_length-1];
  Si4436_Cmd_Response(send, GET_INT_STATUS_tx_length, rx, GET_INT_STATUS_rx_length-1);
}
