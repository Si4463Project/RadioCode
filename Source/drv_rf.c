
#include "board.h"

uint8_t cmdResponse[MAX_RESPONSE_LENGTH];

//Start up TX command.
//Normal mode, Non TCXO, 30MHz crystal.
uint8_t startUpTx[POWER_UP_tx_length] = { POWER_UP, 0x01, 0x00, 0x01, 0xC9, 0xC3, 0x80 };

//struct bind_data {
//  uint32_t basefreq;
//} bind_data;

//struct radio_data {
//  uint8_t modem_modulation; // 0b00000101, packet mode, 4GFSK
//  uint8_t modem_baudrate[3];
//} radio_data;

void startupRF(void)
{
  delay(10); //TODO: this is juts for debugging, to make sure Si shuts down clean
  spi1_sdnmode_disable(); //start up RF
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
  //external oscillator
  //'SET_PROPERTY' 'GLOBAL_XO_TUNE' 52
  uint8_t global_xo_tune[1] = {120};
  Si4436_set_property(GLOBAL_GROUP, GLOBAL_XO_TUNE, global_xo_tune, 1);
  
  //set FRRs
  //A default = INT_STATUS
  //B default = INT_PEND
  //C default = CURRENT_STATE
  //D default = disabled
  uint8_t frr_ctl_d_mode[1] = {3}; //INT_PH_STATUS
  Si4436_set_property(FRR_CTL_GROUP, FRR_CTL_D_MODE, frr_ctl_d_mode, 1);
  
  //set TX packet thresh to zero
//  uint8_t pkt_tx_threshold[1] = {0x00};
//  Si4436_set_property(PKT_GROUP, PKT_TX_THRESHOLD, pkt_tx_threshold, 1);
  
  //set nIRQ output conditions
  uint8_t int_ctl_enable[1] = {0x01}; //PH interrupts only
  Si4436_set_property(INT_CTL_GROUP, INT_CTL_ENABLE, int_ctl_enable, 1);
  uint8_t int_ctl_ph_enable[1] = {0x20}; //PACKET_SENT interrupt only
  Si4436_set_property(INT_CTL_GROUP, INT_CTL_PH_ENABLE, int_ctl_ph_enable, 1);
  
  //set power level (very low, for testing)
  uint8_t pa_pwr_lvl[1] = {0x12}; 
  Si4436_set_property(PA_GROUP, PA_PWR_LVL, pa_pwr_lvl, 1);
  
  //440MHz
  uint8_t freq_control_inte[1] = {0x39};
  uint8_t freq_control_frac[3] = {0x0D, 0x55, 0x55};
  uint8_t modem_clkgen_band[1] = {0x0A};
  
  Si4436_set_property(FREQ_CONTROL_GROUP, FREQ_CONTROL_INTE, freq_control_inte, 1);
  Si4436_set_property(FREQ_CONTROL_GROUP, FREQ_CONTROL_FRAC, freq_control_frac, 3);
  Si4436_set_property(MODEM_GROUP, MODEM_CLKGEN_BAND, modem_clkgen_band, 1);

  //4GFSK, 28.8ksps, 20kHz innerdev
  uint8_t modem_mod_type[1] = {0x05};
  uint8_t modem_data_rate[3] = {0x08, 0xCA, 0x00};
  uint8_t modem_tx_nco_mode[4] = {0x09, 0xC9, 0xC3, 0x80};
  uint8_t modem_freq_dev[3] = {0x00, 0x10, 0x62};
  uint8_t modem_fsk4_gain1[5] = {0x00, 0x1A, 0x58, 0xE4, 0x00};
  uint8_t pkt_field_1_config[1] = {0x10}; //need this for 4GFSK
  uint8_t pkt_config1[1] = {0x20}; //need this for 4GFSK
  
  Si4436_set_property(MODEM_GROUP, MODEM_MOD_TYPE, modem_mod_type, 1);
  Si4436_set_property(MODEM_GROUP, MODEM_DATA_RATE, modem_data_rate, 3);
  Si4436_set_property(MODEM_GROUP, MODEM_TX_NCO_MODE, modem_tx_nco_mode, 4);
  Si4436_set_property(MODEM_GROUP, MODEM_FREQ_DEV, modem_freq_dev, 3);
  Si4436_set_property(MODEM_GROUP, MODEM_FSK4_GAIN1, modem_fsk4_gain1, 5);
  Si4436_set_property(PKT_GROUP, PKT_FIELD_1_CONFIG, pkt_field_1_config, 1);
  Si4436_set_property(PKT_GROUP, PKT_CONFIG1, pkt_config1, 1);
  
  //disable preamble and sync packets (for testing)
  uint8_t preamble_tx_length[1] = {0x00};
  Si4436_set_property(PREAMBLE_GROUP, PREAMBLE_TX_LENGTH, preamble_tx_length, 1);
  uint8_t sync_config[1] = {0x80};
  Si4436_set_property(SYNC_GROUP, SYNC_CONFIG, sync_config, 1);
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
  uint8_t cmdData[5] = {START_TX, hopchannel, 0x30, 0x00, dataLength}; //TX right away, return to ready mode, pkt length 8bit max
  Si4436_Cmd_Response(cmdData, sizeof(cmdData), cmdResponse, 0);
  
  //wait for TX fifo to empty
  while(GPIO_ReadInputDataBit(nIRQ_GPIO,nIRQ));
}
