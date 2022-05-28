/*******************************************************************************************************
  LoRa 2.4GHz Localization - Copyright of the author Louis de Looze - 15/03/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
  -- settings.h
*******************************************************************************************************/

//*******  General Settings ***************
#define DEBUG
const uint32_t RangingAddress = 10; // CHANGE THIS

//*******  Hardware Pin Definitions ***************
#define NSS 16
#define RFBUSY 15
#define NRESET 32
#define DIO1 33
#define LED1 13  
#define LORA_DEVICE DEVICE_SX1280

//*******  LoRa Modem Parameters ***************
const uint32_t Frequency = 2445000000;
const int32_t Offset = 0;
const uint8_t Bandwidth = LORA_BW_0800;
const uint8_t SpreadingFactor = LORA_SF8;
const uint8_t CodeRate = LORA_CR_4_5;
const uint16_t Calibration = 11350;

const int8_t TXpower = 10;
const uint16_t  rangingRXTimeoutmS = 10000;
