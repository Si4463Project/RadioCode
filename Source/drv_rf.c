
#include "board.h"

uint8_t cmdResponse[MAX_RESPONSE_LENGTH];

void startupRF(void)
{
  delay(10); //TODO: this is juts for debugging, to make sure Si shuts down clean
  spi1_sdnmode_disable(); //start up RF
  //Start up TX command.
  //Normal mode, Non TCXO, 30MHz crystal.
  uint8_t startUpTx[POWER_UP_tx_length] = { POWER_UP, 0x01, 0x00, 0x01, 0xC9, 0xC3, 0x80 };
  Si4436_Cmd_Response(startUpTx, sizeof(startUpTx), cmdResponse, 0);
  
  //TODO: img rejection calibration
  
}

void shutdownRF(void)
{
  spi1_sdnmode_enable(); //shut down RFM
}

uint16_t getRFState(void)
{
  uint8_t cmdData = REQUEST_DEVICE_STATE;
  Si4436_Cmd_Response(&cmdData, sizeof(cmdData), cmdResponse, 2);
  return ((cmdResponse[0]<<8) | (cmdResponse[1]));
}

void setRFState(uint8_t state)
{
  state &= 0x0F;
  uint8_t cmdData[2] = {CHANGE_STATE, state};
  Si4436_Cmd_Response(cmdData, sizeof(cmdData), cmdResponse, 0);
}

void modemInit(void)
{
  Si4436_set_property(GLOBAL_GROUP, 0x00, GLOBAL_SET, sizeof(GLOBAL_SET));
  Si4436_set_property(FRR_CTL_GROUP, 0x00, FRR_CTL_SET, sizeof(FRR_CTL_SET));
  //preamble
  Si4436_set_property(SYNC_GROUP, 0x00, SYNC_SET, sizeof(SYNC_SET));
  //pkt
  //TODO: make these functions like the others
  Si4436_set_property(PKT_GROUP, PKT_CONFIG1, pkt_config1, sizeof(pkt_config1));
  Si4436_set_property(PKT_GROUP, PKT_FIELD_1_CONFIG, pkt_field_1_config, sizeof(pkt_field_1_config));
  
  Si4436_set_property(MODEM_GROUP, MODEM_SET_OFFSET_1A, MODEM_SET_1A, sizeof(MODEM_SET_1A));
  Si4436_set_property(MODEM_GROUP, MODEM_SET_OFFSET_1B, MODEM_SET_1B, sizeof(MODEM_SET_1B));
  Si4436_set_property(MODEM_GROUP, MODEM_SET_OFFSET_1C, MODEM_SET_1C, sizeof(MODEM_SET_1C));
  Si4436_set_property(MODEM_GROUP, MODEM_SET_OFFSET_2A, MODEM_SET_2A, sizeof(MODEM_SET_2A));
  Si4436_set_property(MODEM_GROUP, MODEM_SET_OFFSET_2B, MODEM_SET_2B, sizeof(MODEM_SET_2B));
  Si4436_set_property(MODEM_GROUP, MODEM_SET_OFFSET_3A, MODEM_SET_3A, sizeof(MODEM_SET_3A));
  Si4436_set_property(MODEM_GROUP, MODEM_SET_OFFSET_3B, MODEM_SET_3B, sizeof(MODEM_SET_3B));
  Si4436_set_property(MODEM_GROUP, MODEM_SET_OFFSET_4, MODEM_SET_4, sizeof(MODEM_SET_4));
  Si4436_set_property(MODEM_CHFLT_GROUP, MODEM_CHFLT_RX1_CHFLT_COE, MODEM_CHFLT_RX1_CHFLT_A, sizeof(MODEM_CHFLT_RX1_CHFLT_A));
  Si4436_set_property(MODEM_CHFLT_GROUP, MODEM_CHFLT_RX1_CHFLT_COE, MODEM_CHFLT_RX1_CHFLT_B, sizeof(MODEM_CHFLT_RX1_CHFLT_B));
  Si4436_set_property(MODEM_CHFLT_GROUP, MODEM_CHFLT_RX2_CHFLT_COE, MODEM_CHFLT_RX2_CHFLT_A, sizeof(MODEM_CHFLT_RX2_CHFLT_A));
  Si4436_set_property(MODEM_CHFLT_GROUP, MODEM_CHFLT_RX2_CHFLT_COE, MODEM_CHFLT_RX2_CHFLT_B, sizeof(MODEM_CHFLT_RX2_CHFLT_B));
  
  Si4436_set_property(PA_GROUP, 0x00, PA_SET, sizeof(PA_SET));
  //synth
  //match
  Si4436_set_property(FREQ_CONTROL_GROUP, 0x00, FREQ_CONTROL_SET, sizeof(FREQ_CONTROL_SET));
  //rx hop
  
  // set GPIOs to output data
  // 11 = RX_DATA_CLK
  // 14 = RX_DATA
  // 15 = RX_RAW_DATA
  // 18 = VALID_PREAMBLE
  // 19 = INVALID_PREAMBLE
  // 1A = SYNC_WORD_DETECT
  // 1B = CCA (rssi above modem_rssi_thresh, non-latched)
  // 20 = TX_STATE
  // 21 = RX_STATE
  // GPIO0 = TXEN
  // GPIO1 = CTS (default!)
  // GPIO2 = RXEN
  uint8_t gpio_pin_conf[GPIO_PIN_CFG_tx_length] = {GPIO_PIN_CFG, 0x20, 0x00, 0x21, 0x00, 0x00, 0x00, 0x00}; 
  Si4436_Cmd_Response(gpio_pin_conf, sizeof(gpio_pin_conf), cmdResponse, 0);
  
  //RX stuff
  //long preamble timeout
//  uint8_t preamble_config_std_2[3] = {0xFF, 0x31}; //long timeout, preamble config
//  Si4436_set_property(PREAMBLE_GROUP, PREAMBLE_CONFIG_STD_2, preamble_config_std_2, 2);
}

void modemTxInit(void)
{
  //set nIRQ output conditions
  uint8_t int_ctl_enable[1] = {0x01}; //PH interrupts only
  uint8_t int_ctl_ph_enable[1] = {0x20}; //PACKET_SENT interrupt only
  Si4436_set_property(INT_CTL_GROUP, INT_CTL_ENABLE, int_ctl_enable, 1);
  Si4436_set_property(INT_CTL_GROUP, INT_CTL_PH_ENABLE, int_ctl_ph_enable, 1);
}

void modemRxInit(void)
{
  //set nIRQ output conditions
  
  uint8_t int_ctl_enable[1] = {0x01}; //PH interrupts only
//  uint8_t int_ctl_enable[1] = {0x02}; //MODEM interrupts only
  Si4436_set_property(INT_CTL_GROUP, INT_CTL_ENABLE, int_ctl_enable, 1);
  
  uint8_t int_ctl_ph_enable[1] = {0x10}; //PACKET_RX interrupt only
  Si4436_set_property(INT_CTL_GROUP, INT_CTL_PH_ENABLE, int_ctl_ph_enable, 1);
//  uint8_t int_ctl_modem_enable[1] = {0x02}; //PREAMBLE_DETECT interrupt only
//  uint8_t int_ctl_modem_enable[1] = {0x04}; //INVALID_PREAMBLE interrupt only
//  Si4436_set_property(INT_CTL_GROUP, INT_CTL_MODEM_ENABLE, int_ctl_modem_enable, 1);
}

void modemInitDirect(void)
{
  uint8_t modem_mod_type[1] = {0x08}; //mod type direct, CW, GPIO0
  uint8_t gpio_pin_conf[GPIO_PIN_CFG_tx_length] = {GPIO_PIN_CFG, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; //set GPIO0 to input
  Si4436_set_property(MODEM_GROUP, MODEM_MOD_TYPE, modem_mod_type, 1);
  Si4436_Cmd_Response(gpio_pin_conf, sizeof(gpio_pin_conf), cmdResponse, 0);
}

void modemTX(uint8_t *txData, uint8_t dataLength, uint8_t hopchannel)
{
  loadTxFifo(txData, dataLength);
  clearInts();
  uint8_t cmdData[5] = {START_TX, hopchannel, 0x30, 0x00, dataLength}; //TX right away, return to READY mode, pkt length 8bit=255 max
  Si4436_Cmd_Response(cmdData, sizeof(cmdData), cmdResponse, 0);
  
  //wait for TX fifo to empty
  while(GPIO_ReadInputDataBit(nIRQ_GPIO,nIRQ));
}

void clearFIFOs(void)
{
  //might want to force dropping from TX/RX mode first?
  uint8_t fifo_info[FIFO_INFO_tx_length] = {FIFO_INFO, 0x00}; //read FIFO info first
  Si4436_Cmd_Response(fifo_info, sizeof(fifo_info), cmdResponse, 2);
  uint8_t fifo_clear[FIFO_INFO_tx_length] = {FIFO_INFO, 0x03}; //clear FIFOs
  Si4436_Cmd_Response(fifo_clear, sizeof(fifo_clear), cmdResponse, 2);
}
