
#include "board.h"

void configureSPI(void)
{
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

//  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef SPI_InitStruct;
  SPI_I2S_DeInit(SPI1);

// Init pins
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7; // NSS,SCK,MISO,MOSI
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  //GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_5); //NSS
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_5); //SCK
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_5); //MISO
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_5); //MOSI
  
  SPI_StructInit(&SPI_InitStruct);
  SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; //9MHz
  SPI_InitStruct.SPI_DataSize = SPI_DataSize_16b;
  SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
  SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
  SPI_Init(SPI1, &SPI_InitStruct);
  SPI_Cmd(SPI1, ENABLE);
}
