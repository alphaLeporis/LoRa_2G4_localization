# Fix for the S12XX-LoRa Library

For some weird reason, the SX1280 modules buffer will stay full when in ranging mode and receiving a normal LoRa packet. This results in a non-working anchor until time-out.

This solution has been provided by: [https://github.com/NobodyNada](https://github.com/NobodyNada).

Issue has been opened on: https://github.com/StuartsProjects/SX12XX-LoRa/issues/44

Function to be appended to **SX12XX-LoRa/src/SX128XLT.cpp**

```c
int16_t SX128XLT::receiveRangingOrData(uint8_t *rxbuffer, uint8_t size, uint32_t address, uint16_t timeout, int8_t txpower, uint8_t wait)
{
#ifdef SX128XDEBUG
  Serial.println(F("receiveRangingOrData()"));
#endif

  uint8_t index, RXstart, RXend;
  uint16_t regdata;
  uint8_t buffer[2];

  setTxParams(txpower, RADIO_RAMP_02_US);
  setRangingSlaveAddress(address);
  setDioIrqParams(IRQ_RADIO_ALL, (IRQ_RX_DONE + IRQ_RX_TX_TIMEOUT + IRQ_RANGING_SLAVE_RESPONSE_DONE + IRQ_RANGING_SLAVE_REQUEST_DISCARDED + IRQ_HEADER_ERROR), 0, 0);  //set for IRQ on RX done or timeout
  setRx(timeout);

  if (!wait)
  {
    return 0;                          //not wait requested so no packet length to pass
  }

  while (!digitalRead(_RXDonePin));    //Wait for DIO1 to go high

  setMode(MODE_STDBY_RC);              //ensure to stop further packet reception
  regdata = readIrqStatus();

  if ( (regdata & IRQ_HEADER_ERROR) | (regdata & IRQ_CRC_ERROR) | (regdata & IRQ_RX_TX_TIMEOUT ) | (regdata & IRQ_SYNCWORD_ERROR )) //check if any of the preceding IRQs is set
  {
    return -1;                          //packet is errored somewhere so return -1
  }

  if (regdata & IRQ_RANGING_SLAVE_RESPONSE_DONE) {
    return 0;
  }

  readCommand(RADIO_GET_RXBUFFERSTATUS, buffer, 2);
  _RXPacketL = buffer[0];

  if (_RXPacketL > size)               //check passed buffer is big enough for packet
  {
    _RXPacketL = size;                 //truncate packet if not enough space
  }

  RXstart = buffer[1];
  RXend = RXstart + _RXPacketL;
  checkBusy();

#ifdef USE_SPI_TRANSACTION             //to use SPI_TRANSACTION enable define at beginning of CPP file 
  SPI.beginTransaction(SPISettings(LTspeedMaximum, LTdataOrder, LTdataMode));
#endif

  digitalWrite(_NSS, LOW);             //start the burst read
  SPI.transfer(RADIO_READ_BUFFER);
  SPI.transfer(RXstart);
  SPI.transfer(0xFF);

  for (index = RXstart; index < RXend; index++)
  {
    regdata = SPI.transfer(0);
    rxbuffer[index] = regdata;
  }

  digitalWrite(_NSS, HIGH);

#ifdef USE_SPI_TRANSACTION
  SPI.endTransaction();
#endif

  return _RXPacketL;
}
```

May 2022
– Louis de Looze