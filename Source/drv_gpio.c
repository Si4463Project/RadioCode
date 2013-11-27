
#include "board.h"

uint16_t DMA_outputMask;

void configureGPIO(void)
{
  DMA_outputMask = 0x0100;
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE);
  //RCC_AHBPeriph_DMA1
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Pin = 0xFF00;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  
  GPIO_ResetBits(GPIOE, 0xFF00);
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  
//   DMA_InitTypeDef DMA_InitStructure;
//   DMA_InitStructure.DMA_BufferSize = 1;
//   DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
//   DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
//   DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&DMA_outputMask;
//   DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
//   DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
//   DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
//   DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(GPIOE->ODR);
//   DMA_InitStructure.DMA_PeripheralDataSize = DMA_MemoryDataSize_HalfWord;
//   DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//   DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
//   
//   DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
//   NVIC_InitTypeDef NVIC_InitStructure;
//   NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
//   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//   
//   NVIC_Init(&NVIC_InitStructure);
//   DMA_Init(DMA1_Channel1, &DMA_InitStructure);
//   
//   DMA_Cmd(DMA1_Channel1, ENABLE);
}

// void DMA1_Channel1_IRQHandler(void)
// {
//   if(DMA_GetITStatus(DMA1_IT_TC1))
//   {
//     DMA_outputMask = DMA_outputMask << 1;
//     if (DMA_outputMask == 0x0000)
//       DMA_outputMask = 0x0100;
//     DMA_ClearITPendingBit(DMA1_IT_GL1); //clear all ch1 interrupt bits
//     DMA_Cmd(DMA1_Channel1, ENABLE); //re-enable DMA
//   }
// }
