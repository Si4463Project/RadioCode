#include "board.h"
#include <string.h>

static __IO uint8_t touch_counter=0;
static __IO TouchInitStr touchStr;
extern __IO TouchCoords touch_coords;
/*
	If time allows we should use a ring buffer here so we don't loose any touch events

*/
void USART1_IRQHandler(void)
{
		// one pen down event => p=1 then p=0 
	  // one pen up event => p=0 then p=1 then p=0
	  // hence a max of 15 bytes we can read for a given touch event

		
    if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
    {
        /* Read one byte from the receive data register */
        touch_buffer[touch_counter] = (USART_ReceiveData(USART1) & 0x00FF); //make them 8 bits
			
			
				if (touch_counter==9) {
						//ok we got 2x 5 bytes reports check to see what types they are
					  if ((touch_buffer[0]==AR1100_PEN_DOWN_FLAG) && (touch_buffer[5]==AR1100_PEN_UP_FLAG)) {
							  //ok this a real touch down event so we can signal it ! by getting the coords
								touch_coords.p=PEN_DOWN;
								touch_coords.processed=0;
								touch_coords.x=(touch_buffer[2] <<7) | touch_buffer[1];
							  touch_coords.y=(touch_buffer[4] <<7) | touch_buffer[3];
								//now clean up this rx buffer
							  memset((void*)touch_buffer, 0, MAX_BUFFSIZE);
							  touch_counter = 0;
								return;
						} 
				}
				
			
			
        if(touch_counter + 1 == MAX_BUFFSIZE)
        {
						//this has to be a touch up event ! if not ignore it
					  if ((touch_buffer[0]==AR1100_PEN_UP_FLAG) && 
																	(touch_buffer[5]==AR1100_PEN_DOWN_FLAG) &&
																	(touch_buffer[10]==AR1100_PEN_UP_FLAG) ) {
								
								touch_coords.p=PEN_UP;
								touch_coords.processed=0;
								touch_coords.x=(touch_buffer[2] <<7) | touch_buffer[1];
							  touch_coords.y=(touch_buffer[4] <<7) | touch_buffer[3];
												
							
							
						}							
					
					  //now clean up this rx buffer	
            memset((void*)touch_buffer, 0, MAX_BUFFSIZE);
            touch_counter = 0;
        }
        else
        {
            touch_counter++;
        }
    }
		
		if (USART_GetITStatus(USART1, USART_FLAG_ORE) != RESET) {
					USART_ClearITPendingBit(USART1,USART_IT_ORE);
					memset((void*)touch_buffer, 0, MAX_BUFFSIZE);
					touch_counter = 0;			 
					
		}	
		
		
}


void AR1100IRQSetup(FunctionalState newstatus)
{
	USART_ITConfig(USART1, USART_IT_RXNE, newstatus);
}

/*
	Unfortunately writing on the internal control registers doesn't work as expected
	if one tries to write data in the internal registers due to some protocol screwup 
	the AR1100 AKA PIC18 gets stuck

Fix to try : we should wait a bit like 50ms before we attempt to write to the internal registers.
							This idea comes from the other touch controller chips like the AR1010 series
							their datasheet suggests that, however our datasheet doesn't mention that

*/
void AR1100Init(void)
{
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable GPIO clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);	
	
	
    /* Enable USART clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);


	
    /* Connect PXx to USARTx_Tx */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_7);

    /* Connect PXx to USARTx_Rx */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_7);

    /* Configure USART Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure USART Rx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* USART configuration */
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

		AR1100IRQSetup(DISABLE);

		/* Enable the USART1 Interrupt */
		//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		
		
		

    /* Enable USART */
    USART_Cmd(USART1, ENABLE);		
		
		
		//dummy sleep here 
		delay(50);
		
		
		
		//disable any reports
		AR1100TouchDisable();
		delay(50);
			
		//do some dummy reads ! stupid PIC18 internal firmware aparently needs that
			
		//uint8_t buffer[6]={0,0,0,0,0,0};

		//uint8_t sz=AR1100_ReadData(buffer,5);	
		uint8_t regval;
		AR1100_ReadReg(0x14,&regval);
			
		//AR1100_ReadReg(0x0D,&regval);
		//AR1100_WriteReg(0x0D, 0x88);	
		AR1100_WriteReg(0x0C, 0x41); //set the touch mode
		AR1100_ReadReg(0x0C,&regval);	
			
			
		
		AR1100TouchEnable();
		
		AR1100IRQSetup(ENABLE);

	
}

uint8_t AR1100_ReadData(uint8_t *buffer, const uint8_t sz)
{
	if (sz<=0) return 0;  
	uint8_t ch,size_read;
	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET) {}
	ch=(USART_ReceiveData(USART1) & 0x00FF);
	if (ch!=0x55) { 
		 return 0;
	} //sync ?
	
	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET) {}
	
	//now read the size
	size_read=(USART_ReceiveData(USART1) & 0x00FF);
		
	if ((size_read==0) || (size_read>sz)) return 0;

			
	//size 
	for (uint8_t x=0;x<size_read;x++) {
		while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET) {}
		ch=(USART_ReceiveData(USART1) & 0x00FF);
		//now copy it into the buffer
		buffer[x]=ch;	
	}		
	
	return size_read;
	
}


void AR1100TouchEnable(void)
{
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}		
  USART_SendData(USART1, (0x00FF)&0x55);
  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}	
  USART_SendData(USART1, (0x00FF)&0x01);
  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}			
  USART_SendData(USART1,(0x00FF)&0x12);

		
	uint8_t buff[6];
	AR1100_ReadData(buff,6);  
			
}

void AR1100TouchDisable(void)
{
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}
	USART_SendData(USART1, (0x00FF)&0x55);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}
	USART_SendData(USART1, (0x00FF)&0x01);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}	
	USART_SendData(USART1, (0x00FF)&0x13);
		
	//now read the data or we will have overruns
	uint8_t buff[6];
	AR1100_ReadData(buff,6);
		
	
}


void AR1100_WriteReg(uint8_t reg_addr, uint8_t regval)
{
	 while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}
	 USART_SendData(USART1, (0x00FF)&0x55);
	 while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}
	 USART_SendData(USART1, (0x00FF)&0x03); //this is a problem !
	 while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}
	 USART_SendData(USART1, (0x00FF)&0x21);
	 while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}	 
	 USART_SendData(USART1, (0x00FF)&0x00);
	 while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}	 
	 USART_SendData(USART1, (0x00FF)&reg_addr);
	 while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}
	 USART_SendData(USART1, (0x00FF)&regval);


	uint8_t buff[6];
	AR1100_ReadData(buff,6);
		 
}

void AR1100_ReadReg(uint8_t reg_addr, uint8_t *regval)
{
	 while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}	
	 USART_SendData(USART1, (0x00FF)&0x55);
	 while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}	
	 USART_SendData(USART1, (0x00FF)&0x04);
	 while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}
	 USART_SendData(USART1, (0x00FF)&0x20);
	 while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}	
	 USART_SendData(USART1, (0x00FF)&0x00);
	 while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}
	 USART_SendData(USART1, (0x00FF)&reg_addr);
	 while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}		
	 USART_SendData(USART1, (0x00FF)&0x01);	
	 
	 //now read the data
	 
	 //make sure we have something to read !	 
	 while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET) {}
	 
	 uint8_t ch[6]={0,0,0,0,0,0};
		 
	 ch[0]=(USART_ReceiveData(USART1) & 0x00FF);
	 while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET) {}
	 ch[1]=(USART_ReceiveData(USART1) & 0x00FF); 
	 while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET) {}
	 ch[2]=(USART_ReceiveData(USART1) & 0x00FF); 
	 while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET) {}
	 ch[3]=(USART_ReceiveData(USART1) & 0x00FF);
	 while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET) {}
	 ch[4]=(USART_ReceiveData(USART1) & 0x00FF);	 
	 
	 *regval=ch[4];
	 
}

