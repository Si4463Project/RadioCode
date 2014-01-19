
#pragma once

#define NSS       GPIO_Pin_0
#define NSS_GPIO  GPIOB
#define SPI1_SCK  GPIO_Pin_5
#define SPI1_MISO GPIO_Pin_6 //SDO
#define SPI1_MOSI GPIO_Pin_7 //SDI
#define SPI1_GPIO GPIOA
#define CTS       GPIO_Pin_3
#define CTS_GPIO  GPIOA
#define SDN       GPIO_Pin_1
#define SDN_GPIO  GPIOB
#define nIRQ      GPIO_Pin_4
#define nIRQ_GPIO GPIOA

void configureSi4463(void);
void spi1_NSS_enable(void);
void spi1_NSS_disable(void);
void spi1_sdnmode_enable(void);
void spi1_sdnmode_disable(void);
uint8_t spi1Transfer(uint8_t send);
void Si4436_Cmd_Response(uint8_t *Tx, uint8_t txLength, uint8_t *Rx, uint8_t rxLength);
void Si4436_set_property(uint8_t group, uint8_t property, uint8_t * data, uint8_t dataLength);
void Si4436_get_property(uint8_t group, uint8_t property, uint8_t * rxData, uint8_t dataLength);
uint32_t getAllFRRs(void);
uint8_t getFRR(uint8_t);
void loadTxFifo(uint8_t *txData, uint8_t dataLength);
void readRxFifo(uint8_t *rxData, uint8_t dataLength);
void clearInts(void);

/*** Si4463 States ***/
#define SLEEP_MODE                      1
#define READY_MODE                      3
#define TX_MODE                         7
#define RX_MODE                         8

/*** Si4463 commands ***/
//BOOT COMMANDS
#define POWER_UP                        0x02
#define POWER_UP_tx_length              7
#define POWER_UP_rx_length              1
//COMMON COMMANDS
#define PART_INFO                       0x01
#define PART_INFO_tx_length             1
#define PART_INFO_rx_length             9
#define FUNC_INFO                       0x10
#define FUNC_INFO_tx_length             1
#define FUNC_INFO_rx_length             7
#define SET_PROPERTY                    0x11
#define SET_PROPERTY_tx_length          16
#define SET_PROPERTY_rx_length          1
#define GET_PROPERTY                    0x12
#define GET_PROPERTY_tx_length          4
#define GET_PROPERTY_rx_length          17
#define GPIO_PIN_CFG                    0x13
#define GPIO_PIN_CFG_tx_length          8
#define GPIO_PIN_CFG_rx_length          8
#define FIFO_INFO                       0x15
#define FIFO_INFO_tx_length             2
#define FIFO_INFO_rx_length             3
#define GET_INT_STATUS                  0x20
#define GET_INT_STATUS_tx_length        4
#define GET_INT_STATUS_rx_length        9
#define REQUEST_DEVICE_STATE            0x33
#define REQUEST_DEVICE_STATE_tx_length  1
#define REQUEST_DEVICE_STATE_rx_length  3
#define CHANGE_STATE                    0x34
#define CHANGE_STATE_tx_length          2
#define CHANGE_STATE_rx_length          1
#define READ_CMD_BUFF                   0x44
#define READ_CMD_BUFF_tx_length         1
#define READ_CMD_BUFF_rx_length         17
#define FRR_A_READ                      0x50
#define FRR_A_READ_tx_length            1
#define FRR_A_READ_rx_length            4
#define FRR_B_READ                      0x51
#define FRR_B_READ_tx_length            1
#define FRR_B_READ_rx_length            4
#define FRR_C_READ                      0x53
#define FRR_C_READ_tx_length            1
#define FRR_C_READ_rx_length            4
#define FRR_D_READ                      0x57
#define FRR_D_READ_tx_length            1
#define FRR_D_READ_rx_length            4
//IR CAL COMMANDS
#define IRCAL                           0x17
//TX COMMANDS
#define START_TX                        0x31
#define WRITE_TX_FIFO                   0x66
//RX COMMANDS
#define PACKET_INFO                     0x16
#define GET_MODEM_STATUS                0x22
#define START_RX                        0x32
#define RX_HOP                          0x36
#define READ_RX_FIFO                    0x77
//ADVANCED COMMANDS
#define GET_ADC_READING                 0x14
#define PROTOCOL_CFG                    0x18
#define GET_PH_STATUS                   0x21
#define GET_CHIP_STATUS                 0x23

#define MAX_RESPONSE_LENGTH             17

/*** Si4463 properties ***/
#define GLOBAL_GROUP                    0x00
#define GLOBAL_XO_TUNE                  0x00
#define GLOBAL_CLK_CFG                  0x01
#define GLOBAL_LOW_BATT_THRESH          0x02
#define GLOBAL_CONFIG                   0x03
#define GLOBAL_WUT_CONFIG               0x04
#define GLOBAL_WUT_M                    0x05
#define GLOBAL_WUT_R                    0x06
#define GLOBAL_WUT_LDC                  0x07
#define GLOBAL_WUT_CAL                  0x08

#define INT_CTL_GROUP                   0x01
#define INT_CTL_ENABLE                  0x00
#define INT_CTL_PH_ENABLE               0x01
#define INT_CTL_MODEM_ENABLE            0x02
#define INT_CTL_CHIP_ENABLE             0x03

#define FRR_CTL_GROUP                   0x02
#define FRR_CTL_A_MODE                  0x00
#define FRR_CTL_B_MODE                  0x01
#define FRR_CTL_C_MODE                  0x02
#define FRR_CTL_D_MODE                  0x03

#define PREAMBLE_GROUP                  0x10
#define PREAMBLE_TX_LENGTH              0x00
#define PREAMBLE_CONFIG_STD_1           0x01
#define PREAMBLE_CONFIG_NSTD            0x02
#define PREAMBLE_CONFIG_STD_2           0x03
#define PREAMBLE_CONFIG                 0x04
#define PREAMBLE_PATTERN                0x05
#define PREAMBLE_POSTAMBLE_CONFIG       0x09
#define PREAMBLE_POSTAMBLE_PATTERN      0x0A

#define SYNC_GROUP                      0x11
#define SYNC_CONFIG                     0x00
#define SYNC_BITS

#define PKT_GROUP                       0x12
#define PKT_CRC_CONFIG
#define PKT_WHT_POLY
#define PKT_WHT_SEED
#define PKT_WHT_BIT_NUM
#define PKT_CONFIG1                     0x06
#define PKT_LEN
#define PKT_LEN_FIELD_SOURCE
#define PKT_LEN_ADJUST
#define PKT_TX_THRESHOLD                0x0B
#define PKT_RX_THRESHOLD                0x0C
#define PKT_FIELD_1_LENGTH
#define PKT_FIELD_1_CONFIG              0x0F
#define PKT_FIELD_1_CRC_CONFIG
#define PKT_FIELD_2_LENGTH
#define PKT_FIELD_2_CONFIG
#define PKT_FIELD_2_CRC_CONFIG
#define PKT_FIELD_3_LENGTH
#define PKT_FIELD_3_CONFIG
#define PKT_FIELD_3_CRC_CONFIG
#define PKT_FIELD_4_LENGTH
#define PKT_FIELD_4_CONFIG
#define PKT_FIELD_4_CRC_CONFIG
#define PKT_FIELD_5_LENGTH
#define PKT_FIELD_5_CONFIG
#define PKT_FIELD_5_CRC_CONFIG
#define PKT_RX_FIELD_1_LENGTH
#define PKT_RX_FIELD_1_CONFIG           0x23
#define PKT_RX_FIELD_1_CRC_CONFIG
#define PKT_RX_FIELD_2_LENGTH
#define PKT_RX_FIELD_2_CONFIG
#define PKT_RX_FIELD_2_CRC_CONFIG
#define PKT_RX_FIELD_3_LENGTH
#define PKT_RX_FIELD_3_CONFIG
#define PKT_RX_FIELD_3_CRC_CONFIG
#define PKT_RX_FIELD_4_LENGTH
#define PKT_RX_FIELD_4_CONFIG
#define PKT_RX_FIELD_4_CRC_CONFIG
#define PKT_RX_FIELD_5_LENGTH
#define PKT_RX_FIELD_5_CONFIG
#define PKT_RX_FIELD_5_CRC_CONFIG

#define MODEM_GROUP                     0x20
#define MODEM_MOD_TYPE                  0x00
#define MODEM_MAP_CONTROL               0x01
#define MODEM_DSM_CTRL
#define MODEM_DATA_RATE                 0x03
#define MODEM_TX_NCO_MODE               0x06
#define MODEM_FREQ_DEV                  0x0A
#define MODEM_FREQ_OFFSET
#define MODEM_TX_FILTER_COEFF_8
#define MODEM_TX_FILTER_COEFF_7
#define MODEM_TX_FILTER_COEFF_6
#define MODEM_TX_FILTER_COEFF_5
#define MODEM_TX_FILTER_COEFF_4
#define MODEM_TX_FILTER_COEFF_3
#define MODEM_TX_FILTER_COEFF_2
#define MODEM_TX_FILTER_COEFF_1
#define MODEM_TX_FILTER_COEFF_0
#define MODEM_TX_RAMP_DELAY
#define MODEM_MDM_CTRL                  0x19
#define MODEM_IF_CONTROL
#define MODEM_IF_FREQ                   0x1B
#define MODEM_DECIMATION_CFG1
#define MODEM_DECIMATION_CFG0
#define MODEM_BCR_OSR                   0x22
#define MODEM_BCR_NCO_OFFSET
#define MODEM_BCR_GAIN
#define MODEM_BCR_GEAR
#define MODEM_BCR_MISC1
#define MODEM_BCR_MISC0
#define MODEM_AFC_GEAR                  0x2C
#define MODEM_AFC_WAIT
#define MODEM_AFC_GAIN
#define MODEM_AFC_LIMITER
#define MODEM_AFC_MISC
#define MODEM_AFC_ZIFOFF
#define MODEM_ADC_CTRL
#define MODEM_AGC_CONTROL               0x35
#define MODEM_AGC_WINDOW_SIZE           0x38
#define MODEM_AGC_RFPD_DECAY
#define MODEM_AGC_IFPD_DECAY
#define MODEM_FSK4_GAIN1                0x3B
#define MODEM_FSK4_GAIN0                0x3C
#define MODEM_FSK4_TH                   0x3D
#define MODEM_FSK4_MAP                  0x3F
#define MODEM_OOK_PDTC
#define MODEM_OOK_BLOPK
#define MODEM_OOK_CNT1
#define MODEM_OOK_MISC
#define MODEM_RAW_SEARCH                0x44
#define MODEM_RAW_CONTROL
#define MODEM_RAW_EYE
#define MODEM_ANT_DIV_MODE
#define MODEM_ANT_DIV_CONTROL
#define MODEM_RSSI_THRESH               0x4A
#define MODEM_RSSI_JUMP_THRESH
#define MODEM_RSSI_CONTROL              0x4C
#define MODEM_RSSI_CONTROL2
#define MODEM_RSSI_COMP
#define MODEM_CLKGEN_BAND               0x51

#define MODEM_CHFLT_GROUP               0x21
#define MODEM_CHFLT_RX1_CHFLT_COE       0x00
#define MODEM_CHFLT_RX2_CHFLT_COE       0x12

#define PA_GROUP                        0x22
#define PA_MODE
#define PA_PWR_LVL                      0x01
#define PA_BIAS_CLKDUTY
#define PA_TC
#define PA_RAMP_EX
#define PA_RAMP_DOWN_DELAY

#define SYNTH_GROUP                     0x23
#define SYNTH_PFDCP_CPFF
#define SYNTH_PFDCP_CPINT
#define SYNTH_VCO_KV
#define SYNTH_LPFILT3
#define SYNTH_LPFILT2
#define SYNTH_LPFILT1
#define SYNTH_LPFILT0
#define SYNTH_VCO_KVCAL

#define MATCH_GROUP                     0x30
#define MATCH_VALUE_1
#define MATCH_MASK_1
#define MATCH_CTRL_1
#define MATCH_VALUE_2
#define MATCH_MASK_2
#define MATCH_CTRL_2
#define MATCH_VALUE_3
#define MATCH_MASK_3
#define MATCH_CTRL_3
#define MATCH_VALUE_4
#define MATCH_MASK_4
#define MATCH_CTRL_4

#define FREQ_CONTROL_GROUP              0x40
#define FREQ_CONTROL_INTE               0x00
#define FREQ_CONTROL_FRAC               0x01
#define FREQ_CONTROL_CHANNEL_STEP_SIZE  0x04
#define FREQ_CONTROL_W_SIZE             0x06
#define FREQ_CONTROL_VCOCNT_RX_ADJ      0x07

#define RX_HOP_GROUP                    0x50
#define RX_HOP_CONTROL                  0x00
#define RX_HOP_TABLE_SIZE               0x01
#define RX_HOP_TABLE_ENTRY              0x02
