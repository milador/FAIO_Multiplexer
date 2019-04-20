 # FAIO Multiplexer Switch 

FAIO Multiplexer is an open-source Assistive technology wing for Adafruit Feather boards which enables those with limited or no hand movement to use Adaptive switches as input to operate in multiple input modes.

FAIO Multiplexer supports following switch modes:

* Switch Access Mode ( HID Keyboard )
* Morse Keyboard Mode ( HID Keyboard )
* Morse Mouse Mode ( HID Mouse )
* Joystick Mode ( HID Joystick )

Additional firmware to operate Xbox Adaptive Controller using switch module is available as well.

# Downloads 

These are all the files and documentation associated with the FAIO Multiplexer project.

 <table style="width:100%">
  <tr>
    <th>Resource</th>
    <th>Version</th>
    <th>Format</th>
    <th>Link</th>
  </tr>
    <tr>
    <td>FAIO Multiplexer All</td>
    <td>2.0</td>
    <td>ZIP</td>
    <td><a href="https://github.com/milador/FAIO-Multiplexer/archive/master.zip">FAIO-Multiplexer-master.zip</a></td>
  </tr>
  <tr>
    <td>FAIO-Multiplexer Manual</td>
    <td></td>
    <td>PDF</td>
    <td><a href=""> </a></td>
  </tr>
  <tr>
    <td>FAIO-Multiplexer Switch BOM (csv)</td>
    <td>February 18, 2019</td>
    <td>CSV</td>
    <td><a href="https://github.com/milador/FAIO-Multiplexer/blob/master/Components/FAIO-Multiplexer_BOM.csv">FAIO-Multiplexer_BOM.csv</a></td>
  </tr>
  <tr>
    <td>FAIO-Multiplexer USB Switch Software</td>
    <td>1.0</td>
    <td>Ino</td>
    <td><a href="https://github.com/milador/FAIO-2/raw/master/Software/FAIO_Multiplexer_USB_Software/FAIO_Multiplexer_USB_Software.ino">FAIO_Multiplexer_USB_Software.ino</a></td>
  </tr>
  <tr>
    <td>FAIO-Multiplexer XAC Switch Software</td>
    <td>1.0</td>
    <td>Ino</td>
    <td><a href="https://github.com/milador/FAIO-2/raw/master/Software/FAIO_Multiplexer_XAC_Software/FAIO_Multiplexer_XAC_Software.ino">FAIO_Multiplexer_XAC_Software.ino</a></td>
  </tr>
  <tr>
    <td>FAIO-Multiplexer Switch Board Layout</td>
    <td>1.0</td>
    <td>BRD</td>
    <td><a href="https://raw.githubusercontent.com/milador/FAIO-Multiplexer/master/Hardware/PCB_design/FAIO_Multiplexer.brd">FAIO_Multiplexer.brd</a></td>
  </tr>
  <tr>
    <td>FAIO-Multiplexer Switch Board Schematic</td>
    <td>1.0</td>
    <td>SCH</td>
    <td><a href="https://raw.githubusercontent.com/milador/FAIO-Multiplexer/master/Hardware/PCB_design/FAIO_Multiplexer.sch">FAIO_Multiplexer.sch</a></td>
  </tr>
</table> 

# Usage

The FAIO Multiplexer switch interface can operate in 4 modes and switch D is used to switch between them in USB and Bluetooth modes.The RGB Led blinks 2 times to indicate the current mode.

<p align="center">
<img align="center" src="https://raw.githubusercontent.com/milador/FAIO-Multiplexer/master/Resources/Images/faio_multiplexers.png" width="50%" height="50%" alt="FAIO-Multiplexer boards"/>
</p>

## USB or Bluetooth Switch mode

 <table style="width:100%">
  <tr>
    <th>Mode Number</th>
    <th>Mode</th>
    <th>Color</th>
  </tr>
    <tr>
    <td>1</td>
    <td>Keyboard Switch</td>
    <td>Green</td>
  </tr>
  <tr>
    <td>2</td>
    <td>Morse Keyboard</td>
    <td>Pink</td>
  </tr>
  <tr>
    <td>3</td>
    <td>Morse Mouse</td>
    <td>Yellow</td>
  </tr>
  <tr>
    <td>4</td>
    <td>Joystick</td>
    <td>Orange</td>
  </tr>
  <tr>
    <td>5</td>
    <td>Configuration</td>
    <td>Purple</td>
  </tr>
</table> 

<p align="center">
<img align="center" src="https://raw.githubusercontent.com/milador/FAIO-Multiplexer/master/Resources/Images/faio_multiplexer.png" width="50%" height="50%" alt="FAIO-Multiplexer board"/>
</p>

## XAC Switch mode

 <table style="width:100%">
  <tr>
    <th>Button Number</th>
    <th>Xbox Button</th>
    <th>Color</th>
  </tr>
    <tr>
    <td>1</td>
    <td>A</td>
    <td>Green</td>
  </tr>
  <tr>
    <td>2</td>
    <td>B</td>
    <td>Red</td>
  </tr>
  <tr>
    <td>3</td>
    <td>X1</td>
    <td>Blue</td>
  </tr>
  <tr>
    <td>4</td>
    <td>X2</td>
    <td>Yellow</td>
  </tr>
</table> 

## Software Setup Instructions

  1. Install the required libraries 
  
  1.1. Install Neo Pixels : https://github.com/adafruit/Adafruit_NeoPixel

  1.2. Install EasyMorse if using USB or Bluetooth version : https://github.com/milador/EasyMorse
  
  1.3. Install StopWatch if using USB or Bluetooth version : https://github.com/RobTillaart/Arduino/tree/master/libraries/StopWatch
  
  1.4. Install FlashStorage if using Feather M0 Board : https://github.com/cmaglie/FlashStorage

  1.5. Install Joystick library: https://github.com/MHeironimus/ArduinoJoystickLibrary or https://github.com/gdsports/ArduinoJoystickLibrary/tree/samd_patch
  
  2. Download the firmware 
  
  2.1. USB Firmware : https://github.com/milador/FAIO-2/raw/master/Software/FAIO_USB_Firmware/FAIO_USB_Firmware.ino

  2.2. XAC Firmware : https://github.com/milador/FAIO-2/raw/master/Software/FAIO_XAC_Firmware/FAIO_XAC_Firmware.ino
  
  2.3. Bluetooth Firmware (Coming up)
  
  3. Setup Arduino IDE for your feather board according to the instructions on Adafruit website
  
  4. Verify and upload firmware code to your Feather Board
