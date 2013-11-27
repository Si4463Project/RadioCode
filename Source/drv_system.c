
#include "board.h"

void SysTick_Handler(void)
{
    sysTickUptime++;
}

void delayMicroseconds(uint32_t us)
{
    uint32_t now = micros();

    while (micros() - now < us);
}

void delay(uint32_t ms)
{
    while (ms--)
        delayMicroseconds(1000);
}

// Return system uptime in microseconds (rollover in 70minutes)
uint32_t micros(void)
{
    register uint32_t ms, cycle_cnt;

    do
    {
        ms = sysTickUptime;
        cycle_cnt = SysTick->VAL;
    }
    while (ms != sysTickUptime);

    return (ms * 1000) + ((SystemCoreClock / 1000) - cycle_cnt) / (SystemCoreClock / 1000000);
}

// Return system uptime in milliseconds (rollover in 49 days)
uint32_t millis(void)
{
    return sysTickUptime;
}
