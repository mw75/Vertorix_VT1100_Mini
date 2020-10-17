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
This is the Arduino Library for the Vertorix VT1100 Mini SPI PCB.

<!-- PROJECT LOGO -->
<!--
Place Logo here
-->
<br/>

<p align="center">
<img src="https://github.com/VertorixAU/VertorixAU.github.io/raw/main/Images/VT1100/VT1100Side.jpg" width="300" height="300">

<br/>

<!-- MOTIVATION -->
## Motivation

This project provides an easy method to setup low cost wireless mesh networks that can be used in Internet of Things (IOT) applications.

<!-- GETTING STARTED -->
## Getting Started

Follow the below instructions to run the example Arduino Sketches.

### Prerequisites

* Minimum of two VT1100 boards to establish a wireless network;
* Arduino IDE - download [here](https://www.arduino.cc/en/main/software)

### Installation

#### Boards
Install the Vertorix Boards into the Arduino IDE

1. Open the Arduino IDE.  Select:
```sh
File > Preferences > Additional Boards Manager URLs
```
```sh
Paste this link: https://raw.githubusercontent.com/VertorixAU/ArduinoBoards/main/BoardManager/package_vertorix_index.json
```
2. From the menu select:
```sh
Tools > Board > Board Manager
```
3. Use the search bar at the top to search for **Vertorix**;
4. Click on Vertorix and then click Install.

#### Libraries
Install the Vertorix Library

1. Use the menu to select:
```sh
Sketch > Include Library > Manage Libraries
```
2. Use the search bar at the top to search for **Vertorix**;
3. Find the Vertorix VT100 Library and click the install button;

#### Running the Examples
1. Open the Example Sketches from the menu:
```sh
File > Examples > Select Library name > Select Example
```
2. Open the **VT1100_SimpleReceive.ino** and **VT1100_SimpleSend.ino** Examples;
3. Read through the Sketches to obtain a basic understanding of how they work;
4. Connect the two VT1100 boards to the computer using serial to USB converters and set Digital Pin 2 (**D2**) LOW (GND) (this will Commission the CC2530 into the Network on power up).  If you have a second computer available I recommend installing the Arduino IDE on both and running the Send / Receive sketches separately.  This makes debugging easier when selecting COM ports etc.;
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
[license-shield]: https://img.shields.io/github/license/VertorixAU/CC2530.svg?style=flat-square
[license-url]: https://github.com/VertorixAU/CC2530/blob/master/LICENSE
