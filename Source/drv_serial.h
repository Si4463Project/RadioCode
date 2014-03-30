#ifndef DRV_SERIAL_H
#define DRV_SERIAL_H

void SerialPort_Init(uint32_t speed, uint8_t enable_recvirq);
uint8_t USART2_ReadChar(void);

#endif
