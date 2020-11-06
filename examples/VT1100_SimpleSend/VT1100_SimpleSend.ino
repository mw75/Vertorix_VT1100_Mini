/*
  Board: VT1100MiniSPI
  Example: Simple Send
  Description: A simple example that starts the CC2530 as a Router and sends messages once every 10 seconds.
*/

#include <VT1100MiniSPI.h>
#include <SPI.h>

#define InitButton 2                                                  // Button on Digital Pin 2 (D2)

// Library Class Instance
CC2530 myCC2530;                                                      // Make an instance of the class from the Library.  A short name for referring to variables and functions from the Libraries class.

// Millis timing
int period = 10000;
unsigned long time_now = 0;

void setup()
{
  pinMode(InitButton, INPUT_PULLUP);                                  // Button to control if the CC2530 should be commissioned.

  SPI.begin();
  Serial.begin(115200);
  delay(100);

  // Parameters for CC2530
  myCC2530.SetPANID(0x00A2);                                          // PAN ID.  Two bytes set between 0x0000 and 0x3FFF.  Examples: 0x00A1, 0x00A2 or 0x00A3.
  myCC2530.SetLOGICAL_TYPE(0x01);                                     // Device type.  Examples: Coordinator = 0x00, Roouter = 0x01 or End Device = 0x02
  myCC2530.SetCHANLIST(11);                                           // Wireless Channel. Examples: 11 to 26

  myCC2530.POWER_UP();                                                // Power up the CC2530 (wake on reset).

  if (digitalRead(InitButton) == LOW)
  {
    myCC2530.COMMISSION();                                            // Clears the configuration and network state then writes the new configuration parameters to the CC2530 non-volitile (NV) memory.  This should only be run once on initial setup.
  }

  myCC2530.AF_REGISTER(0x01);                                         // Register Endpoint
  myCC2530.ZDO_STARTUP_FROM_APP();                                    // Starts the CC2530 in the network
}

void loop()
{
  myCC2530.POLL();                                                    // Need to constantly POLL the CC2530 to see if it has any queued data to send to the application processor

  if (millis() >= time_now + period)                                  // Functions runs every 10 seconds
  {
    time_now += period;
    uint8_t Data = 0x08;                                              // Data to send
    myCC2530.AF_DATA_REQUEST(0x00, 0x00, Data, sizeof(Data));         // AF_DATA_REQUEST(Short Address, Short Address, Variable to send, Length).  The coordinators address is always 0x0000.
  }
}
