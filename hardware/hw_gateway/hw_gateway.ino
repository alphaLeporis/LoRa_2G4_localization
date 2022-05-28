/*******************************************************************************************************
  LoRa 2.4GHz Localization - Copyright of the author Louis de Looze - 15/03/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.

  This code runs on the gateway. It will send the data back to a Redis server for further processing.

  ** IMPROVEMENTS **
  TODO: Get online anchors with a GET request from the database on boot.
  
*******************************************************************************************************/
#include <SPI.h>
#include <SX128XLT.h>

#include <Redis.h>
#include <ArduinoJson.h>
#include <WiFiClient.h>
#include <WiFi.h>
#include "settings.h"

void publishToRedis(void * parameter);

SX128XLT LT;

StaticJsonDocument<192> doc;

struct dataPacket datapacket;
struct dataPacket hellopacket;
WiFiClient redisConn;
Redis *gRedis = nullptr;

uint8_t onlineAnchors[5];
uint8_t TXPacketL;
uint8_t PacketOK;



void setup() {
  // Serial port
  #ifdef DEBUG
  Serial.begin(115200);
  Serial.println("LoRa anchor booting ");
  pinMode(LED1, OUTPUT);
  #endif
  
//*****************************************************************************************
// Setting up WiFi
//*****************************************************************************************
  #ifdef DEBUG
  Serial.print("Connecting to ");
  Serial.println("wifi");
  #endif
  WiFi.begin(ssid,password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
  }
  #ifdef DEBUG
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  #endif

//*****************************************************************************************
// Setting up Redis
//*****************************************************************************************
  if (!redisConn.connect(REDIS_ADDR, REDIS_PORT)) {
    Serial.println("Failed to connect to the Redis server!");
    return;
  } else {
    gRedis = new Redis(redisConn);
    auto connRet = gRedis->authenticate(REDIS_PASSWORD);
    if (connRet == RedisSuccess) {
        Serial.printf("Connected to the Redis server at %s!\n", REDIS_ADDR);
        auto listeners = gRedis->publish("nodes", "6_online");
        digitalWrite(LED1, HIGH);
    }
    else {
        Serial.printf("Failed to authenticate to the Redis server! Errno: %d\n", (int)connRet);
        while (1);
    }
  }

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
  //LT.setupRanging(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate, RangingAddress, RANGING_MASTER);


//*****************************************************************************************
// Getting online anchors (Webhook)
//*****************************************************************************************
  hellopacket.packettype = 2;
  hellopacket.anchor_id = 4;
  hellopacket.payload[0] = 6;
  hellopacket.payload[1] = 7;
  hellopacket.payload[2] = 9;
  hellopacket.payload[3] = 10;

  #ifdef DEBUG
  Serial.println("[GW] Setup is done!");
  #endif
}

void loop() {
  PacketOK = LT.receiveReliable((uint8_t *) &datapacket, sizeof(datapacket), NetworkID, RXtimeout, WAIT_RX);

  if (PacketOK > 0)
  {
    if (datapacket.packettype == 0) {
      TXPacketL = LT.transmitReliable((uint8_t *) &hellopacket, sizeof(hellopacket), NetworkID, TXtimeout, TXpower, WAIT_TX);
      
      if (TXPacketL > 0)
      {
        Serial.println("Hello packet sent!");
      }
      else
      {
        Serial.println("Hello packet failed!");
      }
      return;
    }

    if (datapacket.packettype == deviceID)
    {
      // Packet is localization data! -> Sending over Redis
      Serial.println(F("Legit localization packet received!"));

      doc["id"] = datapacket.id;
      doc["node"] = datapacket.node_id;
      doc["anchor"] = datapacket.anchor_id;

      JsonArray data = doc.createNestedArray("data");
      data.add(datapacket.payload[0]);
      data.add(datapacket.payload[1]);
      data.add(datapacket.payload[2]);
      data.add(datapacket.payload[3]);
      data.add(datapacket.payload[4]);

      // sending webhook
      char Buf[192];
      serializeJson(doc, Buf);
      //Serial.printf("Sending JSON payload:\n\t'%s'\n", Buf);
      xTaskCreate(
          publishToRedis,    // Function that should be called
          "Upload to Redis",  // Name of the task (for debugging)
          5000,            // Stack size (bytes)
          &Buf,            // Parameter to pass
          1,               // Task priority
          NULL             // Task handle
      );
       doc.clear();
      return;
    } 
  }
}


void publishToRedis(void * parameter){
  Serial.printf("Sending JSON payload:\n\t'%s'\n", (const char *) parameter);
  auto listeners = gRedis->publish("loc-data", (const char *) parameter);
  vTaskDelete(NULL);
}
