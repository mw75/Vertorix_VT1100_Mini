<!-- PROJECT SHIELDS -->
<!--
*** I'm using markdown "reference style" links for readability.
*** Reference links are enclosed in brackets [ ] instead of parentheses ( ).
*** See the bottom of this document for the declaration of the reference variables
*** for contributors-url, forks-url, etc. This is an optional, concise syntax you may use.
*** https://www.markdownguide.org/basic-syntax/#reference-style-links
-->
[![Documents][Docs-shield]][Docs-url]
[![MIT License][license-shield]][license-url]

<!-- Title -->
# VT1100 Mini SPI

<!-- Introduction -->
This is the Arduino Library for the Vertorix VT1100 Mini SPI PCB.  The Vertorix VT1100 Mini is a low powered Zigbee development board compatible with the Arduino IDE.  

<!-- PROJECT LOGO -->
<!--
Place Logo here
-->
<br/>

<p align="center">
<img src="https://github.com/VertorixAU/VertorixAU.github.io/raw/main/Images/VT1100/VT1100Side.png" width="500" height="500">

<br/>

<!-- MOTIVATION -->
## Motivation

This Repository shows you how to use the VT1100 Library with the Arduino IDE and provides example sketches.

<!-- GETTING STARTED -->
## Getting Started

Follow the below instructions to run the example Arduino Sketches.

### Prerequisites

* VT1100 Mini;
* USB-to-TTL Serial Converter;
* Arduino IDE - download [here](https://www.arduino.cc/en/main/software)

### Connections

The VT1100 was made without built in USB to keep the board compact.  A separate USB to TTL Serial converter must be used to upload sketches.  This is the same method as used with the Arduino Pro Mini Boards.  There are many online tutorials for connecting USB-to-TTL Serial Converters.   

USB-to-TTL Serial Converters:
* CP2102 IC
* FTDI FT232RL USB to serial 3.3V IC

Disconnect any other power sources such as batteries from the VIN pin prior to connecting to the USB port of your computer.

### Board Installation in Arduino IDE

Install the Vertorix Boards into the Arduino IDE.

1. Open the Arduino IDE.  From the menu select:
```sh
File > Preferences > Additional Boards Manager URLs
```
Paste link:
```sh
https://raw.githubusercontent.com/VertorixAU/VertorixBoards/main/BoardManager/package_vertorix_index.json
```
2. Click OK and from the menu select:
```sh
Tools > Board > Board Manager
```
3. Use the search bar at the top to search for **Vertorix**;
4. Click on Vertorix and then click Install.

### Libraries

#### Manually Install Libraries

1. Copy the library **VT1100MiniSPI.h** and **VT1100MiniSPI.cpp** files to your Arduino "libraries" folder;
2. Run the example sketches in the Arduino IDE.

#### Arduino IDE Library Installation

1. Use the menu to select:
```sh
Sketch > Include Library > Manage Libraries
```
2. Use the search bar at the top to search for **Vertorix**;
3. Find the Vertorix VT100 Library and click the install button;

### Running the Examples

1. Open the Example Sketches from the menu:
```sh
File > Examples > Select Library name > Select Example
```
2. Open the **VT1100_SimpleReceive.ino** and **VT1100_SimpleSend.ino** Examples;
3. Read through the Sketches to obtain a basic understanding of how they work;
4. Set Digital Pin 2 (**D2**) LOW (GND) (this will Commission the CC2530 into the Network on power up).  Connect the two VT1100 boards to the computer using serial to USB converters.
5. Select the correct Board, Processor and Ports then Upload the Sketches;
```sh
Tools > Board > Vertorix > "VT1100 Mini SPI"
Tools > Processor: "Atmega328P (3.3V, 8 MHz)"
Tools > Port: "COM#"
Sketch > Upload
```
6. First open the Serial Monitor on the **VT1100_SimpleReceive.ino** Sketch and it will start this device as a Coordinator and form the Network;
7. Then open the Serial Monitor on the **VT1100_SimpleSend.ino** Sketch and it will join the Network as a Router and send messages to the Coordinator every 10 seconds;
8. Set D2 High (disconnected) once the devices are commissioned in the network.  This will allow the devices to restore Network States and Configurations on reset.


<!-- LICENSE -->
## License

Distributed under the MIT License. See `LICENSE` for more information.

<!-- CONTACT -->
## Contact

Project Link: [https://vertorixau.github.io/](https://vertorixau.github.io/)


<!-- MARKDOWN LINKS & IMAGES -->
<!-- Douments Shield -->
[Docs-shield]: https://img.shields.io/badge/Docs-Project%20Documentation-blue
[Docs-url]: https://vertorixau.github.io/
<!-- License Shield -->
[license-shield]: https://img.shields.io/badge/License-MIT-brightgreen
[license-url]: https://github.com/VertorixAU/Vertorix_VT1100_Mini_SPI/blob/main/LICENSE
