
#pragma once

#define SPI1_NSS  GPIO_Pin_4
#define SPI1_SCK  GPIO_Pin_5
#define SPI1_MISO GPIO_Pin_6
#define SPI1_MOSI GPIO_Pin_7
#define SPI1_GPIO GPIOA

void configureSPI(void);
