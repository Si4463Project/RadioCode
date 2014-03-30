#include "board.h"

extern uint8_t rx_buffer[RX_BUFFER_LENGTH];
extern uint8_t rx_counter;

void USART2_IRQHandler(void)
{
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        /* Read one byte from the receive data register */
        rx_buffer[rx_counter] = (USART_ReceiveData(USART2) & 0x7F);

        if(rx_counter + 1 == RX_BUFFER_LENGTH ||
                rx_buffer[rx_counter] == '\n' || rx_buffer[rx_counter] == '\r')
        {
            printf("%s\n\r", rx_buffer);
            memset(rx_buffer, 0, RX_BUFFER_LENGTH);
            rx_counter = 0;
        }
        else
        {
            rx_counter++;
        }
    }
}


uint8_t USART2_ReadChar()
{
    while(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);
    return USART_ReceiveData(USART2);
}



// PD5 -> TX and PD6 -> RX for USART2

void SerialPort_Init(uint32_t speed, uint8_t enable_recvirq)
{
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable GPIO clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);

    /* Enable USART clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    /* Connect PXx to USARTx_Tx */
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_7);

    /* Connect PXx to USARTx_Rx */
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_7);

    /* Configure USART Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* Configure USART Rx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* USART configuration */
    USART_InitStructure.USART_BaudRate = speed;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);

    if(enable_recvirq)
    {
        USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

        /* Enable the USART2 Interrupt */
        NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
    }

    /* Enable USART */
    USART_Cmd(USART2, ENABLE);	
	
	
}
