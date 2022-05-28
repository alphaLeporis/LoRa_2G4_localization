/*******************************************************************************************************
  LoRa 2.4GHz Localization - Copyright of the author Louis de Looze - 15/03/22

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.

  This is the code that runs on the fixed anchors. The only thing it does is reply to ranging requests.

  IMPORTANT: This code uses a modified version of the SX12XX-LoRa library from Stuart Robinson. 
  Code can be found in the "ranging_fix" folder!

  ** IMPROVEMENTS **
  TODO: Add debug flag to Serial.
  TODO: Add the possibility to change parameters on the fly.
  TODO: Add the possibility to set ranging calibration.
  
*******************************************************************************************************/

#include <SPI.h>
#include <SX128XLT.h>
#include "settings.h"

SX128XLT LT;

uint16_t IrqStatus;

#define RXBUFFER_SIZE 255
uint8_t RXBUFFER[RXBUFFER_SIZE];

void setup()
{
  Serial.begin(115200);
  Serial.println("[SYSTEM] Anchor is booting... ");
  Serial.print("[SYSTEM] Has ID: ");
  Serial.println(RangingAddress);

  pinMode(LED1, OUTPUT);
  led_Flash(2, 125);

  SPI.begin();

  if (LT.begin(NSS, NRESET, RFBUSY, DIO1, LORA_DEVICE))
  {
    Serial.println(F("[LORA] Device found"));
    led_Flash(2, 125);
    delay(1000);
  }
  else
  {
    Serial.println(F("[LORA] No device responding"));
    while (1)
    {
      led_Flash(50, 50);                                 //long fast speed flash indicates device error
    }
  }

  LT.setupRanging(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate, RangingAddress, RANGING_SLAVE);

  LT.setRangingCalibration(Calibration);

  Serial.print(F("[LORA] Calibration set to: "));
  Serial.println(LT.getSetCalibrationValue());           //reads the calibratuion value currently set
  delay(2000);
  Serial.println("[SYSTEM] Setup is done! ");
}

void loop()
{
  LT.receiveRangingOrData(RXBUFFER, RXBUFFER_SIZE, RangingAddress, 60000, TXpower, WAIT_RX);
  IrqStatus = LT.readIrqStatus();
    digitalWrite(LED1, HIGH);

    if (IrqStatus & IRQ_RANGING_SLAVE_RESPONSE_DONE)
    {
      Serial.print("[RANGING] Response sent!");
    }
    else
    {
      Serial.print("[RANGING] Slave error - ");
      Serial.print("IRQ: ");
      Serial.print(IrqStatus, HEX);
      LT.printIrqStatus();
    }
    Serial.println();
  

    digitalWrite(LED1, LOW);
  

}

void led_Flash(unsigned int flashes, unsigned int delaymS)
{
  //flash LED to show board is alive
  unsigned int index;

  for (index = 1; index <= flashes; index++)
  {
    digitalWrite(LED1, HIGH);
    delay(delaymS);
    digitalWrite(LED1, LOW);
    delay(delaymS);
  }
}
