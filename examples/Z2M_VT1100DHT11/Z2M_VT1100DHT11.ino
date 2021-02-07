/*
    Example: VT1100DHT11
    Description: A low powered Temperature and Humidity sensor which is designed to work with Zigbee2MQTT.
    Please note this device is not Zigbee compliant.

    The device goes through the following loop sequence:
    1. Take sensor readings
    2. Send data
    3. Sleep

    Zigbee2MQTT Device Converters

    File: devices.js
  {
    zigbeeModel: ['VT1100DHT11'],
    model: 'VT1100',
    vendor: 'Vertorix',
    description: 'Vertorix Temperature & Humidity Sensor',
    fromZigbee: [fz.temperature, fz.humidity],
    toZigbee: [],
  },
*/

/* ------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------
*/
#include <VT1100MiniSPI.h>
#include <SPI.h>
#include "LowPower.h"
#include <dht.h>

/* ------------------------------------------------------------------
   Class Instances
   ------------------------------------------------------------------
*/
CC2530 mycc2530;                                                      // Library VT1100MiniSPI Class Instance
dht DHT;                                                              // ****Instance of the dht class called DHT

/* ------------------------------------------------------------------
   Pins
   ------------------------------------------------------------------
*/
#define PIN_EN      7                                                 // Enable
#define PIN_SRDY    8                                                 // (P0_4) SRDY: Slave ready.
#define PIN_RES     9                                                 // (RST) RESET
#define PIN_SS_MRDY 10                                                // (P0_3) (P1_4) SS: Slave select.  MRDY: Master Ready.  Active Low.  Connect SS and MRDY together
#define PIN_MOSI    11                                                // (P1_6) SPI MO: Master-output slave-input data
#define PIN_MISO    12                                                // (P1_7) SPI MI: Master-input slave-output data
#define PIN_SCK     13                                                // (P1_5) SPI Serial Clock

#define Button      2                                                 // Commission button
#define DHT11_PIN   3                                                 // DHT11 input pin

/* ------------------------------------------------------------------
   Sleep Timer
   ------------------------------------------------------------------
*/
static const uint8_t sleepcount = 8;                                  // Watchdog timer can sleep for max of 8 sec.  Uses a for loop.  8*8=60sec or 8*15 = 120sec (8=sleep ~1 min or 15=sleep ~2 min)

/* ------------------------------------------------------------------
   Millis - Timing
   ------------------------------------------------------------------
*/
unsigned long period = 30000;
unsigned long time_now = 0;

/* ------------------------------------------------------------------
   Global Veriables
   ------------------------------------------------------------------
*/
uint8_t seqCount = 0;                                                 // used for keeping track of message transaction sequences
uint8_t seqNumber = 0;                                                // used for keeping track of message transaction sequences

/* ------------------------------------------------------------------
   ZCL: Read Attribute Response Commands
   ------------------------------------------------------------------
*/
uint8_t ModelIdentifier[] = {0x05, 0x00, 0x00, 0x42, 0x0C, 0x56, 0x54, 0x31, 0x31, 0x30, 0x30, 0x44, 0x48, 0x54, 0x31, 0x31};           // Model Identifier: VT1100DHT11
uint8_t ManufacturerName[] = {0x04, 0x00, 0x00, 0x42, 0x08, 0x56, 0x65, 0x72, 0x74, 0x6f, 0x72, 0x69, 0x78};                            // Manufacturer Name: Vertorix
uint8_t PowerSource[] = {0x07, 0x00, 0x00, 0x30, 0x03};                                                                                 // Power Source: Battery
uint8_t ApplicationVersion[] = {0x01, 0x00, 0x00, 0x20, 0x01};                                                                          // Application Version
uint8_t ZCLVersion[] = {0x00, 0x00, 0x00, 0x20, 0x01};                                                                                  // ZCL version
uint8_t StackVersion[] = {0x02, 0x00, 0x00, 0x20, 0x02};                                                                                // Stack version
uint8_t HWVersion[] = {0x03, 0x00, 0x00, 0x20, 0x02};                                                                                   // Hardware version
uint8_t Datecode[] = {0x06, 0x00, 0x00, 0x42, 0x08, 0x32, 0x30, 0x32, 0x31, 0x30, 0x32, 0x30, 0x33};                                    // Date Code
uint8_t SoftwareBuildID[] = {0x00, 0x40, 0x00, 0x42, 0x09, 0x31, 0x32, 0x30, 0x30, 0x2d, 0x30, 0x30, 0x30, 0x31};                       // Software Build ID

/********************************************************************
   Setup
 ********************************************************************
*/
void setup()
{
  Serial.begin(115200);
  delay(100);

  pinMode(PIN_EN, OUTPUT);
  digitalWrite(PIN_EN, LOW);

  SPI.begin();

  // Parameters for CC2530
  mycc2530.SetPANID(0xffff);                                          // PAN ID.  Two bytes set between 0x0000 and 0x3FFF.  Examples: 0x00A1, 0x00A2 or 0x00A3.
  mycc2530.SetLOGICAL_TYPE(0x02);                                     // Device type.  Examples: Coordinator = 0x00, Router = 0x01 or End Device = 0x02
  mycc2530.SetCHANLIST(11);                                           // Wireless Channel. Examples: 11 to 26 or 0xFF All Channels

  Init_CC2530();
}

/********************************************************************
   Loop
 ********************************************************************
*/
void loop()
{
  /* ------------------------------------------------------------------
    Sensor Readings
    ------------------------------------------------------------------
  */
  digitalWrite(PIN_EN, HIGH);                                           // Power on sensors

  delay(2000);
  int chk = DHT.read11(DHT11_PIN);                                      // Read DHT11 sensor
  delay(1500);                                                          // Sensor needs 1.5 sec to take readings

  digitalWrite(PIN_EN, LOW);                                            // Power off sensors

  /* ------------------------------------------------------------------
    Send Data
    ------------------------------------------------------------------
  */
  Poll(50);                                                             // Give time for power to stabalise before transmitting
  ZCLFrame_ReportAttributes_Temp();                                     // Send Temperature ZCL report attribute
  ZCLFrame_ReportAttributes_Humidity();                                 // Send Humidity ZCL report attribute
  Poll(500);
  /* -----------------------------------------------------------------
    Sleep
    -----------------------------------------------------------------
  */
  Sleep();                                                              // Sleep for the time set by the Sleep Timer
}


/********************************************************************
   Other Functions
 ********************************************************************
*/

/* ------------------------------------------------------------------
   Initialise CC2530
   ------------------------------------------------------------------
*/
void Init_CC2530()
{
  mycc2530.POWER_UP();                                                // Power up the CC2530 (wake on reset).
  SetTXPower();

  pinMode(Button, INPUT_PULLUP);
  if (digitalRead(Button) == LOW)
  {
    mycc2530.COMMISSION();                                            // Clears the configuration and network state then writes the new configuration parameters to the CC2530 non-volitile (NV) memory.  This should only be run once on initial setup.
    AF_REGISTER(0x01);                                                // Register Endpoint 1 ZCL
    mycc2530.ZDO_STARTUP_FROM_APP();                                  // Starts the CC2530 in the network
    Poll(5000);
    AF_REGISTER(0x01);                                                // Register Endpoint 1 ZCL
    mycc2530.ZDO_STARTUP_FROM_APP();                                  // Starts the CC2530 in the network
    Poll(5000);
    ZCLFrame_ReportAttributes();                                      // ZCL Report Model Identifier and Application Version
    Interview(30000);
  }

  if (digitalRead(Button) == HIGH)
  {
    AF_REGISTER(0x01);
    mycc2530.ZDO_STARTUP_FROM_APP();                                    // Starts the CC2530 in the network
    Poll(5000);
  }

  NVSetPollRate0sec();                                                  // Set the poll rate to 0 (polling turned off) for maximum power saving and to stop rejoining on loss of parent.
  NVSetPollFailRetries();                                               // Set poll failure retries to 255 to ensure a rejoin isn't started on loss of parent device.  To reduce battery drain.
}

/* ------------------------------------------------------------------
   Set Poll Rate 7sec Function
   ------------------------------------------------------------------
*/
void NVSetPollRate7sec()
{
  uint8_t SetPollRate[] = {0x08, 0x21, 0x09, 0x35, 0x00, 0x00, 0x04, 0x58, 0x1B, 0x00, 0x00}; // SYS_OSAL_NV_WRITE Set the poll rate 7sec 0x00001B58(Needs to be less than INDIRECT_MSG_TIMEOUT of Routers)
  uint8_t ReadPollRate[] = {0x03, 0x21, 0x08, 0x35, 0x00, 0x00}; // SYS_OSAL_NV_READ

  mycc2530.WRITE_DATA(SetPollRate);
  mycc2530.WRITE_DATA(ReadPollRate);
  Serial.println("Poll Rate set to 7 sec, polling turned on");
}

/* ------------------------------------------------------------------
   Set Poll Rate 0sec Function
   ------------------------------------------------------------------
*/
void NVSetPollRate0sec()
{
  uint8_t SetPollRate[] = {0x08, 0x21, 0x09, 0x35, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00}; // SYS_OSAL_NV_WRITE Set the poll rate 7sec 0x00001B58(Needs to be less than INDIRECT_MSG_TIMEOUT of Routers)
  uint8_t ReadPollRate[] = {0x03, 0x21, 0x08, 0x35, 0x00, 0x00}; // SYS_OSAL_NV_READ

  mycc2530.WRITE_DATA(SetPollRate);
  Serial.println("Poll Rate set to 0 sec, polling turned off");
  mycc2530.WRITE_DATA(ReadPollRate);
}

/* ------------------------------------------------------------------
   Set Poll Failure Retries Function
   ------------------------------------------------------------------
*/
void NVSetPollFailRetries()
{
  uint8_t SetPollFailRetries[] = {0x05, 0x21, 0x09, 0x29, 0x00, 0x00, 0x01, 0xFF}; // SYS_OSAL_NV_WRITE Set poll failure retries to 255 before start rejoin scan/backoff process
  uint8_t ReadPollFailRetries[] = {0x03, 0x21, 0x08, 0x29, 0x00, 0x00}; // SYS_OSAL_NV_READ

  mycc2530.WRITE_DATA(SetPollFailRetries);
  Serial.println("Poll failure retries set to 255");
  mycc2530.WRITE_DATA(ReadPollFailRetries);
}

/* ------------------------------------------------------------------
   Set TX Power Function
   ------------------------------------------------------------------
*/
void SetTXPower()
{
  uint8_t TXPower[] = {0x01, 0x21, 0x14, 0x04};

  mycc2530.WRITE_DATA(TXPower);
  Serial.println("~~TX power set~~");
}

/* ------------------------------------------------------------------
   Poll Function
   ------------------------------------------------------------------
*/
void Poll(unsigned long WaitTime)
{
  time_now = millis();
  while (millis() - time_now < WaitTime)
  {
    mycc2530.POLL();
  }
}

/* ------------------------------------------------------------------
   Interview for Zigbee2MQTT
   ------------------------------------------------------------------
*/
void Interview(unsigned long WaitTime)
{
  time_now = millis();
  while (millis() - time_now < WaitTime)
  {
    mycc2530.POLL();                                                  // Need to constantly Poll for data from the CC2530 to the application processor

    if (mycc2530.AF_INCOMING_MSG())                                   // Function returns true if a AF_INCOMING_MSG is received
    { // Process Incoming Messages in this block of code
      mycc2530.LINK_QUALITY();                                        // Function prints the Short Address and Link Quality from a received message.  The link quality is from the last Hop to the receiving device.
      
      uint8_t ClusterID[2];
      ClusterID[0] = mycc2530.ReceivedBytes[5];
      ClusterID[1] = mycc2530.ReceivedBytes[6];

      if (ClusterID[0] == 0x00 && ClusterID[1] == 0x00)
      {
        ZCL_Interview();                                              // Send ZCL Read attributes response.  This is used in the zigbee2mqtt interview process.
      }
      mycc2530.LINK_QUALITY();                                        // Function prints the Short Address and Link Quality from a received message.  The link quality is from the last Hop to the receiving device.
    }
  }
}

/* ------------------------------------------------------------------
   Sleep Function
   ------------------------------------------------------------------
*/
void Sleep()
{
  SPI.end();                                                            // Need change state of SPI pins to reduce sleep current
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);

  pinMode(PIN_MISO, INPUT_PULLUP);                                      // Needs to be pulled HIGH during sleep.  CC2530 MISO pin needs to be pulled high to reduce sleep current.

  Serial.println("~~SLEEP~~");
  delay(50);                                                            // Delay required to allow time to print "SLEEP" to serial monitor prior to sleeping
  for (int sleepCounter = sleepcount; sleepCounter > 0; sleepCounter--) // Enter power down state with ADC and BOD module disabled
  {
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);                     // Watchdog timer can only sleep maximum of 8 seconds, so loop this function
  }

  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(6, INPUT);

  delay(100);                                                           // Delay needed to stabalise after sleep and print "WAKE"
  Serial.println("~~WAKE~~");
  SPI.begin();
  delay(100);                                                           // Delay needed after SPI begin to ensure POLL command can use SPI
}

/* ------------------------------------------------------------------
   AF_Register Function
   ------------------------------------------------------------------
*/
void AF_REGISTER(uint8_t EndPoint)
{
  uint8_t Len = 0x0F;                                               // total length after Cmd1 excl. XOR
  uint8_t Cmd0 = 0x24;
  uint8_t Cmd1 = 0x00;
  uint8_t AppEndPoint = EndPoint;                                   // Register End Point
  uint8_t AppProfileID0 = 0x04;
  uint8_t AppProfileID1 = 0x01;
  uint8_t DeviceID0 = 0x00;
  uint8_t DeviceID1 = 0x00;
  uint8_t DeviceVer = 0x01;
  uint8_t LatencyReq = 0x00;
  uint8_t AppNumInClusters = 0x03;
  uint8_t InCluster0 = 0x00;
  uint8_t InCluster1 = 0x00;
  uint8_t InCluster2 = 0x02;
  uint8_t InCluster3 = 0x04;
  uint8_t InCluster4 = 0x05;
  uint8_t InCluster5 = 0x04;
  uint8_t AppNumOutClusters = 0x00;

  // Make array
  uint8_t AFRegister[] = {Len, Cmd0, Cmd1, AppEndPoint, AppProfileID0, AppProfileID1, DeviceID0, DeviceID1, DeviceVer, LatencyReq, AppNumInClusters, InCluster0, InCluster1, InCluster2, InCluster3, InCluster4, InCluster5, AppNumOutClusters};

  DEBUG_SERIAL.println(F("AF_REGISTER SREQ"));
  // SREQ
  digitalWrite(PIN_SS_MRDY, LOW);
  while (digitalRead(PIN_SRDY) == HIGH) {};
  SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
  for (int i = 0; i < sizeof(AFRegister); i++)
  {
    SPI.transfer(AFRegister[i]);
  }
  while (digitalRead(PIN_SRDY) == LOW) {};

  mycc2530.SRSP();
}

/* ------------------------------------------------------------------
   ZCL Report Model Identifier and Application Version
   ------------------------------------------------------------------
*/

void ZCLFrame_ReportAttributes()
{
  // ZCL Header
  uint8_t FrameControl = 0x18;                                      // Bitfield that defines the command type and other relevant information in the ZCL command
  uint8_t SequenceNumber = 0x00;                                    // A sequence number used to correlate a ZCL command with a ZCL response.
  uint8_t CommandID = 0x0a;                                         // CommandID: Report Attributes
  uint8_t ZCLHeader[] = {FrameControl, SequenceNumber, CommandID};  // ZCL Header

  // ZCL Payload
  uint8_t AttributeID_ModelID[2] = {0x05, 0x00};                    // Attribute ID: Model Identifier
  uint8_t DataType_ModelID = 0x42;                                  // Data Type: Character String
  uint8_t Len = 0x0b;                                               // Length of string
  uint8_t string[] = {0x56, 0x54, 0x31, 0x31, 0x30, 0x30, 0x44, 0x48, 0x54, 0x31, 0x31}; // Model Identifier: VT1100DHT11

  uint8_t AttributeID_AppVer[2] = {0x01, 0x00};                     // Attribute ID: Application Version
  uint8_t DataType_AppVer = 0x20;                                   // Data Type: uint8_t
  uint8_t AppVer = 0x01;                                            // Application version: 1
  uint8_t ZCLPayload[] = {AttributeID_ModelID[0], AttributeID_ModelID[1], DataType_ModelID, Len, string[0], string[1], string[2], string[3], string[4], string[5], string[6], string[7], string[8], string[9], string[10], AttributeID_AppVer[0], AttributeID_AppVer[1], DataType_AppVer, AppVer};

  // ZCL Frame
  uint8_t ZCLFrame[sizeof(ZCLHeader) + sizeof(ZCLPayload)];
  memcpy(ZCLFrame, ZCLHeader, sizeof(ZCLHeader));
  memcpy(ZCLFrame + sizeof(ZCLHeader), ZCLPayload, sizeof(ZCLPayload));

  // Send ZCL Frame
  mycc2530.SetAF_DATA_REQUEST(0x01, 0x01, 0x00, 0x00, 0x00, 0x10, 0x30); // Destination EP, Source EP, ClusterID, ClusterID, Trans ID, Options, Radius
  mycc2530.AF_DATA_REQUEST(0x00, 0x00, ZCLFrame, sizeof(ZCLFrame));      // NwkAddr, NwkAddr, Payload to Send, Length of Payload
}

/* ------------------------------------------------------------------
   ZCL Read Attributes Response
   ------------------------------------------------------------------
*/
void ZCLFrame_ReadAttributesResponse(uint8_t Data[], uint8_t arraysize)
{
  // ZCL Header
  uint8_t FrameControl = 0x18;                                      // Bitfield that defines the command type and other relevant information in the ZCL command
  uint8_t SequenceNumber = seqNumber;                               // A sequence number used to correlate a ZCL command with a ZCL response.
  uint8_t CommandID = 0x01;                                         // Read Attributes Response
  const uint8_t ZCLHeader[] = {FrameControl, SequenceNumber, CommandID};  // ZCL Header

  // ZCL Payload
  uint8_t ZCLPayload[30];
  memcpy(ZCLPayload, Data, arraysize);

  // ZCL Frame
  uint8_t ZCLFrame[sizeof(ZCLHeader) + sizeof(ZCLPayload)];
  memcpy(ZCLFrame, ZCLHeader, sizeof(ZCLHeader));
  memcpy(ZCLFrame + sizeof(ZCLHeader), ZCLPayload, sizeof(ZCLPayload));
  uint8_t ZCLFrameLength = sizeof(ZCLHeader) + arraysize;

  // Send ZCL Frame
  mycc2530.SetAF_DATA_REQUEST(0x01, 0x01, 0x00, 0x00, 0x00, 0x10, 0x30);
  mycc2530.AF_DATA_REQUEST(0x00, 0x00, ZCLFrame, ZCLFrameLength);
}

/* ------------------------------------------------------------------
   ZCL_Interview
   ------------------------------------------------------------------
*/
void ZCL_Interview()
{
  uint8_t ControlField = mycc2530.ReceivedBytes[20];
  seqNumber = mycc2530.ReceivedBytes[21];
  uint8_t Command = mycc2530.ReceivedBytes[22];
  uint16_t Attribute[2];
  Attribute[0] = mycc2530.ReceivedBytes[23];
  Attribute[1] = mycc2530.ReceivedBytes[24];

  if (Attribute[0] == 0x05 && Attribute[1] == 0x00)
  {
    ZCLFrame_ReadAttributesResponse(ModelIdentifier, sizeof(ModelIdentifier));
    mycc2530.RECV_CALLBACK();
  }
  else if (Attribute[0] == 0x04 && Attribute[1] == 0x00)
  {
    ZCLFrame_ReadAttributesResponse(ManufacturerName, sizeof(ManufacturerName));
    mycc2530.RECV_CALLBACK();
  }
  else if (Attribute[0] == 0x07 && Attribute[1] == 0x00)
  {
    ZCLFrame_ReadAttributesResponse(PowerSource, sizeof(PowerSource));
    mycc2530.RECV_CALLBACK();
  }
  else if (Attribute[0] == 0x01 && Attribute[1] == 0x00)
  {
    ZCLFrame_ReadAttributesResponse(ApplicationVersion, sizeof(ApplicationVersion));
    mycc2530.RECV_CALLBACK();
  }
  else if (Attribute[0] == 0x00 && Attribute[1] == 0x00)
  {
    ZCLFrame_ReadAttributesResponse(ZCLVersion, sizeof(ZCLVersion));
    mycc2530.RECV_CALLBACK();
  }
  else if (Attribute[0] == 0x02 && Attribute[1] == 0x00)
  {
    ZCLFrame_ReadAttributesResponse(StackVersion, sizeof(StackVersion));
    mycc2530.RECV_CALLBACK();
  }
  else if (Attribute[0] == 0x03 && Attribute[1] == 0x00)
  {
    ZCLFrame_ReadAttributesResponse(HWVersion, sizeof(HWVersion));
    mycc2530.RECV_CALLBACK();
  }
  else if (Attribute[0] == 0x06 && Attribute[1] == 0x00)
  {
    ZCLFrame_ReadAttributesResponse(Datecode, sizeof(Datecode));
    mycc2530.RECV_CALLBACK();
  }
  else if (Attribute[0] == 0x00 && Attribute[1] == 0x40)
  {
    ZCLFrame_ReadAttributesResponse(SoftwareBuildID, sizeof(SoftwareBuildID));
    mycc2530.RECV_CALLBACK();
  }
}

/* ------------------------------------------------------------------
   ZCL Report Temperature
   ------------------------------------------------------------------
*/

void ZCLFrame_ReportAttributes_Temp()
{
  // ZCL Header
  uint8_t FrameControl = 0x18;                                      // Bitfield that defines the command type and other relevant information in the ZCL command
  uint8_t SequenceNumber = seqCount;                                // A sequence number used to correlate a ZCL command with a ZCL response.
  seqCount++;
  uint8_t CommandID = 0x0a;                                         // CommandID: Report Attributes
  uint8_t ZCLHeader[] = {FrameControl, SequenceNumber, CommandID};  // ZCL Header

  // ZCL Payload
  uint8_t AttributeID_MeasuredVal[2] = {0x00, 0x00};                // Attribute ID: Measured Value
  uint8_t DataType = 0x29;                                          // Data Type: 16 bit integer Temperature measurement
  int Temp = DHT.temperature * 100;                                 // Temperature value
  uint8_t ZCLPayload[] = {AttributeID_MeasuredVal[0], AttributeID_MeasuredVal[1], DataType, lowByte(Temp), highByte(Temp)}; // ZCL Payload

  // ZCL Frame
  uint8_t ZCLFrame[sizeof(ZCLHeader) + sizeof(ZCLPayload)];
  memcpy(ZCLFrame, ZCLHeader, sizeof(ZCLHeader));
  memcpy(ZCLFrame + sizeof(ZCLHeader), ZCLPayload, sizeof(ZCLPayload));

  // Send ZCL Frame
  mycc2530.SetAF_DATA_REQUEST(0x01, 0x01, 0x02, 0x04, 0x00, 0x00, 0x30);  // Destination EP, Source EP, ClusterID, ClusterID, Trans ID, Options, Radius
  mycc2530.AF_DATA_REQUEST(0x00, 0x00, ZCLFrame, sizeof(ZCLFrame));       // NwkAddr, NwkAddr, Payload to Send, Length of Payload
}

/* ------------------------------------------------------------------
   ZCL Report Humidity
   ------------------------------------------------------------------
*/

void ZCLFrame_ReportAttributes_Humidity()
{
  // ZCL Header
  uint8_t FrameControl = 0x18;                                      // Bitfield that defines the command type and other relevant information in the ZCL command
  uint8_t SequenceNumber = seqCount;                                // A sequence number used to correlate a ZCL command with a ZCL response.
  seqCount++;
  uint8_t CommandID = 0x0a;                                         // CommandID: Report Attributes
  uint8_t ZCLHeader[] = {FrameControl, SequenceNumber, CommandID};  // ZCL Header

  // ZCL Payload
  uint8_t AttributeID_MeasuredVal[2] = {0x00, 0x00};                // Attribute ID: Measured Value
  uint8_t DataType = 0x21;                                          // Data Type: 16 bit integer Humidity Measurement
  int Humidity = DHT.humidity * 100;                                // Humidity value
  uint8_t ZCLPayload[] = {AttributeID_MeasuredVal[0], AttributeID_MeasuredVal[1], DataType, lowByte(Humidity), highByte(Humidity)}; // ZCL Payload

  // ZCL Frame
  uint8_t ZCLFrame[sizeof(ZCLHeader) + sizeof(ZCLPayload)];
  memcpy(ZCLFrame, ZCLHeader, sizeof(ZCLHeader));
  memcpy(ZCLFrame + sizeof(ZCLHeader), ZCLPayload, sizeof(ZCLPayload));

  // Send ZCL Frame
  mycc2530.SetAF_DATA_REQUEST(0x01, 0x01, 0x05, 0x04, 0x00, 0x00, 0x30);  // Destination EP, Source EP, ClusterID, ClusterID, Trans ID, Options, Radius
  mycc2530.AF_DATA_REQUEST(0x00, 0x00, ZCLFrame, sizeof(ZCLFrame));       // NwkAddr, NwkAddr, Payload to Send, Length of Payload
}
