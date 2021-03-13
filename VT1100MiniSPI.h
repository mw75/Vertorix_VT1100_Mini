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
  VT1100MiniSPI.h Library
  Created by Michael Quinn

  Revision    Date          Descripton
  1.0         18/09/2020    First version
*/

#ifndef VT1100MiniSPI_h
	#define VT1100MiniSPI_h

  #if ARDUINO >= 100
    #include "Arduino.h"
    #else
    #include "WProgram.h"
  #endif
	#include "SPI.h"

  /*
    Debug Mode
    Turn debugging off in Library to save program space.
  */
  #define DEBUG true //set to true for debug output, false for no debug output
  #define DEBUG_SERIAL if(DEBUG)Serial

  /*
    Class
    CC2530
  */
	class CC2530
	{
		public:

    /*
      Global Variables
    */
    uint8_t ReceivedBytes[64];
    boolean NewData = false;
    boolean AFDataIncoming = false; // New AF_DATA_INOMING message

    CC2530(uint8_t PIN_EN = 7, uint8_t PIN_SRDY = 8, uint8_t PIN_RES = 9, uint8_t PIN_SS_MRDY = 10, uint8_t PIN_MOSI = 11, uint8_t PIN_MISO = 12, uint8_t PIN_SCK = 13);
    void POWER_UP();
    void COMMISSION();
    void WRITE_DATA(uint8_t *Data);
		void HARD_RESET_REQ();
    void SYS_RESET_REQ();
		void POLL();
		void EMPTY_BUFFER();
		void SRSP();
		boolean NEW_DATA();
    boolean AF_INCOMING_MSG();
    void RECV_CALLBACK();
    void LINK_QUALITY();
		void SYS_GPIO_SET_DIR(uint8_t Val);
		void SYS_GPIO_SET_INPUT_MODE(uint8_t Val);
		void SYS_GPIO_SET(uint8_t Val);
		void SYS_GPIO_CLEAR(uint8_t Val);
		void SYS_GPIO_READ(uint8_t Val);
		void AF_REGISTER(uint8_t EndPoint);
		void ZDO_STARTUP_FROM_APP();
    void ZB_GET_SHORT_ADDRESS(uint8_t ShortAddr[2]);
    void ZB_GET_IEEE_ADDRESS(uint8_t IEEEAddr[8]);
    void ZDO_MGMT_PERMIT_JOIN_REQ(bool PermitJoin = true);
    void ZDO_END_DEVICE_BIND_REQ(uint8_t EndPoint);
    void ZDO_MGMT_LEAVE_REQ(uint8_t DstIEEEAddr[8]);
    void ZDO_NODE_DESC_REQ(uint8_t DstAddr[2], uint8_t NWKAddrOfInterest[2]);
    uint16_t cmd_conv(uint8_t Cmd0, uint8_t Cmd1);
    void SetPANID(uint16_t Val = 0);
    void SetLOGICAL_TYPE(uint8_t Val = 0);
    void SetCHANLIST(uint8_t Val = 0);
    void SetZDO_DIRECT_CB(uint8_t Val = 0);
    void SetPOLL_RATE(uint16_t Val = 0);
    void SetQUEUED_POLL_RATE(uint16_t Val = 0);
    void SetRESPONSE_POLL_RATE(uint16_t Val = 0);
    void SetREJOIN_POLL_RATE(uint16_t Val = 0);
    void SetPOLL_FAILURE_RETRIES(uint8_t Val = 0);
    void SetPRECFGKEYEnable(uint8_t Val = 0);
    void SetPRECFGKEY(uint8_t a = 0, uint8_t b = 0, uint8_t c = 0, uint8_t d = 0, uint8_t e = 0, uint8_t f = 0, uint8_t g = 0, uint8_t h = 0, uint8_t i = 0, uint8_t j = 0, uint8_t k = 0, uint8_t l = 0, uint8_t m = 0, uint8_t n = 0, uint8_t o = 0, uint8_t p = 0);
    void SetAF_DATA_REQUEST(uint8_t DesEP = 0, uint8_t SourceEP = 0, uint8_t ClusterID0 = 0, uint8_t ClusterID1 = 0, uint8_t TransID = 0, uint8_t Options = 0, uint8_t Radius = 0);
    void SetAF_DATA_REQUEST_EXT(uint8_t DesEP = 0, uint8_t PanID0 = 0, uint8_t PanID1 = 0, uint8_t SourceEP = 0, uint8_t ClusterID0 = 0, uint8_t ClusterID1 = 0, uint8_t TransID = 0, uint8_t Options = 0, uint8_t Radius = 0);
    void SetTX_POWER(uint8_t Val = 0);

    /*
      AF_DATA_REQUEST
      Used by the Application processor to send a message
    */
    template <typename T> unsigned int AF_DATA_REQUEST (uint8_t ShortAddr0, uint8_t ShortAddr1, const T& Value, uint8_t Length)
    {
      uint8_t Len = Length + 10;
      uint8_t Cmd0 = 0x24;
      uint8_t Cmd1 = 0x01;
      uint8_t DstAddr0 = ShortAddr1;
      uint8_t DstAddr1 = ShortAddr0;
      uint8_t DesEP = _AFDataReqCfg[0];
      uint8_t SourceEP = _AFDataReqCfg[1];
      uint8_t ClusterID0 = _AFDataReqCfg[2];
      uint8_t ClusterID1 = _AFDataReqCfg[3];
      uint8_t TransID = _AFDataReqCfg[4];
      uint8_t Options = _AFDataReqCfg[5];
      uint8_t Radius = _AFDataReqCfg[6];
      uint8_t DataLen = Length;

      // Make array
      uint8_t Data[13] = {Len, Cmd0, Cmd1, DstAddr0, DstAddr1, DesEP, SourceEP, ClusterID0, ClusterID1, TransID, Options, Radius, DataLen};
      DEBUG_SERIAL.println(F("AF_DATA_REQUEST SREQ"));
      // SREQ
      digitalWrite(_SS_MRDY, LOW);
      while (digitalRead(_SRDY) == HIGH) {};
      SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
      for (int i = 0; i < sizeof(Data); i++)
      {
        SPI.transfer(Data[i]);
      }

      const uint8_t * p = (const uint8_t*) &Value; // Send Data as a stream of bytes
      unsigned int i;
      for (i = 0; i < Length; i++)
      SPI.transfer(*p++);

      while (digitalRead(_SRDY) == LOW) {};
      SRSP();
      return i;
    }

    /*
      AF_DATA_REQUEST_EXT
      Used for sending messages using binding.
      1. Send a ZDO_END_DEVICE_BIND_REQ(uint8_t EndPoint) on a End Device;
      2. Send a ZDO_END_DEVICE_BIND_REQ(uint8_t EndPoint) on the Coordinator within default 8 seconds Binding time.
      3. Send a AF_DATA_REQUEST_EXT (const T& Value) to lookup the address in the binding table
    */
    template <typename T> unsigned int AF_DATA_REQUEST_EXT (uint8_t AddrMode, const uint8_t IEEEAddr[8], const T& Value, uint8_t Length)
    {
      uint8_t Len = Length + 20;
      uint8_t Cmd0 = 0x24;
      uint8_t Cmd1 = 0x02;
      uint8_t DstAddrMode = AddrMode; // Use destination mode 0x00 to lookup the address in the binding table
      uint8_t DstAddr0 = IEEEAddr[7];
      uint8_t DstAddr1 = IEEEAddr[6];
      uint8_t DstAddr2 = IEEEAddr[5];
      uint8_t DstAddr3 = IEEEAddr[4];
      uint8_t DstAddr4 = IEEEAddr[3];
      uint8_t DstAddr5 = IEEEAddr[2];
      uint8_t DstAddr6 = IEEEAddr[1];
      uint8_t DstAddr7 = IEEEAddr[0];
      uint8_t DesEP = _AFDataReqExtCfg[0];
      uint8_t DstPanId0 = _AFDataReqExtCfg[1];
      uint8_t DstPanId1 = _AFDataReqExtCfg[2];
      uint8_t SourceEP = _AFDataReqExtCfg[3];
      uint8_t ClusterID0 = _AFDataReqExtCfg[4];
      uint8_t ClusterID1 = _AFDataReqExtCfg[5];
      uint8_t TransID = _AFDataReqExtCfg[6];
      uint8_t Options = _AFDataReqExtCfg[7];
      uint8_t Radius = _AFDataReqExtCfg[8];
      uint8_t DataLen0 = Length;
      uint8_t DataLen1 = 0x00;

      // Make array
      uint8_t Data[23] = {Len, Cmd0, Cmd1, DstAddrMode, DstAddr0, DstAddr1, DstAddr2, DstAddr3, DstAddr4, DstAddr5, DstAddr6, DstAddr7, DesEP, DstPanId0, DstPanId1, SourceEP, ClusterID0, ClusterID1, TransID, Options, Radius, DataLen0, DataLen1};
      DEBUG_SERIAL.println(F("AF_DATA_REQUEST_EXT SREQ"));
      // SREQ
      digitalWrite(_SS_MRDY, LOW);
      while (digitalRead(_SRDY) == HIGH) {};
      SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
      for (int i = 0; i < sizeof(Data); i++)
      {
        SPI.transfer(Data[i]);
      }

      const uint8_t * p = (const uint8_t*) &Value; // Send Data as a stream of bytes
      unsigned int i;
      for (i = 0; i < Length; i++)
      SPI.transfer(*p++);

      while (digitalRead(_SRDY) == LOW) {};
      SRSP();
      return i;
    }

    /*
      COPY_PAYLOAD
      Copies the payload from the AF_INCOMING_MSG to a variable
    */
    template <typename T> unsigned int COPY_PAYLOAD (T& Value)
    {
      uint8_t *p;
      p = ReceivedBytes;
      memcpy(&Value, p + 20, ReceivedBytes[19]); // AFDataIncoming Data payload always starts at byte 20 in recieve buffer "ReceivedBytes"
    }

    private:

    uint8_t _EN;
    uint8_t _SRDY;
    uint8_t _RES;
    uint8_t _SS_MRDY;
    uint8_t _MOSI;
    uint8_t _MISO;
    uint8_t _SCK;

    uint8_t _SYS_Reset[4] = {0x01, 0x41, 0x00, 0x00};
    uint8_t _TXPower[4] = {0x01, 0x21, 0x14, 0x04}; // Default 4dBm
    uint8_t _NVStartUpKeep[6] = {0x03, 0x26, 0x05, 0x03, 0x01, 0x00}; // Keeps device specific and network parameters stored in non-volitile (NV) memory
    uint8_t _NVStartUpClear[6] = {0x03, 0x26, 0x05, 0x03, 0x01, 0x03}; // Clears device specific and network parameters stored in non-volitile (NV) memory
    uint8_t _PanID[7] = {0x04, 0x26, 0x05, 0x83, 0x02, 0xA1, 0x00}; // Default 0x00A1 PanId
    uint8_t _Channel[9] = {0x06, 0x26, 0x05, 0x84, 0x04, 0x00, 0x08, 0x00, 0x00}; // Default Channel 11
    uint8_t _LogicalType[6] = {0x03, 0x26, 0x05, 0x87, 0x01, 0x00}; // Default 0x00 Coordinator
    uint8_t _CallBack[6] = {0x03, 0x26, 0x05, 0x8F, 0x01, 0x01}; // Default 0x01 True
    uint8_t _PollRate[7] = {0x04, 0x26, 0x05, 0x35, 0x02, 0xD0, 0x07}; // Default 2000 milliseconds
    uint8_t _QueuedPollRate[7] = {0x04, 0x26, 0x05, 0x25, 0x02, 0x64, 0x00}; // Default 100 milliseconds
    uint8_t _ResponsePollRate[7] = {0x04, 0x26, 0x05, 0x26, 0x02, 0x64, 0x00}; // Default 100 milliseconds
    uint8_t _RejoinPollRate[7] = {0x04, 0x26, 0x05, 0x27, 0x02, 0xB8, 0x01}; // Default 440 milliseconds, required for End device join using TC Link Key, mandatory packets when joining network
    uint8_t _PollFailRetries[6] = {0x03, 0x26, 0x05, 0x29, 0x01, 0xFF}; // Default 0xFF 255 Retries
    uint8_t _PreCFGKeyEnable[6] = {0x03, 0x26, 0x05, 0x63, 0x01, 0x01}; // Default 0x01 True
    uint8_t _PreCFGKey[21] = {0x12, 0x26, 0x05, 0x62, 0x10, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04}; // Default 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04
    uint8_t _AFDataReqCfg[7] = {0x01, 0x01, 0xB0, 0xFE, 0x01, 0x00, 0x04};
    uint8_t _AFDataReqExtCfg[9] = {0x01, _PanID[5], _PanID[6], 0x01, 0xB0, 0xFE, 0x01, 0x00, 0x04};
    uint8_t _ZDOStartUpFromApp[5] = {0x02, 0x25, 0x40, 0x00, 0x00};
    uint8_t _GPIOSetDir[6] = {0x02, 0x21, 0x0E, 0x00, 0x0F}; // Default 0x0F All GPIO's
    uint8_t _GPIOSetInput[6] = {0x02, 0x21, 0x0E, 0x01, 0x0F}; // Default 0x0F All GPIO's
    uint8_t _GPIOSet[6] = {0x02, 0x21, 0x0E, 0x02, 0x0F}; // Default 0x0F All GPIO's
    uint8_t _GPIOClear[6] = {0x02, 0x21, 0x0E, 0x03, 0x0F}; // Default 0x0F All GPIO's
    uint8_t _GPIORead[6] = {0x02, 0x21, 0x0E, 0x05, 0x0F}; // Default 0x0F All GPIO's
    uint8_t _NodeDesc[7] = {0x04, 0x25, 0x02, 0x00, 0x00, 0x00, 0x00};
    uint8_t _ZBGetShortAddr[4] = {0x01, 0x26, 0x06, 0x02};
    uint8_t _ZBGetIEEEAddr[4] = {0x01, 0x26, 0x06, 0x01};
    uint8_t _PermitJoinTrue[7] = {0x04, 0x25, 0x36, 0x00, 0x00, 0xFF, 0x00};
    uint8_t _PermitJoinFalse[7] = {0x04, 0x25, 0x36, 0x00, 0x00, 0x00, 0x00};
  };

#endif
