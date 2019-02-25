
//FFFFFFFFFFFFFFFFFFFFFF               AAA               IIIIIIIIII     OOOOOOOOO           222222222222222                 000000000     
//F::::::::::::::::::::F              A:::A              I::::::::I   OO:::::::::OO        2:::::::::::::::22             00:::::::::00   
//F::::::::::::::::::::F             A:::::A             I::::::::I OO:::::::::::::OO      2::::::222222:::::2          00:::::::::::::00 
//FF::::::FFFFFFFFF::::F            A:::::::A            II::::::IIO:::::::OOO:::::::O     2222222     2:::::2         0:::::::000:::::::0
//  F:::::F       FFFFFF           A:::::::::A             I::::I  O::::::O   O::::::O                 2:::::2         0::::::0   0::::::0
//  F:::::F                       A:::::A:::::A            I::::I  O:::::O     O:::::O                 2:::::2         0:::::0     0:::::0
//  F::::::FFFFFFFFFF            A:::::A A:::::A           I::::I  O:::::O     O:::::O              2222::::2          0:::::0     0:::::0
//  F:::::::::::::::F           A:::::A   A:::::A          I::::I  O:::::O     O:::::O         22222::::::22           0:::::0 000 0:::::0
//  F:::::::::::::::F          A:::::A     A:::::A         I::::I  O:::::O     O:::::O       22::::::::222             0:::::0 000 0:::::0
//  F::::::FFFFFFFFFF         A:::::AAAAAAAAA:::::A        I::::I  O:::::O     O:::::O      2:::::22222                0:::::0     0:::::0
//  F:::::F                  A:::::::::::::::::::::A       I::::I  O:::::O     O:::::O     2:::::2                     0:::::0     0:::::0
//  F:::::F                 A:::::AAAAAAAAAAAAA:::::A      I::::I  O::::::O   O::::::O     2:::::2                     0::::::0   0::::::0
//FF:::::::FF              A:::::A             A:::::A   II::::::IIO:::::::OOO:::::::O     2:::::2       222222        0:::::::000:::::::0
//F::::::::FF             A:::::A               A:::::A  I::::::::I OO:::::::::::::OO      2::::::2222222:::::2 ......  00:::::::::::::00 
//F::::::::FF            A:::::A                 A:::::A I::::::::I   OO:::::::::OO        2::::::::::::::::::2 .::::.    00:::::::::00   
//FFFFFFFFFFF           AAAAAAA                   AAAAAAAIIIIIIIIII     OOOOOOOOO          22222222222222222222 ......      000000000    

//FAIO 2 USB Switch Software
//FAIO_USB_Switch_Software
//Written by: Milad Hajihassan
//Version 1.0 (23/2/2019)
//Github Link: https://github.com/milador/FAIO-2

#include "Mouse.h"
#include "Keyboard.h"
#include <StopWatch.h>
#include "EasyMorse.h"
#include "Joystick.h"
#include <math.h>
#include <Adafruit_NeoPixel.h>

//Use FlashStorage library for M0 Boards and EEPROM for Atmega32U4 boards
#ifdef defined(ARDUINO_SAMD_FEATHER_M0)
#include <FlashStorage.h>
#endif
#ifdef defined(__AVR_Atmega32U4__)
#include <EEPROM.h>
#endif

//Can be changed based on the needs of the users 
#define FIXED_DELAY 30
#define MOUSE_MOVE_MULTI 2
#define MORSE_RESULT_TIME 5000
#define JOYSTICK_DEADZONE 5

//Define Switch pins
#define LED_PIN 12
#define SWITCH1_PIN A0
#define SWITCH2_PIN A2
#define SWITCH3_PIN 11
#define SWITCH4_PIN 6

//Define Joystick pins (x and y are reversed 
#define Y_JOYSTICK_PIN A4
#define X_JOYSTICK_PIN A5  

EasyMorse morse;

// Variable Declaration

//Declare switch state variables for each switch
int switch1State;
int switch2State;
int switch3State;
int switch4State;

//Declare joystick variables 
int xJoystick;
int yJoystick;
bool availableJoystick = true;                                          //Turn it to off if joystick is not used

//Stopwatches array used to time switch presses
StopWatch SW[3];

//Initialize time variables for morse code
unsigned msMin = MS_MIN_DD;
unsigned msMax = MS_MAX_DD;
unsigned msEnd = MS_END;
unsigned msClear = MS_CL;

//Declare Switch variables for settings 
int switchConfigured;
int switchDelay;
int switchSpeedLevel;
int switchMode;

//Declare Flash storage variables 
#ifdef defined(ARDUINO_SAMD_FEATHER_M0)
FlashStorage(switchConfiguredFlash, int);
FlashStorage(switchSpeedLevelFlash, int);
FlashStorage(switchModeFlash, int);
#endif


//RGB LED Color code structure 

struct rgbColorCode {
    int r;    // red value 0 to 255
    int g;   // green value
    int b;   // blue value
 };

//Color structure 
typedef struct { 
  uint8_t colorNumber;
  String colorName;
  rgbColorCode colorCode;
} colorStruct;

 //Mode structure 
typedef struct { 
  uint8_t modeNumber;
  String modeName;
  uint8_t modeColorNumber;
} modeStruct;

 //Joystick structure 
typedef struct { 
  uint8_t switchNumber;
  uint8_t joystickButton;
} joystickStruct;

//Color properties 
const colorStruct colorProperty[] {
    {0,"Green",{0,50,0}},
    {1,"Pink",{50,00,20}},
    {2,"Yellow",{50,50,0}},    
    {3,"Orange",{50,20,0}},
    {4,"Blue",{0,0,50}},
    {5,"Red",{50,0,0}},
    {6,"Purple",{50,0,50}},
    {7,"Teal",{0,128,128}}       
};

//Mode properties 
const modeStruct modeProperty[] {
    {0,"Keyboard Switch",0},
    {1,"Morse Keyboard",1},
    {2,"Morse Mouse",2},
    {3,"Joystick",3},
    {4,"Configuration",6}
};

//Joystick properties (switch number , joystick button -1 )
const joystickStruct joystickProperty[] {
    {0,0},
    {1,1},
    {2,2}
};

//Setup NeoPixel LED
Adafruit_NeoPixel ledPixels = Adafruit_NeoPixel(1, LED_PIN, NEO_GRB + NEO_KHZ800);

//Defining the joystick REPORT_ID and profile type
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, 
  JOYSTICK_TYPE_JOYSTICK, 8, 0,
  true, true, false, 
  false, false, false,
  false, false, 
  false, false, false);     



void setup() {

  ledPixels.begin();                                                           //Start NeoPixel
  Serial.begin(115200);                                                        //Start Serial
  switchSetup();                                                               //Setup switch
  morseSetup();                                                                //Setup morse

  displayFeatureList();
  
  //Initialize the switch pins as inputs
  pinMode(SWITCH1_PIN, INPUT_PULLUP);    
  pinMode(SWITCH2_PIN, INPUT_PULLUP);   
  pinMode(SWITCH3_PIN, INPUT_PULLUP);    
  pinMode(SWITCH4_PIN, INPUT_PULLUP);  

  //Initialize the joystick pins as analog inputs
  pinMode(X_JOYSTICK_PIN, INPUT);
  pinMode(Y_JOYSTICK_PIN, INPUT);

  //Initialize the LED pin as an output
  pinMode(LED_PIN, OUTPUT);                                                      


};

void loop() {

  //Update status of switch inputs
  switch1State = digitalRead(SWITCH1_PIN);
  switch2State = digitalRead(SWITCH2_PIN);
  switch3State = digitalRead(SWITCH3_PIN);
  switch4State = digitalRead(SWITCH4_PIN);

  //Update status of joystick inputs and check if joystick is available 
  if(availableJoystick)
  {
    xJoystick = analogRead(X_JOYSTICK_PIN);
    yJoystick = analogRead(Y_JOYSTICK_PIN);
  } else {
    xJoystick=0;
    yJoystick=0;
  }

  //Check if switch 4 is pressed to change switch mode
  if (switch4State == LOW) { 
    changeSwitchMode();
  }

  //Perform actions based on the mode
  switch (switchMode) {
      case 0:
        keyboardAction(switch1State,switch2State,switch3State,switch4State);                //Switch mode
        break;
      case 1:
        morseAction(1,switch1State,switch2State,switch4State);                              //Keyboard Morse mode
        break;
      case 2:
        morseAction(2,switch1State,switch2State,switch4State);                              //Mouse Morse mode
        break;
      case 3:
        joystickAction(switch1State,switch2State,switch3State,switch4State,xJoystick,yJoystick); //Joystick mode
        break;
      case 4:
        configAction(switch1State,switch2State,switch3State,switch4State);                  //Configuration mode
        break;
  };

    ledPixels.show();                                                                        //Update pixel color through the hardware.

}

//***DISPLAY FEATURE LIST FUNCTION***//

void displayFeatureList(void) {

  Serial.println(" ");
  Serial.println(" --- ");
  Serial.println("This is the FAIO 2 USB firmware");
  Serial.println(" ");
  Serial.println("VERSION: 1.0 (23 Jan 2019)");
  Serial.println(" ");
  Serial.println(" --- ");
  Serial.println("Features: Adaptive switch, Morse Keyboard, Morse Mouse, and Joystick");
  Serial.println(" --- ");
  Serial.println(" ");

}

//***RGB LED FUNCTION***//

void ledAction(int numBlinks, int delayBlinks, int ledColor) {
  if (numBlinks < 0) numBlinks *= -1;

      for (int i = 0; i < numBlinks; i++) {
        ledPixels.setPixelColor(0, ledPixels.Color(colorProperty[ledColor].colorCode.g,colorProperty[ledColor].colorCode.r,colorProperty[ledColor].colorCode.b));
        ledPixels.show(); 
        delay(delayBlinks);
        ledPixels.setPixelColor(0, ledPixels.Color(0,0,0));
        ledPixels.show(); 
        delay(delayBlinks);
      }
}

//***CLEAR RGB LED FUNCTION***//

void ledClear() {
  ledPixels.setPixelColor(0, ledPixels.Color(0,0,0));
  ledPixels.show(); 
}

//***SETUP SWITCH HID PROFILES FUNCTION (LOAD HID PROFILES)***//

void switchHidSetup(){
    if (switchMode == 0) {
      Keyboard.begin();                                                             //Start keyboard emulation 
      Mouse.end();
      Joystick.end(); 
    }
    else if (switchMode == 1) {
      Keyboard.begin();                                                             //Start keyboard emulation 
      Mouse.end();
      Joystick.end(); 
    }
    else if (switchMode == 2) {
      Keyboard.end();                                                             //Start keyboard emulation 
      Mouse.begin();
      Joystick.end(); 
    }
    else if (switchMode == 3) {
      Keyboard.end();                                                             //Start keyboard emulation 
      Mouse.end();
      Joystick.begin(); 
      Joystick.setXAxisRange(-127, 127);
      Joystick.setYAxisRange(-127, 127);
    }
    else {
      Keyboard.end();                                                             //Start keyboard emulation 
      Mouse.end();
      Joystick.end(); 
    }
  
}

//***SETUP SWITCH MODE FUNCTION***//

void switchSetup() {
  //Check if it's first time running the code
  #ifdef defined(ARDUINO_SAMD_FEATHER_M0)
  switchConfigured = switchConfiguredFlash.read();
  #endif
  #ifdef defined(__AVR_Atmega32U4__)
  EEPROM.get(22, switchConfigured);
  delay(5);
    if(switchConfigured<0){ switchConfigured = 0; } 
  #endif
  if (switchConfigured==0) {
    //Define default settings if it's first time running the code
    switchSpeedLevel=6;
    switchMode=0;
    switchConfigured=1;

    //Write default settings to flash storage 
    #ifdef defined(ARDUINO_SAMD_FEATHER_M0)
    switchSpeedLevelFlash.write(switchSpeedLevel);
    switchModeFlash.write(switchMode);
    switchConfiguredFlash.write(switchConfigured);
    #endif
    #ifdef defined(__AVR_Atmega32U4__)
    EEPROM.put(6, switchSpeedLevel);
    delay(5);
    EEPROM.put(4, switchMode);
    delay(5);
    EEPROM.put(2, switchConfigured);
    delay(5);
    #endif
  } else {
    //Load settings from flash storage if it's not the first time running the code
    #ifdef defined(ARDUINO_SAMD_FEATHER_M0)
    switchSpeedLevel=switchSpeedLevelFlash.read();
    switchMode=switchModeFlash.read();
    #endif
    #ifdef defined(__AVR_Atmega32U4__)
    EEPROM.get(6, switchSpeedLevel);
    delay(5);
    EEPROM.get(4, switchMode);
    delay(5);
    #endif    
  }  

    //Serial print settings 
    Serial.print("Switch Mode: ");
    Serial.println(switchMode);

    Serial.print("Switch Speed Level: ");
    Serial.println(switchSpeedLevel);
    
    //Setup switch modes (HID profiles )
    switchHidSetup();
    //Led will blink in a color to show the current mode 
    ledAction(2,500,modeProperty[switchMode].modeColorNumber);
    //Calculate switch delay based on switchSpeedLevel
    switchDelay = ((11-switchSpeedLevel)*FIXED_DELAY)+50;
}

//***SETUP MORSE FUNCTION***//

void morseSetup() {
    morse.clear();
    msMin = 50;
    msMax = msEnd = msClear = MORSE_RESULT_TIME; 

}

//***ADAPTIVE SWITCH KEYBOARD FUNCTION***//

void keyboardAction(int switch1,int switch2,int switch3,int switch4) {
    if(!switch1) {
      ledAction(1,50,4);
      //Serial.println("a");
      Keyboard.press('a');
    } else if(!switch2) {
      ledAction(1,50,5);
      //Serial.println("b");
      Keyboard.press('b');
    } else if(!switch3) {
      ledAction(1,50,7);
      //Serial.println("c");
      Keyboard.press('c');
    } else
    {
      ledClear();
      Keyboard.releaseAll();
    }
    delay(switchDelay);

}

//***MORSE CODE TO MOUSE CONVERT FUNCTION***//

void morseAction(int mode,int switch1,int switch2,int switch4) {
  int i, j;
  static int ctr;
  unsigned long timePressed;
  unsigned long timeNotPressed;
  static int previousSwitch1;
  static int previousSwitch2;

  int isShown;                                                                                  //is character shown yet?
  int backspaceDone = 0;                                                                        //is backspace entered? 0 = no, 1=yes

  timePressed = timeNotPressed  = 0; 

  if (!ctr) {
    previousSwitch1 = LOW;  
    previousSwitch2 = LOW;  
    ctr++;
  }
   //Check if dot or dash switch is pressed
   if ((switch1 == LOW && previousSwitch1 == HIGH) || (switch2 == LOW && previousSwitch2 == HIGH)) {
      //Dot
     if (switch1 == LOW) { 
         ledAction(1,50,4);                                                                     //Blink blue once
         previousSwitch1 = LOW;
     } //Dash
     if (switch2 == LOW) {
         ledAction(1,50,5);                                                                     //Blink red once
         previousSwitch2 = LOW;
     }
     isShown = 0;                                                                               //A key is pressed but not Shown yet
     for (i=0; i<3; i++) SW[i].stop();                                                          //Stop end of char
     
     SW[0].reset();                                                                             //Reset and start the the timer
     SW[0].start();                                                                             
   }

 
 // Switch 1 or Dot was released
 if (switch1 == HIGH && previousSwitch1 == LOW) {
   previousSwitch1 = HIGH;
   
   backspaceDone = 0;                                                                           //Backspace is not entered 
   timePressed = SW[0].elapsed();                                                               //Calculate the time that key was pressed 
   for (i=0; i<3; i++) SW[i].stop();                                                            //Stop end of character and reset
   for (i=0; i<3; i++) SW[i].reset();                                                           
   SW[1].start();
  
  //Push dot to morse stack if it's pressed for specified time
    if (timePressed >= msMin && timePressed < msMax)
      morse.push(1);
   isShown = 0; 
 }

   //Switch 2 or Dash was released
   if (switch2 == HIGH && previousSwitch2 == LOW) {

    previousSwitch2 = HIGH;
    backspaceDone = 0;                                                                          //Backspace is not entered 
    timePressed = SW[0].elapsed();                                                              //Calculate the time that key was pressed 
    for (i=0; i<3; i++) SW[i].stop();                                                           //Stop end of character and reset
    for (i=0; i<3; i++) SW[i].reset();
    SW[1].start();
   
    //Push dash to morse stack if it's pressed for specified time
    if (timePressed >= msMin && timePressed < msMax)
       morse.push(2);
    isShown = 0; 
   }

 // Processing the backspace key if button 1 or button 2 are hold and pressed for defined time
  if (timePressed >= msMax && timePressed >= msClear && backspaceDone == 0 &&  mode== 1) {
    previousSwitch1 = HIGH;
    previousSwitch2 = HIGH;
    if(mode==1) Keyboard.write(8);                                                               //Press Backspace key
    backspaceDone = 1;                                                                          //Set Backspace done already
    isShown = 1;
    for (i=1; i<3; i++) { SW[i].stop(); SW[i].reset(); }                                        //Stop and reset end of character
 }

   //End the character if nothing pressed for defined time and nothing is shown already 
   timeNotPressed = SW[1].elapsed();
    if (timeNotPressed >= msMax && timeNotPressed >= msEnd && isShown == 0 && backspaceDone == 0) {
      if(mode==1) {
        Keyboard.write(morse.getCharAscii());                                                  //Enter keyboard key based on ascii code if it's in morse keyboard mode
      } else if (mode==2) {  
        int* mouseAct;
        mouseAct=morse.getMouse();
        mouseAction((int)mouseAct[0],(int)mouseAct[1],(int)mouseAct[2]);                        //Perform mouse action if it's in morse mouse mode
      }
      
      //Clean up morse code and get ready for next character
      morse.clear();
      isShown = 1;                                                                                //Set variable to is shown                                                                                      
      SW[1].stop();                                                                               //Stop and reset the timer to form a character
      SW[1].reset();
  }
  delay(switchDelay);
}


//***PERFORM MOUSE ACTIONS FUNCTION***//

void mouseAction(int button,int xValue,int yValue) {
    switch (button) {
      case 0:
        break;
      case 1:
        if (!Mouse.isPressed(MOUSE_LEFT)) {
          Mouse.press(MOUSE_LEFT);
          delay(50);
          Mouse.release(MOUSE_LEFT);
        } 
        else if (Mouse.isPressed(MOUSE_LEFT)) {
          Mouse.release(MOUSE_LEFT);
        }    
        break;
      case 2:
        if (!Mouse.isPressed(MOUSE_RIGHT)) {
          Mouse.press(MOUSE_RIGHT);
          delay(50);
          Mouse.release(MOUSE_RIGHT);
        }
        else if (Mouse.isPressed(MOUSE_RIGHT)) {
          Mouse.release(MOUSE_RIGHT);
        }   
        break;
      case 3:
        if (!Mouse.isPressed(MOUSE_LEFT)) {
          Mouse.press(MOUSE_LEFT);
          delay(50);
          Mouse.release(MOUSE_LEFT);
          delay(50);
          Mouse.press(MOUSE_LEFT);
          delay(50);
          Mouse.release(MOUSE_LEFT);                    
        }
        else if (Mouse.isPressed(MOUSE_LEFT)) {
          Mouse.release(MOUSE_LEFT);
        }   
        break;        
      case 4:
        if (!Mouse.isPressed(MOUSE_RIGHT)) {
          Mouse.press(MOUSE_RIGHT);
          delay(50);
          Mouse.release(MOUSE_RIGHT);
          delay(50);
          Mouse.press(MOUSE_RIGHT);
          delay(50);
          Mouse.release(MOUSE_RIGHT);                    
        }
        else if (Mouse.isPressed(MOUSE_RIGHT)) {
          Mouse.release(MOUSE_RIGHT);
        }   
        break;
      case 5:             
        if (!Mouse.isPressed(MOUSE_LEFT)) {
          Mouse.press(MOUSE_LEFT);
        } 
        else if (Mouse.isPressed(MOUSE_LEFT)) {
          Mouse.release(MOUSE_LEFT);
        }    
        break;
      case 6:
        if (!Mouse.isPressed(MOUSE_RIGHT)) {
          Mouse.press(MOUSE_RIGHT);
        } 
        else if (Mouse.isPressed(MOUSE_RIGHT)) {
          Mouse.release(MOUSE_RIGHT);
        }    
        break;                           
  };

  Mouse.move(xValue*MOUSE_MOVE_MULTI, yValue*MOUSE_MOVE_MULTI, 0);

}


//***PERFORM JOYSTICK ACTIONS FUNCTION***//

void joystickAction(int switch1,int switch2,int switch3,int switch4,int x,int y) {
    if(!switch1) {
      ledAction(1,50,4);
      Joystick.pressButton(joystickProperty[0].joystickButton);    
    }  else {
      Joystick.releaseButton(joystickProperty[0].joystickButton);
    }
    
    if(!switch2) {
      ledAction(1,50,5);
      Joystick.pressButton(joystickProperty[1].joystickButton);   
    }  else {
      Joystick.releaseButton(joystickProperty[1].joystickButton);
    }
    
    if(!switch3) {
      ledAction(1,50,7);
      Joystick.pressButton(joystickProperty[2].joystickButton); 
    }  else {
      Joystick.releaseButton(joystickProperty[2].joystickButton);
    }
    
    x = map(x, 0, 1023, -127, 127);
    y = map(y, 0, 1023, -127, 127); 
  
    if (x<=JOYSTICK_DEADZONE && x>=-JOYSTICK_DEADZONE) Joystick.setXAxis(0);  
    else Joystick.setXAxis(x);  
    
    if (y<=JOYSTICK_DEADZONE && y>=-JOYSTICK_DEADZONE) Joystick.setYAxis(0);  
    else Joystick.setYAxis(y);  

    delay(switchDelay);

}

//***CLEAR JOYSTICK ACTION FUNCTION***//

void joystickClear(){
    Joystick.setXAxis(0);
    Joystick.setYAxis(0);  
}


//***CHANGE SWITCH MODE FUNCTION***//

void changeSwitchMode(){
    //Update switch mode varia
    switchMode++;
    if (switchMode == 5) {
      switchMode=0;
    } 
    else {
    }

    //Setup hid profiles and morse code
    switchHidSetup();
    morseSetup();
    
    //Blink 2 times in modes color 
    ledAction(2,500,modeProperty[switchMode].modeColorNumber);

    //Serial print switch mode
    Serial.print("Switch Mode: ");
    Serial.println(switchMode);
    
    //Save switch mode in flash storage
    #ifdef defined(ARDUINO_SAMD_FEATHER_M0)
    switchModeFlash.write(switchMode);
    #endif
    #ifdef defined(__AVR_Atmega32U4__)
    EEPROM.put(4, switchMode);
    delay(5);
    #endif
    delay(25);
}

//***CONFIGURATION MODE ACTIONS FUNCTION***//

void configAction(int switch1,int switch2,int switch3,int switch4) {
  if(switch1==LOW) {
    decreaseSpeed();
  }
  if(switch2==LOW) {
    increaseSpeed();
  }
}

//***INCREASE SWITCH SPEED FUNCTION***//

void increaseSpeed(void) {
  switchSpeedLevel++;
  if (switchSpeedLevel == 11) {
    ledAction(6,100,3);
    switchSpeedLevel = 10;
  } else {
    ledAction(switchSpeedLevel,100,4);
    switchDelay = ((11-switchSpeedLevel)*FIXED_DELAY)+50;
    delay(25);
  }
  Serial.print("Speed level: ");
  Serial.println(switchSpeedLevel);
  #ifdef defined(ARDUINO_SAMD_FEATHER_M0)
  switchSpeedLevelFlash.write(switchSpeedLevel);
  #endif
  #ifdef defined(__AVR_Atmega32U4__)
  EEPROM.put(6, switchSpeedLevel);
  delay(5);
  #endif
  delay(25);
}

//***DECREASE SWITCH SPEED FUNCTION***//

void decreaseSpeed(void) {
  switchSpeedLevel--;
  if (switchSpeedLevel == 0) {
    ledAction(6,100,3);
    switchSpeedLevel = 1; 
  } else {
    ledAction(switchSpeedLevel,100,5);
    switchDelay = ((11-switchSpeedLevel)*FIXED_DELAY)+50;
    delay(25);
  } 
  Serial.print("Speed level: ");
  Serial.println(switchSpeedLevel);
  #ifdef defined(ARDUINO_SAMD_FEATHER_M0)
  switchSpeedLevelFlash.write(switchSpeedLevel);
  #endif
  #ifdef defined(__AVR_Atmega32U4__)
  EEPROM.put(6, switchSpeedLevel);
  delay(5);
  #endif
  delay(25);
}
