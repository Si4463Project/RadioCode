
#ifndef DRV_SYSTEM_H
#define DRV_SYSTEM_H

void SysTick_Handler(void);
void delayMicroseconds(uint32_t us);
void delay(uint32_t ms);
uint32_t micros(void);
uint32_t millis(void);

static volatile uint32_t sysTickUptime = 0;

#endif
