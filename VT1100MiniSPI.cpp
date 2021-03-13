/*
  MIT License
  Copyright (c) 2020 Michael Quinn

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

/*
  References
  Title: Z-Stack ZNP Interface Specification
  Author: Texas Instruments Incorporated
  Date: 2010-2012
  Revision: 1.4
  Availability: https://www.ti.com/tool/Z-STACK-ARCHIVE

  Title: Z-Stack Monitor and Test API
  Author: Texas Instruments Incorporated
  Date: 2008-2015
  Revision: 1.13
  Availability: https://www.ti.com/tool/Z-STACK-ARCHIVE
*/

/*
  VT1100MiniSPI.cpp Library
  Created by Michael Quinn

  Revision    Date          Descripton
  1.0         18/09/2020    First version
*/

#include "Arduino.h"
#include "VT1100MiniSPI.h"
#include "SPI.h"

/*
  Pin Connections
  PIN_EN        7     Enable pin for the 3V3 output
  PIN_SRDY      8     (P0_4) SRDY: Slave ready.
  PIN_RES       9     (RST) Reset
  PIN_SS_MRDY   10    (P0_3) (P1_4) SS: Slave select.  MRDY: Master Ready.  Active Low.  Connect SS and MRDY together
  PIN_MOSI      11    (P1_6) SPI MO: Master-output slave-input Data
  PIN_MISO      12    (P1_7) SPI MI: Master-input slave-output Data
  PIN_SCK       13    (P1_5) SPI DEBUG_SERIAL Clock
  CFG0                (P1_2) connected to RST Pin.  If high use external cystal for timing.
  CFG1                (P2_0) not connected.
*/

/*
  Constructor
  Define pin modes and states
*/
CC2530::CC2530(uint8_t PIN_EN, uint8_t PIN_SRDY, uint8_t PIN_RES, uint8_t PIN_SS_MRDY, uint8_t PIN_MOSI, uint8_t PIN_MISO, uint8_t PIN_SCK)
{
  pinMode(PIN_SS_MRDY, OUTPUT);
  pinMode(PIN_SRDY, INPUT);
  pinMode(PIN_RES, OUTPUT);
  pinMode(PIN_EN, OUTPUT);

  digitalWrite(PIN_EN, LOW);
  digitalWrite(PIN_RES, LOW); // Hold in RESET

  _EN = PIN_EN;
  _SRDY = PIN_SRDY;
  _RES = PIN_RES;
  _SS_MRDY = PIN_SS_MRDY;
  _MOSI = PIN_MOSI;
  _MISO = PIN_MISO;
  _SCK = PIN_SCK;
}

/*
  Bit shift function to read CMD0 and CMD1 in order
*/
uint16_t CC2530::cmd_conv(uint8_t Cmd0, uint8_t Cmd1)
{
  return Cmd0 << 8 | Cmd1;
}

/*
  Set the PanID
  Description: The network identification number set by the Coordinator.  Routers and End Devices will only join a coordinator with the same PANID
  Valid Values: 0x0000 to 0x3FFF
  Default Value: 0x00A1
*/
void CC2530::SetPANID(uint16_t Val)
{
  _PanID[5] = ((Val & 0xFFFF) >> 8);
	_PanID[6] = (Val & 0xFF);
}

/*
  Set the Channel
  Description: Set the wireless channel
  Valid Values: 0xFF or 11 to 26
  Default Value: 11
*/
void CC2530::SetCHANLIST(uint8_t Val)
{
  if (Val == 0xFF) // Channel ALL - 0x07FFF800 (00 F8 FF 07)
  {
    _Channel[5] = 0x00; _Channel[6] = 0xF8; _Channel[7] = 0xFF; _Channel[8] = 0x07;
  }
  else if (Val == 11) // Channel 11 - 0x00000800 (00 08 00 00)
  {
    _Channel[5] = 0x00; _Channel[6] = 0x08; _Channel[7] = 0x00; _Channel[8] = 0x00;
  }
  else if (Val == 12) // Channel 12 - 0x00001000 (00 10 00 00)
  {
    _Channel[5] = 0x00; _Channel[6] = 0x10; _Channel[7] = 0x00; _Channel[8] = 0x00;
  }
  else if (Val == 13) // Channel 13 - 0x00002000 (00 20 00 00)
  {
    _Channel[5] = 0x00; _Channel[6] = 0x20; _Channel[7] = 0x00; _Channel[8] = 0x00;
  }
  else if (Val == 14) // Channel 14 - 0x00004000 (00 40 00 00)
  {
    _Channel[5] = 0x00; _Channel[6] = 0x40; _Channel[7] = 0x00; _Channel[8] = 0x00;
  }
  else if (Val == 15) // Channel 15 - 0x00008000 (00 80 00 00)
  {
    _Channel[5] = 0x00; _Channel[6] = 0x80; _Channel[7] = 0x00; _Channel[8] = 0x00;
  }
  else if (Val == 16) // Channel 16 - 0x00010000 (00 00 01 00)
  {
    _Channel[5] = 0x00; _Channel[6] = 0x00; _Channel[7] = 0x01; _Channel[8] = 0x00;
  }
  else if (Val == 17) // Channel 17 - 0x00020000 (00 00 02 00)
  {
    _Channel[5] = 0x00; _Channel[6] = 0x00; _Channel[7] = 0x02; _Channel[8] = 0x00;
  }
  else if (Val == 18) // Channel 18 - 0x00040000 (00 00 04 00)
  {
    _Channel[5] = 0x00; _Channel[6] = 0x00; _Channel[7] = 0x04; _Channel[8] = 0x00;
  }
  else if (Val == 19) // Channel 19 - 0x00080000 (00 00 08 00)
  {
    _Channel[5] = 0x00; _Channel[6] = 0x00; _Channel[7] = 0x08; _Channel[8] = 0x00;
  }
  else if (Val == 20) // Channel 20 - 0x00100000 (00 00 10 00)
  {
    _Channel[5] = 0x00; _Channel[6] = 0x00; _Channel[7] = 0x10; _Channel[8] = 0x00;
  }
  else if (Val == 21) // Channel 21 - 0x00200000 (00 00 20 00)
  {
    _Channel[5] = 0x00; _Channel[6] = 0x00; _Channel[7] = 0x20; _Channel[8] = 0x00;
  }
  else if (Val == 22) // Channel 22 - 0x00400000 (00 00 40 00)
  {
    _Channel[5] = 0x00; _Channel[6] = 0x00; _Channel[7] = 0x40; _Channel[8] = 0x00;
  }
  else if (Val == 23) // Channel 23 - 0x00800000 (00 00 80 00)
  {
    _Channel[5] = 0x00; _Channel[6] = 0x00; _Channel[7] = 0x80; _Channel[8] = 0x00;
  }
  else if (Val == 24) // Channel 24 - 0x01000000 (00 00 00 01)
  {
    _Channel[5] = 0x00; _Channel[6] = 0x00; _Channel[7] = 0x00; _Channel[8] = 0x01;
  }
  else if (Val == 25) // Channel 25 - 0x02000000 (00 00 00 02)
  {
    _Channel[5] = 0x00; _Channel[6] = 0x00; _Channel[7] = 0x00; _Channel[8] = 0x02;
  }
  else if (Val == 26) // Channel 26 - 0x04000000 (00 00 00 04)
  {
    _Channel[5] = 0x00; _Channel[6] = 0x00; _Channel[7] = 0x00; _Channel[8] = 0x04;
  }
}

/*
  Set the Logical Type
  Description: Set the device type.
  Valid Values: 0x00 (Coordinator), 0x01 (Router) or 0x02 (End Device)
  Default Value: 0x00 (Coordinator)
*/
void CC2530::SetLOGICAL_TYPE(uint8_t Val)
{
  _LogicalType[5] = Val;
}

/*
  Set ZDO_DIRECT_CB Callback function
  Description: Set to true to recieve callbacks from the E18-MS1
  Valid Values: 0x01 or 0x00
  Default Value: 0x01 (true)
*/
void CC2530::SetZDO_DIRECT_CB(uint8_t Val)
{
  _CallBack[5] = Val;
}

/*
  Set POLL_RATE
  Description: The time period in milliseconds to poll the parent for messages.  Only for sleepy End Devices.
  Valid Values: 1 to 65000 milliseconds
  Default Value: 0x07D0 (2000 milliseconds)
*/
void CC2530::SetPOLL_RATE(uint16_t Val)
{
  _PollRate[5] = ((Val & 0xFFFF) >> 8);
	_PollRate[6] = (Val & 0xFF);
}

/*
  Set QUEUED_POLL_RATE
  Description: The time period to poll again for more data if the parent does have data.
  Valid Values: 1 to 65000 milliseconds
  Default Value: 0x0064 (100 milliseconds)
*/
void CC2530::SetQUEUED_POLL_RATE(uint16_t Val)
{
  _QueuedPollRate[5] = ((Val & 0xFFFF) >> 8);
	_QueuedPollRate[6] = (Val & 0xFF);
}

/*
  Set RESPONSE_POLL_RATE
  Description: The time period to poll after sending a AF_Data_Request to poll for acknowledgement of recieved messages.
  Valid Values: 1 to 65000 milliseconds
  Default Value: 0x0064 (100 milliseconds)
*/
void CC2530::SetRESPONSE_POLL_RATE(uint16_t Val)
{
  _ResponsePollRate[5] = ((Val & 0xFFFF) >> 8);
	_ResponsePollRate[6] = (Val & 0xFF);
}

/*
  Set REJOIN_POLL_RATE
  Description:  The time period to poll for rejoin requests. This is required when joining using TClinkkey joining process.
  Valid Values: 440 milliseconds (leave as default)
  Default Value: 0x01B8 (440 milliseconds)
*/
void CC2530::SetREJOIN_POLL_RATE(uint16_t Val)
{
  _RejoinPollRate[5] = ((Val & 0xFFFF) >> 8);
	_RejoinPollRate[6] = (Val & 0xFF);
}

/*
  Set POLL_FAILURE_RETRIES
  Description: The number of times a end device will fail to communicate to its parent before before attempting to join a new parent.
  Valid Values: 0x00 to 0xFF
  Default Value: 0xFF (255 retries)
*/
void CC2530::SetPOLL_FAILURE_RETRIES(uint8_t Val)
{
  _PollFailRetries[5] = Val;
}

/*
  Set PRECFGKEYEnable
  Description: Pre configured security key used on all devices
  Valid Values: 0x01 or 0x00
  Default Value: 0x01 (true)
*/
void CC2530::SetPRECFGKEYEnable(uint8_t Val)
{
  _PreCFGKeyEnable[5] = Val;
}

/*
  Set PRECFGKEY
  Description: A 16 byte security key
  Valid Values: 16 bytes between 0x00 and 0xFF
  Default Values: 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04
*/
void CC2530::SetPRECFGKEY(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f, uint8_t g, uint8_t h, uint8_t i, uint8_t j, uint8_t k, uint8_t l, uint8_t m, uint8_t n, uint8_t o, uint8_t p)
{
  _PreCFGKey[5] = a;
  _PreCFGKey[6] = b;
  _PreCFGKey[7] = c;
  _PreCFGKey[8] = d;
  _PreCFGKey[9] = e;
  _PreCFGKey[10] = f;
  _PreCFGKey[11] = g;
  _PreCFGKey[12] = h;
  _PreCFGKey[13] = i;
  _PreCFGKey[14] = j;
  _PreCFGKey[15] = k;
  _PreCFGKey[16] = l;
  _PreCFGKey[17] = m;
  _PreCFGKey[18] = n;
  _PreCFGKey[19] = o;
  _PreCFGKey[20] = p;
}

/*
  Set parameters for AF_DATA_REQUEST
  Description: Set the parameters for sending a AF_DATA_REQUEST
  Valid Values: Refer to Z-Stack ZNP Interface Specification
  Default Values:
  DesEP       0x01
  SourceEP    0x01
  ClusterID0  0xB0
  ClusterID1  0xFE
  TransID     0x01
  Options     0x00
  Radius      0x04
*/
void CC2530::SetAF_DATA_REQUEST(uint8_t DesEP, uint8_t SourceEP, uint8_t ClusterID0, uint8_t ClusterID1, uint8_t TransID, uint8_t Options, uint8_t Radius)
{
  _AFDataReqCfg[0] = DesEP;
  _AFDataReqCfg[1] = SourceEP;
  _AFDataReqCfg[2] = ClusterID0;
  _AFDataReqCfg[3] = ClusterID1;
  _AFDataReqCfg[4] = TransID;
  _AFDataReqCfg[5] = Options;
  _AFDataReqCfg[6] = Radius;
}

/*
  Set parameters for AF_DATA_REQUEST_EXT
  Description: Set the parameters for sending a AF_DATA_REQUEST_EXT
  Valid Values: Refer to Z-Stack ZNP Interface Specification
  Default Values:
  DesEP       0x01
  PanID0      0xA1
  PanID1      0x00
  SourceEP    0x01
  ClusterID0  0xB0
  ClusterID1  0xFE
  TransID     0x01
  Options     0x00
  Radius      0x04
*/
void CC2530::SetAF_DATA_REQUEST_EXT(uint8_t DesEP, uint8_t PanID0, uint8_t PanID1, uint8_t SourceEP, uint8_t ClusterID0, uint8_t ClusterID1, uint8_t TransID, uint8_t Options, uint8_t Radius)
{
  _AFDataReqExtCfg[0] = DesEP;
  _AFDataReqExtCfg[1] = PanID0;
  _AFDataReqExtCfg[2] = PanID1;
  _AFDataReqExtCfg[3] = SourceEP;
  _AFDataReqExtCfg[4] = ClusterID0;
  _AFDataReqExtCfg[5] = ClusterID1;
  _AFDataReqExtCfg[6] = TransID;
  _AFDataReqExtCfg[7] = Options;
  _AFDataReqExtCfg[8] = Radius;
}

/*
  Set TX_POWER
  Description: Set the radio transmit power in dBm
  Valid Values: 0x00 to 0x04
  Default Value: 0x04
*/
void CC2530::SetTX_POWER(uint8_t Val)
{
  _TXPower[3] = Val;
}

/*
  POWER_UP
  Description: Powers up the E18-MS1 by setting reset pin high
*/
void CC2530::POWER_UP()
{
  pinMode(_RES, INPUT_PULLUP); // Wake on RESET
  digitalWrite(_SS_MRDY, HIGH);
  delay(4000); // Need Delay after reset for CC2530 to startup

  RECV_CALLBACK();
}

/*
  Commission the E18-MS1
  Description: Clears the network state and configuration parameters then writes the new configuration parameters to non volatile memory.  This function should only be run once when joining a network.
*/
void CC2530::COMMISSION()
{
  WRITE_DATA(_NVStartUpClear); // ZCD_NV_STARTUP_OPTION_CLEAR

  // RESET
  SYS_RESET_REQ();

  WRITE_DATA(_NVStartUpKeep); // ZCD_NV_STARTUP_OPTION_KEEP
  WRITE_DATA(_LogicalType); // ZCD_NV_LogicalType
  WRITE_DATA(_PanID); // ZCD_NV_PanID
  WRITE_DATA(_Channel); // ZCD_NV_CHANLIST
  WRITE_DATA(_PollRate); // ZCD_NV_POLL_RATE
  WRITE_DATA(_QueuedPollRate); // ZCD_NV_QUEUED_POLL_RATE
  WRITE_DATA(_ResponsePollRate); // ZCD_NV_RESPONSE_POLL_RATE
  WRITE_DATA(_RejoinPollRate); // ZCD_NV_REJOIN_POLL_RATE
  WRITE_DATA(_PollFailRetries); // ZCD_NV_POLL_FAILURE_RETRIES
  WRITE_DATA(_CallBack); // ZCD_NV_ZDO_DIRECT_CB
  WRITE_DATA(_PreCFGKeyEnable); // ZCD_NV_PRECFGKEYEnable
  WRITE_DATA(_PreCFGKey); // ZCD_NV_PRECFGKEY
  WRITE_DATA(_TXPower); // SYS_SET_TX_POWER

  // IMPORTANT! RESET again to apply POLL settings.  All NV settings are saved on RESET.  If you don't RESET after setting POLL NV settings there will be a periodic POLL every few seconds.
  SYS_RESET_REQ();
}

/*
  POLL
  Description: The application processor polls the E18-MS1 for queued data
*/
void CC2530::POLL()
{
  while (digitalRead(_SRDY) == LOW)                                   // If SRDY is low CC2530 has message to send
  {
    DEBUG_SERIAL.println(F("POLL"));
    digitalWrite(_SS_MRDY, LOW);                                      // Detect SRDY is low then make MRDY low
    SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
    SPI.transfer(0x00);                                               // POLL message: Send three zero's to CC2530 (Length = 0, Cmd0 = 0 & Cmd1 = 0)
    SPI.transfer(0x00);
    SPI.transfer(0x00);

    while (digitalRead(_SRDY) == LOW) {};                             // Wait for SRDY to go high (CC2530 has AREQ frame to send, and will set SRDY high when ready to send)

    uint8_t Len = SPI.transfer(0x00);
    uint8_t Cmd0 = SPI.transfer(0x00);
    uint8_t Cmd1 = SPI.transfer(0x00);

    if (Len > 0)
    {

      ReceivedBytes[0] = Len;
      ReceivedBytes[1] = Cmd0;
      ReceivedBytes[2] = Cmd1;

      for (int i = 0; i < Len; i++)
      {
        ReceivedBytes[i+3] = SPI.transfer(0x00);
      }
      NewData = true;
      if (Cmd0 == 0x44 && Cmd1 == 0x81)
      {
        AFDataIncoming = true;
      }
      DEBUG_SERIAL.println(F("2530 AREQ"));                                 // Recieve AREQ message from CC2530
      DEBUG_SERIAL.print(F("Data length: "));
      DEBUG_SERIAL.println(Len, HEX);
      DEBUG_SERIAL.print(F("CMD: 0x"));
      DEBUG_SERIAL.println(cmd_conv(Cmd0, Cmd1), HEX);                      // Bit shift Cmd0 and Cmd1 to put in correct order.
      DEBUG_SERIAL.print(F("Data: "));
      for (int i = 0; i < Len; i++)
      {
        DEBUG_SERIAL.print(ReceivedBytes[i+3], HEX);
        DEBUG_SERIAL.print(F(" "));
      }
      DEBUG_SERIAL.println(F(""));
      DEBUG_SERIAL.println(F(""));
    }
  }
  SPI.endTransaction();
  digitalWrite(_SS_MRDY, HIGH);                                       // At the end of a POLL set MRDY = HIGH.  SRDY will also remain HIGH, until the CC2530 has another queued message to send.
}

/*
  SRSP
  Description: Synchronous response data from the E18-MS1 in response to a synchronous request by the application processor
*/
void CC2530::SRSP()
{
  DEBUG_SERIAL.println(F("SRSP"));

  uint8_t Len = SPI.transfer(0x00);
  uint8_t Cmd0 = SPI.transfer(0x00);
  uint8_t Cmd1 = SPI.transfer(0x00);

  if (Len > 0)
  {

    ReceivedBytes[0] = Len;
    ReceivedBytes[1] = Cmd0;
    ReceivedBytes[2] = Cmd1;

    for (int i = 0; i < Len; i++)
    {
      ReceivedBytes[i+3] = SPI.transfer(0x00);
    }
    DEBUG_SERIAL.println(F("2530 AREQ"));                                   // Recieve AREQ message from CC2530
    DEBUG_SERIAL.print(F("Data length: "));
    DEBUG_SERIAL.println(Len, HEX);
    DEBUG_SERIAL.print(F("CMD: 0x"));
    DEBUG_SERIAL.println(cmd_conv(Cmd0, Cmd1), HEX);                        // Bit shift Cmd0 and Cmd1 to put in correct order.
    DEBUG_SERIAL.print(F("Data: "));
    for (int i = 0; i < Len; i++)
    {
      DEBUG_SERIAL.print(ReceivedBytes[i+3], HEX);
      DEBUG_SERIAL.print(F(" "));
    }
    DEBUG_SERIAL.println(F(""));
    DEBUG_SERIAL.println(F(""));
  }
  SPI.endTransaction();
  digitalWrite(_SS_MRDY, HIGH);                                       // At the end of a POLL set MRDY = HIGH.  SRDY will also remain HIGH, until the CC2530 has another queued message to send.
}

/*
  Empty the Receive Buffer
*/
void CC2530::EMPTY_BUFFER()
{
  for (uint8_t n = 0; n < NumBytes; n++)
  {
    ReceivedBytes[n] = 0;
  }
}

/*
  NEW_DATA
  Description: Function returns true when an NewData is received for processing in the main sketch
*/
boolean CC2530::NEW_DATA()
{
  if (NewData == true)
  {
    NewData = false;
    return 1;
  }
  return 0;
}

/*
  AF_INCOMING_MSG
  Description: Function returns true when an AF_INCOMING_MSG is received for processing in the main sketch
*/
boolean CC2530::AF_INCOMING_MSG()
{
  if (AFDataIncoming == true)
  {
    AFDataIncoming = false;
    return 1;
  }
  return 0;
}

/*
  Recieve Callback
*/
void CC2530::RECV_CALLBACK()
{
  unsigned long time_now = millis();
  while (millis() - time_now < 500)
  {
    POLL();
    if (NewData == true)
    {
      NewData = false;
      break;
    }
  }
}

/*
  Link Quality
  Description: Print the short address and link quality from an AF_DATA_INCOMING message.  The link quality is from the last hop.
*/
void CC2530::LINK_QUALITY()
{
  uint8_t Len = ReceivedBytes[0];
  uint8_t ShortAddr0 = ReceivedBytes[Len];
  uint8_t ShortAddr1 = ReceivedBytes[(Len + 1)];
  uint8_t LQI = ReceivedBytes[12];

  DEBUG_SERIAL.print(F("Short Address: "));
  DEBUG_SERIAL.print(ShortAddr0, HEX);
  DEBUG_SERIAL.print(F(" "));
  DEBUG_SERIAL.print(ShortAddr1, HEX);
  DEBUG_SERIAL.print(F(" "));
  DEBUG_SERIAL.print(F("LQI: "));
  DEBUG_SERIAL.println(LQI, DEC);
}

/*
  Hardware Reset 2530
  Description: Reset the E18-MS1 using the reset pin
*/
void CC2530::HARD_RESET_REQ()
{
  DEBUG_SERIAL.println("");
  DEBUG_SERIAL.println(F("HARDWARE RESET"));
  pinMode(_RES, OUTPUT);
  digitalWrite(_RES, LOW);
  delay(100);
  pinMode(_RES, INPUT_PULLUP);                                      // Wake on RESET
  DEBUG_SERIAL.println("");
  delay(4000);
  RECV_CALLBACK();
}

/*
  SYS_RESET_REQ
  Description: Reset the E18-MS1 using software through an internal watchdog reset.
*/
void CC2530::SYS_RESET_REQ()
{
  DEBUG_SERIAL.println("");
  DEBUG_SERIAL.println(F("SYS_RESET_REQ"));
  WRITE_DATA(_SYS_Reset); // SYS_RESET_REQ
  delay(4000);
  RECV_CALLBACK();
}

/*
  Write Data to the E18-MS1
  Description: Write data to the E18-MS1
*/
void CC2530::WRITE_DATA(uint8_t *Data)
{
  DEBUG_SERIAL.println(F(""));
  DEBUG_SERIAL.print(F("0x"));
  DEBUG_SERIAL.println(Data[3],HEX);

  uint8_t Len = Data[0]+3;

  // SREQ
  digitalWrite(_SS_MRDY, LOW);
  while (digitalRead(_SRDY) == HIGH) {};
  SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));

  for (int i = 0; i < Len; i++)
  {
    SPI.transfer(Data[i]);
  }

  while (digitalRead(_SRDY) == LOW) {};

  SRSP();
}

/*
  Get Short Address of the E18-MS1
  Description: Obtain the short address (2 bytes) of the local E18-MS1
*/
void CC2530::ZB_GET_SHORT_ADDRESS(uint8_t ShortAddr[2])
{
  WRITE_DATA(_ZBGetShortAddr);
  ShortAddr[0] = ReceivedBytes[4];
  ShortAddr[1] = ReceivedBytes[5];
}

/*
  Get IEEE Address of the E18-MS1
  Description: Obtain the IEEE address (8 bytes) of the local E18-MS1
*/
void CC2530::ZB_GET_IEEE_ADDRESS(uint8_t IEEEAddr[8])
{
  WRITE_DATA(_ZBGetIEEEAddr);
  IEEEAddr[0] = ReceivedBytes[4];
  IEEEAddr[1] = ReceivedBytes[5];
  IEEEAddr[2] = ReceivedBytes[6];
  IEEEAddr[3] = ReceivedBytes[7];
  IEEEAddr[4] = ReceivedBytes[8];
  IEEEAddr[5] = ReceivedBytes[9];
  IEEEAddr[6] = ReceivedBytes[10];
  IEEEAddr[7] = ReceivedBytes[11];
}

/*
  ZDO_MGMT_PERMIT_JOIN_REQ
  Description: Set if the coordinator will permit devices to join the network
  Valid Values: true or false
  Default Value: true
*/
void CC2530::ZDO_MGMT_PERMIT_JOIN_REQ(bool PermitJoin)
{
  if (PermitJoin == true)
  {
    DEBUG_SERIAL.println(F("ZDO_MGMT_PERMIT_JOIN_REQ TRUE"));
    WRITE_DATA(_PermitJoinTrue);
  }
  else if (PermitJoin == false)
  {
    DEBUG_SERIAL.println(F("ZDO_MGMT_PERMIT_JOIN_REQ FASLE"));
    WRITE_DATA(_PermitJoinFalse);
  }
}

/*
  ZDO_MGMT_LEAVE_REQ
  Description: Sent from the Coordinator to make a device leave the network
*/
void CC2530::ZDO_MGMT_LEAVE_REQ(uint8_t DstAddr[2], uint8_t IEEEAddr[8])
{
  uint8_t Len = 0x0B;
  uint8_t Cmd0 = 0x25;
  uint8_t Cmd1 = 0x34;
  uint8_t Rejoin = 0x00;

  // Make array
  uint8_t LeaveReq[24] = {Len, Cmd0, Cmd1, DstAddr[0], DstAddr[1], IEEEAddr[0], IEEEAddr[1], IEEEAddr[2], IEEEAddr[3], IEEEAddr[4], IEEEAddr[5], IEEEAddr[6], IEEEAddr[7], Rejoin};

  DEBUG_SERIAL.println(F("ZDO_MGMT_LEAVE_REQ"));
  // SREQ
  digitalWrite(_SS_MRDY, LOW);
  while (digitalRead(_SRDY) == HIGH) {};
  SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
  for (int i = 0; i < sizeof(LeaveReq); i++)
  {
    SPI.transfer(LeaveReq[i]);
  }
  while (digitalRead(_SRDY) == LOW) {};

  SRSP();
}

/*
  ZDO_END_DEVICE_BIND_REQ
  Description: Used for binding two devices together for sending and recieving messages without knowing their network addressess
  Method:
  1. Send a ZDO_END_DEVICE_BIND_REQ(uint8_t EndPoint) on a End Device;
  2. Send a ZDO_END_DEVICE_BIND_REQ(uint8_t EndPoint) on the Coordinator within the default 8 seconds Binding time.
  3. Send a AF_DATA_REQUEST_EXT (const T& Value) to destination mode 0x00 and it will automatically lookup the address in the binding table
*/
void CC2530::ZDO_END_DEVICE_BIND_REQ (uint8_t EndPoint)
{
  uint8_t Len = 0x15;
  uint8_t Cmd0 = 0x25;
  uint8_t Cmd1 = 0x20;
  uint8_t DstAddr0 = 0x00;
  uint8_t DstAddr1 = 0x00;
  uint8_t ShortAddr[2];
  ZB_GET_SHORT_ADDRESS (ShortAddr);
  uint8_t IEEEAddr[8];
  ZB_GET_IEEE_ADDRESS (IEEEAddr);
  uint8_t Endpoint = EndPoint;
  uint8_t ProfileID0 = 0x04;
  uint8_t ProfileID1 = 0x05;
  uint8_t AppNumInClusters = 0x01;
  uint8_t InCluster0 = 0xB0;
  uint8_t InCluster1 = 0xFE;
  uint8_t AppNumOutClusters = 0x01;
  uint8_t OutCluster0 = 0xB0;
  uint8_t OutCluster1 = 0xFE;

  // Make array
  uint8_t ZDOEndDeviceBind[24] = {Len, Cmd0, Cmd1, DstAddr0, DstAddr1, ShortAddr[0], ShortAddr[1], IEEEAddr[0], IEEEAddr[1], IEEEAddr[2], IEEEAddr[3], IEEEAddr[4], IEEEAddr[5], IEEEAddr[6], IEEEAddr[7], Endpoint, ProfileID0, ProfileID1, AppNumInClusters, InCluster0, InCluster1, AppNumOutClusters, OutCluster0, OutCluster1};

  DEBUG_SERIAL.println(F("ZDO_END_DEVICE_BIND_REQ"));
  // SREQ
  digitalWrite(_SS_MRDY, LOW);
  while (digitalRead(_SRDY) == HIGH) {};
  SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
  for (int i = 0; i < sizeof(ZDOEndDeviceBind); i++)
  {
    SPI.transfer(ZDOEndDeviceBind[i]);
  }
  while (digitalRead(_SRDY) == LOW) {};

  SRSP();
}

/*
  ZDO_NODE_DESC_REQ
  Description: Sent from the Coordinator to Destination device to recieve node descriptor information from the device.
*/
void CC2530::ZDO_NODE_DESC_REQ(uint8_t DstAddr[2], uint8_t NWKAddrOfInterest[2])
{
  DEBUG_SERIAL.println(F("ZDO_NODE_DESC_REQ"));
  _NodeDesc[3] = DstAddr[0];
  _NodeDesc[4] = DstAddr[1];
  _NodeDesc[5] = NWKAddrOfInterest[0];
  _NodeDesc[6] = NWKAddrOfInterest[1];
  WRITE_DATA(_NodeDesc);
}

/*
  SYS_GPIO
  Description: Used by the application processor to configure the GPIO pins on the E18-MS1.  The four Lower Order Bits are used for selecting GPIO's.
  Valid Values: 0x01, 0x02, 0x04, 0x08, 0x00 or 0x0F.
  Default Value: 0x0F

  Pin ID,   Hex value,  Pin Name,   Byte value
  P0.0,     0x01,       GPIO0,      00000001
  P0.1,     0x02,       GPIO1,      00000010
  P0.6,     0x04,       GPIO2,      00000100
  P1.0,     0x08,       GPIO3,      00001000
  All,      0x0F,       ALL PINS,   00001111
  None,     0x00,       ALL PINS,   00000000

*/

/*
  SYS_GPIO_SET_DIR
  Description: Configure the direction of the GPIO pins. A value of 1 in a bit position will set the corresponding pin as an output whereas a value of 0 in a bit position will set the pin as an input
*/
void CC2530::SYS_GPIO_SET_DIR(uint8_t Val)  // Configures the direction of the GPIO pins as Outputs.
{
  DEBUG_SERIAL.println(F("SYS_GPIO_Set_Dir SREQ"));
  _GPIOSetDir[4] = Val;
  WRITE_DATA(_GPIOSetDir);
}

/*
  SYS_GPIO_SET_INPUT_MODE
  Description: Configures the Input mode of the GPIO pins. A value of 1 in a bit position will set the corresponding pin into tri-state mode.  Otherwise if the pin is high is will be set as a pull-up or if the pin low it will be set as a pull-down.
*/
void CC2530::SYS_GPIO_SET_INPUT_MODE(uint8_t Val)  // Configures the direction of the GPIO pins as Inputs.
{
  DEBUG_SERIAL.println(F("SYS_GPIO_Set_INPUT_MODE SREQ"));
  _GPIOSetInput[4] = Val;
  WRITE_DATA(_GPIOSetInput);
}

/*
  SYS_GPIO_SET
  Description: A value of 1 in a bit position will set the pin high
*/
void CC2530::SYS_GPIO_SET(uint8_t Val) // Writes a 1 (Output High)
{
  DEBUG_SERIAL.println(F("SYS_GPIO_Set SREQ"));
  _GPIOSet[4] = Val;
  WRITE_DATA(_GPIOSet);
}

/*
  SYS_GPIO_CLEAR
  Description: A value of 1 in a bit position will set the pin low
*/
void CC2530::SYS_GPIO_CLEAR(uint8_t Val) // Writes a 0 (Output Low)
{
  DEBUG_SERIAL.println(F("SYS_GPIO_Clear SREQ"));
  _GPIOClear[4] = Val;
  WRITE_DATA(_GPIOClear);
}

/*
  SYS_GPIO_READ
  Description: Reads the value of the GPIO pins.
*/
void CC2530::SYS_GPIO_READ(uint8_t Val) // Reads the GPIO pins
{
  DEBUG_SERIAL.println(F("SYS_GPIO_Read SREQ"));
  _GPIORead[4] = Val;
  WRITE_DATA(_GPIORead);
}

/*
  AF_REGISTER
  Description: Register an applications endpoint description.  Multiple endpoints can be registered.  The profile ID and Cluster ID's are left as default because a proprietary profile is used in this application.
*/
void CC2530::AF_REGISTER(uint8_t EndPoint)
{
  uint8_t Len = 0x0D; // total length after Cmd1 excl. XOR
  uint8_t Cmd0 = 0x24;
  uint8_t Cmd1 = 0x00;
  uint8_t AppEndPoint = EndPoint; // Register End Point
  uint8_t AppProfileID0 = 0x04;
  uint8_t AppProfileID1 = 0x05;
  uint8_t DeviceID0 = 0x00;
  uint8_t DeviceID1 = 0x00;
  uint8_t DeviceVer = 0x01;
  uint8_t LatencyReq = 0x00;
  uint8_t AppNumInClusters = 0x01;
  uint8_t InCluster0 = 0xB0;
  uint8_t InCluster1 = 0xFE;
  uint8_t AppNumOutClusters = 0x01;
  uint8_t OutCluster0 = 0xB0;
  uint8_t OutCluster1 = 0xFE;


  // Make array
  uint8_t AFRegister[16] = {Len, Cmd0, Cmd1, AppEndPoint, AppProfileID0, AppProfileID1, DeviceID0, DeviceID1, DeviceVer, LatencyReq, AppNumInClusters, InCluster0, InCluster1, AppNumOutClusters, OutCluster0, OutCluster1};

  DEBUG_SERIAL.println(F("AF_REGISTER SREQ"));
  // SREQ
  digitalWrite(_SS_MRDY, LOW);
  while (digitalRead(_SRDY) == HIGH) {};
  SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
  for (int i = 0; i < sizeof(AFRegister); i++)
  {
    SPI.transfer(AFRegister[i]);
  }
  while (digitalRead(_SRDY) == LOW) {};

  SRSP();
}

/*
  ZDO_STARTUP_FROM_APP
  Description: Starts the device in the network
*/
void CC2530::ZDO_STARTUP_FROM_APP()
{
  DEBUG_SERIAL.println(F("ZDO_STARTUP_FROM_APP SREQ"));
  WRITE_DATA(_ZDOStartUpFromApp); // ZDO_STARTUP_FROM_APP

  unsigned long time_now = millis();
  while (millis() - time_now < 60000)
  {
    POLL();

    if (NewData == true)
    {
      uint8_t Cmd0 = ReceivedBytes[1];
      uint8_t Cmd1 = ReceivedBytes[2];
      uint8_t State = ReceivedBytes[3];

      if (Cmd0 == 0x45 && Cmd1 == 0xC0)
      {
        if (State == 0x06)
        {
          DEBUG_SERIAL.println(F("Started as End Device"));
          break;
        }
        else if (State == 0x07)
        {
          DEBUG_SERIAL.println(F("Started as Router"));
          break;
        }
        else if (State == 0x09)
        {
          DEBUG_SERIAL.println(F("Started as Coordinator"));
          break;
        }
        else if (State == 0x10)
        {
          DEBUG_SERIAL.println(F("Lost parent"));
          break;
        }
      }
      NewData = false;
    }
  }
}
