
#pragma once

void startupRF(void);
void shutdownRF(void);
uint16_t getRFState(void);
void setRFState(uint8_t state);
void modemInit(void);
void modemInitDirect(void);
void modemTX(uint8_t *txData, uint8_t dataLength, uint8_t hopchannel);
