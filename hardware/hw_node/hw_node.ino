/*******************************************************************************************************
  LoRa 2.4GHz Localization - Copyright of the author Louis de Looze - 15/03/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.

  This is the code that runs on the mobile nodes. It will do ranging on all online anchros and publish 
  this to the gateway. 

  ** IMPROVEMENTS **
  TODO: Make the system allow more than 4 online anchors -> Allocate space during runtime
  TODO: Make the polling rate variable based on distance difference.
  TODO: Add the possibility to send other telemetry data to the gateway.
  TODO: Add a timeout for a not answering anchor. That it will only contact it again after a cooldown
    period.
  
*******************************************************************************************************/
#include <SPI.h>
#include <SX128XLT.h>
#include "settings.h"

SX128XLT LT;
struct dataPacket datapacket;
struct dataPacket hellopacket;
bool isSetup = true;
uint8_t failedAnchor;
uint16_t IrqStatus;
uint8_t TXPacketL;
uint8_t PacketOK;
int32_t range_result;

uint8_t onlineAnchors[4];


void setup() {
  // Serial port
  #ifdef DEBUG
  Serial.begin(115200);
  Serial.println("LoRa node booting ");
  pinMode(LED1, OUTPUT);
  #endif

//*****************************************************************************************
// Setting up LoRa
//*****************************************************************************************
  SPI.begin();
  if (LT.begin(NSS, NRESET, RFBUSY, DIO1, LORA_DEVICE))
  {
    Serial.println(F("Device found"));
    delay(1000);
  }
  else
  {
    Serial.println(F("No device responding"));
  }
  LT.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate);
}
void loop() {
//*****************************************************************************************
// Setup
//*****************************************************************************************
  if (isSetup) {
    hellopacket.packettype = 0;
    hellopacket.node_id = deviceID;
    TXPacketL = LT.transmitReliable((uint8_t *) &hellopacket, sizeof(hellopacket), networkID, TXtimeout, TXpower, WAIT_TX);
  
    if (TXPacketL > 0)
    {
      Serial.println("Hello packet sent!");
    }
    else
    {
      Serial.println("Hello packet failed!");
      return;
    }
    // Receiving HELLO ACK
      while(1) {
        PacketOK = LT.receiveReliable((uint8_t *) &hellopacket, sizeof(hellopacket), networkID, RXtimeout, WAIT_RX);
        if (PacketOK > 0) {
          Serial.println("Hello answer received!");
           //Packet is HELLO-answer! -> Setup complete
          if (hellopacket.packettype == 2) {     
            Serial.println(F("New HELLO-answer packet received!"));
            //onlineAnchors[hellopacket.anchor_id];
            onlineAnchors[0] = hellopacket.payload[0];
            onlineAnchors[1] = hellopacket.payload[1];
            onlineAnchors[2] = hellopacket.payload[2];
            onlineAnchors[3] = hellopacket.payload[3];
            // Dit afwerken want is hard coded -> detail :)
            isSetup = false;
            break;
        }
      }
    }
  }
  

  for(uint8_t i = 0; i <= 3; i++) {
    failedAnchor = 0;
    Serial.print("Anchor: ");
    Serial.println(onlineAnchors[i]);
    LT.setupRanging(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate, onlineAnchors[i], RANGING_MASTER);
    LT.setRangingCalibration(Calibration);
    
    for (uint8_t index = 0; index <= 4; index++) {
      LT.transmitRanging(onlineAnchors[i], TXtimeout, RangingTXPower, WAIT_TX);
      IrqStatus = LT.readIrqStatus();
      if (IrqStatus & IRQ_RANGING_MASTER_RESULT_VALID) {
        Serial.print(F("Valid"));
        range_result = LT.getRangingResultRegValue(RANGING_RESULT_RAW);
        Serial.print(F(",Register,"));
        Serial.print(range_result);
  
        if (range_result > 800000)
        {
          range_result = 0;
        }
  
        datapacket.payload[index] = LT.getRangingDistance(RANGING_RESULT_RAW, range_result, distance_adjustment);
      } else {
        datapacket.payload[index] = 0;
        failedAnchor++;
        if (failedAnchor > 2) {
          datapacket.payload[index] = 0;
          break;
        }
      }
    }
    if (failedAnchor < 2) {
      LT.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate);
      datapacket.packettype = 1;
      datapacket.anchor_id = onlineAnchors[i];
      datapacket.node_id = deviceID;
      TXPacketL = LT.transmitReliable((uint8_t *) &datapacket, sizeof(datapacket), networkID, TXtimeout, TXpower, WAIT_TX);
      
      if (TXPacketL > 0)
      {
        Serial.println("Loc packet sent!");
      } else {
        Serial.print(F("failed, code "));
      }
    }
      digitalWrite(LED1, LOW);
      delay(100);
    
  }
  delay(100);  
}
