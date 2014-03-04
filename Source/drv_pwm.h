
void pwmOutputInit(void);
void setPWM(uint16_t * pwmArray); //feed this servo values in milliseconds
void packServos(uint16_t * servos, uint8_t * pack);
void unpackServos(uint16_t * servos, uint8_t * pack);
