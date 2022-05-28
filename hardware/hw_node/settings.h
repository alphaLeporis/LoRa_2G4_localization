/*******************************************************************************************************
  LoRa 2.4GHz Localization - Copyright of the author Louis de Looze - 15/03/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
  -- settings.h
*******************************************************************************************************/

//*******  General Settings ***************
//#define MASTER;
#define DEBUG
#define deviceID 1 // CHANGE THIS
//#define networkID 0x3210
const uint16_t networkID = 0x3210;

//*******  Hardware Pin Definitions ***************
#define NSS 16
#define RFBUSY 15
#define NRESET 32
#define DIO1 33
#define LED1 13
#define LORA_DEVICE DEVICE_SX1280 

//*******  LoRa Modem Parameters ***************
const uint32_t Frequency = 2445000000;           //frequency of transmissions in hz
const int32_t Offset = 0;                        //offset frequency in hz for calibration purposes
const uint8_t Bandwidth = LORA_BW_0800;          //LoRa bandwidth
const uint8_t SpreadingFactor = LORA_SF8;        //LoRa spreading factor
const uint8_t CodeRate = LORA_CR_4_5;            //LoRa coding rate
const uint16_t Calibration = 11350;              //Manual Ranging calibrarion value

#define TXpower 10
#define TXtimeout 5000
#define RXtimeout 10000

float distance_adjustment = 1.0000; 
const int8_t RangingTXPower = 10;


//*******  Data type ***************
struct dataPacket
{
  uint8_t packettype;
  uint8_t node_id;
  uint8_t anchor_id;
  int32_t payload[5];
  uint8_t id;  
} __attribute__((packed, aligned(1))); 
