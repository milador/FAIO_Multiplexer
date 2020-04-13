
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

//FAIO_Multiplexer USB Switch Software
//FAIO_Multiplexer_USB_Software
//Written by: Milad Hajihassan
//Version 1.0 (12/4/20120)
//Github Link: https://github.com/milador/FAIO_Multiplexer

#include "Mouse.h"
#include "Keyboard.h"
#include <StopWatch.h>
#include "EasyMorse.h"
#include <math.h>
#include <Adafruit_NeoPixel.h>

//Use FlashStorage library for M0 Boards and EEPROM for Atmega32U4 boards
#if defined(ARDUINO_SAMD_FEATHER_M0)
#include <FlashStorage.h>
#elif defined(__AVR_Atmega32U4__)
#include <EEPROM.h>
#endif


//Can be changed based on the needs of the users 
#define MORSE_TIMEOUT 1000                                            //Maximum timeout (1000ms =1s)
#define MORSE_REACTION_TIME 10                                        //Minimum time for a dot or dash switch action ( level 10 : (1.5^1)x10 =15ms , level 1 : (1.5^10)x10=570ms )
#define MOUSE_MOVE_MULTI 2                                            //Morse mouse move multiplier 
#define SWITCH_REACTION_TIME 50                                       //Minimum time for each switch action ( level 10 : 1x50 =50ms , level 1 : 10x50=500ms )
#define SWITCH_MODE_CHANGE_TIME 2000                                  //How long to hold switch 4 to change mode 

#define LED_BRIGHTNESS 150                                             //The mode led color brightness which is always on ( Use a low value to decrease power usage )
#define LED_ACTION_BRIGHTNESS 150                                      //The action led color brightness which can be a higher value than LED_BRIGHTNESS


//Define Switch pins
#define LED_PIN 12
#define SWITCH1_PIN A0
#define SWITCH2_PIN A2
#define SWITCH3_PIN 11
#define SWITCH4_PIN 6

EasyMorse morse;

// Variable Declaration

//Declare switch state variables for each switch
int switch1State;
int switch2State;
int switch3State;
int switch4State;


//Stopwatches array used to time switch presses
StopWatch timeWatcher[3];
StopWatch switch4TimeWatcher[1];

//Initialize time variables for morse code
unsigned msMin = MS_MIN_DD;
unsigned msMax = MS_MAX_DD;
unsigned msEnd = MS_END;
unsigned msClear = MS_CL;

//Declare Switch variables for settings 
int switchConfigured;
int switchReactionTime;
int switchReactionLevel;
int switchMode;

int morseReactionTime;

//Declare Flash storage variables 
#if defined(ARDUINO_SAMD_FEATHER_M0)
FlashStorage(switchConfiguredFlash, int);
FlashStorage(switchReactionLevelFlash, int);
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


 //Switch structure 
typedef struct { 
  uint8_t switchNumber;
  String switchName;
  uint8_t switchChar;
  uint8_t switchColorNumber;
  uint8_t switchMorseTimeMulti;
} switchStruct;


 //Settings Action structure 
typedef struct { 
  uint8_t settingsActionNumber;
  String settingsActionName;
  uint8_t settingsActionColorNumber;
} settingsActionStruct;

//Color properties 
const colorStruct colorProperty[] {
    {1,"Green",{0,50,0}},
    {2,"Pink",{50,00,20}},
    {3,"Yellow",{50,50,0}},    
    {4,"Orange",{50,20,0}},
    {5,"Blue",{0,0,50}},
    {6,"Red",{50,0,0}},
    {7,"Purple",{50,0,50}},
    {8,"Teal",{0,128,128}}       
};

//Switch properties 
const switchStruct switchProperty[] {
    {1,"DOT",'a',5,1},                             //{1=dot,"DOT",'a',5=blue,1=1xMORSE_REACTION}
    {2,"DASH",'b',6,3},                            //{2=dash,"DASH",'b',6=red,3=3xMORSE_REACTION}
    {3,"",'c',1,1},                                //{3,"",'c',1=green,1=1xMORSE_REACTION}
    {4,"",'d',3,1}                                 //{4,"",'d',3=yellow,1=1xMORSE_REACTION}
};

//Settings Action properties 
const settingsActionStruct settingsProperty[] {
    {1,"Increase Reaction",5},                             //{1=Increase Reaction,5=blue}
    {2,"Decrease Reaction",6},                             //{2=Decrease Reaction,6=red}
    {3,"Max Reaction",1},                                  //{3=Max Reaction,1=green}
    {4,"Min Reaction",1}                                   //{4=Min Reaction,1=green}
};

//Mode properties 
const modeStruct modeProperty[] {
    {1,"Keyboard Switch",8},
    {2,"Morse Keyboard",7},
    {3,"Morse Mouse",2},
    {4,"Settings",4}
};


//Setup NeoPixel LED
Adafruit_NeoPixel ledPixels = Adafruit_NeoPixel(1, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {

  ledPixels.begin();                                                           //Start NeoPixel
  Serial.begin(115200);                                                        //Start Serial
  Keyboard.begin();                                                            //Starts keyboard emulation
  Mouse.begin();                                                               //Starts mouse emulation
  delay(1000);
  switchSetup();                                                               //Setup switch
  delay(5);
  initLedFeedback();                                                          //Led will blink in a color to show the current mode 
  delay(5);
  morseSetup();                                                                //Setup morse
  delay(5);
  displayFeatureList();
  
  //Initialize the switch pins as inputs
  pinMode(SWITCH1_PIN, INPUT_PULLUP);    
  pinMode(SWITCH2_PIN, INPUT_PULLUP);   
  pinMode(SWITCH3_PIN, INPUT_PULLUP);    
  pinMode(SWITCH4_PIN, INPUT_PULLUP);  

  //Initialize the LED pin as an output
  pinMode(LED_PIN, OUTPUT);                                                      


};

void loop() {
  static int ctr;                          //Control variable to set previous status of switches 
  unsigned long timePressed;               //Time that switch one or two are pressed
  unsigned long timeNotPressed;            //Time that switch one or two are not pressed
  static int previousSw4State;             //Previous status of switch four
  
  //Update status of switch inputs
  switch1State = digitalRead(SWITCH1_PIN);
  switch2State = digitalRead(SWITCH2_PIN);
  switch3State = digitalRead(SWITCH3_PIN);
  switch4State = digitalRead(SWITCH4_PIN);
  timePressed = timeNotPressed  = 0;       //reset time counters
  if (!ctr) {                              //Set previous status of switch four 
    previousSw4State = HIGH;  
    ctr++;
  }
  //Check if switch 4 is pressed to change switch mode
  if (switch4State == LOW && previousSw4State == HIGH) {
     if (switch4State == LOW) { 
      previousSw4State = LOW; 
     }
     switch4TimeWatcher[0].stop();                                //Reset and start the timer         
     switch4TimeWatcher[0].reset();                                                                        
     switch4TimeWatcher[0].start(); 
  }
  // Switch four was released
  if (switch4State == HIGH && previousSw4State == LOW) {
    previousSw4State = HIGH;
    timePressed = switch4TimeWatcher[0].elapsed();                //Calculate the time that switch one was pressed 
    switch4TimeWatcher[0].stop();                                 //Stop the single action (dot/dash) timer and reset
    switch4TimeWatcher[0].reset();
    //Perform action if the switch has been hold active for specified time
    if (timePressed >= SWITCH_MODE_CHANGE_TIME){
      changeSwitchMode();                                                                
    } else if(switchMode==1) {
      keyboardAction(switch1State,switch2State,switch3State,LOW);
    }
  }
  //Serial.println(switch1State);
  //Perform actions based on the mode
  switch (switchMode) {
      case 1:
        keyboardAction(switch1State,switch2State,switch3State,HIGH);                        //Switch mode
        break;
      case 2:
        morseAction(1,switch1State,switch2State);                                           //Keyboard Morse mode
        break;
      case 3:
        morseAction(2,switch1State,switch2State);                                           //Mouse Morse mode
        break;
      case 4:
        settingsAction(switch1State,switch2State);                                          //Settings mode
        break;
  };
  ledPixels.show(); 
  delay(5);
}

//***DISPLAY FEATURE LIST FUNCTION***//

void displayFeatureList(void) {

  Serial.println(" ");
  Serial.println(" --- ");
  Serial.println("This is the FAIO Multiplexer USB firmware");
  Serial.println(" ");
  Serial.println("VERSION: 1.0 (12 April 2020)");
  Serial.println(" ");
  Serial.println(" --- ");
  Serial.println("Features: Adaptive switch, Morse Keyboard, Morse Mouse");
  Serial.println(" --- ");
  Serial.println(" ");

}

//***RGB LED FUNCTION***//

void setLedBlink(int numBlinks, int delayBlinks, int ledColor,uint8_t ledBrightness) {
  if (numBlinks < 0) numBlinks *= -1;

      for (int i = 0; i < numBlinks; i++) {
        ledPixels.setPixelColor(0, ledPixels.Color(colorProperty[ledColor-1].colorCode.g,colorProperty[ledColor-1].colorCode.r,colorProperty[ledColor-1].colorCode.b));
        ledPixels.setBrightness(ledBrightness);
        ledPixels.show(); 
        delay(delayBlinks);
        ledPixels.setPixelColor(0, ledPixels.Color(0,0,0));
        ledPixels.setBrightness(ledBrightness);
        ledPixels.show(); 
        delay(delayBlinks);
      }
}

//***UPDATE RGB LED COLOR FUNCTION***//

void updateLedColor(int ledColor, uint8_t ledBrightness) {
    ledPixels.setPixelColor(0, ledPixels.Color(colorProperty[ledColor-1].colorCode.g,colorProperty[ledColor-1].colorCode.r,colorProperty[ledColor-1].colorCode.b));
    ledPixels.setBrightness(ledBrightness);
    ledPixels.show();
}

//***GET RGB LED COLOR FUNCTION***//

uint32_t getLedColor(int ledModeNumber) {

  int colorNumber= modeProperty[ledModeNumber-1].modeColorNumber-1;
  
  return (ledPixels.Color(colorProperty[colorNumber].colorCode.g,colorProperty[colorNumber].colorCode.r,colorProperty[colorNumber].colorCode.b));
}

//***GET RGB LED BRIGHTNESS FUNCTION***//

uint8_t getLedBrightness() {
  return (ledPixels.getBrightness());
}

//***SET RGB LED COLOR FUNCTION***//

void setLedColor (uint32_t ledColor, uint8_t ledBrightness){
  ledPixels.setPixelColor(0, ledColor);
  ledPixels.setBrightness(ledBrightness);
  ledPixels.show(); 

}

//***SET RGB LED BRIGHTNESS FUNCTION***//

void setLedBrightness(uint8_t ledBrightness) {
  ledPixels.setBrightness(ledBrightness);
  ledPixels.show();
}

//***CLEAR RGB LED FUNCTION***//

void ledClear() {
  ledPixels.setPixelColor(0, ledPixels.Color(0,0,0));
  ledPixels.show(); 
}

void switchFeedback(int switchNumber,int modeNumber,int delayTime, int blinkNumber =1)
{
  //Get previous led color and brightness 
  uint32_t previousColor = getLedColor(modeNumber);
  uint8_t previousBrightness = getLedBrightness();
 
  //updateLedColor(switchProperty[switchNumber-1].switchColorNumber,LED_ACTION_BRIGHTNESS);
  //delay(MORSE_REACTION);
  setLedBlink(blinkNumber,delayTime,switchProperty[switchNumber-1].switchColorNumber,LED_ACTION_BRIGHTNESS);
  delay(5);

  //Set previous led color and brightness 
  setLedColor(previousColor,previousBrightness);
  
}

void settingsFeedback(int settingsNumber,int modeNumber,int delayTime, int blinkNumber =1)
{
  //Get previous led color and brightness 
  uint32_t previousColor = getLedColor(modeNumber);
  uint8_t previousBrightness = getLedBrightness();
 
  setLedBlink(blinkNumber,delayTime,settingsProperty[settingsNumber-1].settingsActionColorNumber,LED_ACTION_BRIGHTNESS);
  delay(5);

  //Set previous led color and brightness 
  setLedColor(previousColor,previousBrightness);
  
}

void modeFeedback(int modeNumber,int delayTime, int blinkNumber =1)
{

   //Get new led color and brightness 
  uint32_t newColor = getLedColor(modeNumber);
  uint8_t newBrightness = getLedBrightness();
  
  setLedBlink(blinkNumber,delayTime,modeProperty[modeNumber-1].modeColorNumber,LED_ACTION_BRIGHTNESS);
  delay(5);

  //Set new led color and brightness 
  setLedColor(newColor,newBrightness);
  
}

//***SETUP SWITCH MODE FUNCTION***//

void switchSetup() {
  //Check if it's first time running the code
  #if defined(ARDUINO_SAMD_FEATHER_M0)
    switchConfigured = switchConfiguredFlash.read();
  #elif defined(__AVR_Atmega32U4__)
    EEPROM.get(22, switchConfigured);
    delay(5);
    if(switchConfigured<0){ switchConfigured = 0; } 
  #endif
  if (switchConfigured==0) {
    //Define default settings if it's first time running the code
    switchReactionLevel=10;
    switchMode=1;
    switchConfigured=1;

    //Write default settings to flash storage 
    #if defined(ARDUINO_SAMD_FEATHER_M0)
      switchReactionLevelFlash.write(switchReactionLevel);
      switchModeFlash.write(switchMode);
      switchConfiguredFlash.write(switchConfigured);
    #elif defined(__AVR_Atmega32U4__)
      EEPROM.put(26, switchReactionLevel);
      delay(5);
      EEPROM.put(24, switchMode);
      delay(5);
      EEPROM.put(22, switchConfigured);
      delay(5);
    #endif
  } else {
    //Load settings from flash storage if it's not the first time running the code
    #if defined(ARDUINO_SAMD_FEATHER_M0)
      switchReactionLevel=switchReactionLevelFlash.read();
      switchMode=switchModeFlash.read();
    #elif defined(__AVR_Atmega32U4__)
      EEPROM.get(26, switchReactionLevel);
      delay(5);
      EEPROM.get(24, switchMode);
      delay(5);
    #endif    
  }  

    //Serial print settings 
    Serial.print("Switch Mode: ");
    Serial.println(switchMode);

    Serial.print("Switch Reaction Level: ");
    Serial.println(switchReactionLevel);
    Serial.print("Reaction Time(ms): ");
    Serial.print(switchReactionTime);
    Serial.print("-");
    Serial.println(morseReactionTime);   
    //Calculate switch delay based on switchReactionLevel
    switchReactionTime = ((11-switchReactionLevel)*SWITCH_REACTION_TIME);
    morseReactionTime = (pow(1.5,(11-switchReactionLevel))*MORSE_REACTION_TIME);
}

//***SETUP MORSE FUNCTION***//

void morseSetup() {
    morse.clear();
    msMin = morseReactionTime;
    msMax = msEnd = msClear = MORSE_TIMEOUT; 

}

void initLedFeedback(){
  setLedBlink(2,500,modeProperty[switchMode-1].modeColorNumber,LED_ACTION_BRIGHTNESS);
  delay(5);
  updateLedColor(modeProperty[switchMode-1].modeColorNumber,LED_BRIGHTNESS);
  delay(5);
}

//***ADAPTIVE SWITCH KEYBOARD FUNCTION***//

void keyboardAction(int switch1,int switch2,int switch3,int switch4) {
    if(!switch1) {
      switchFeedback(1,switchMode,switchReactionTime,1);
      //Serial.println("a");
      Keyboard.press(switchProperty[0].switchChar);
    } else if(!switch2) {
      switchFeedback(2,switchMode,switchReactionTime,1);
      //Serial.println("b");
      Keyboard.press(switchProperty[1].switchChar);
    } else if(!switch3) {
      switchFeedback(3,switchMode,switchReactionTime,1);
      //Serial.println("c");
      Keyboard.press(switchProperty[2].switchChar);
    } else if(!switch4) {
      switchFeedback(4,switchMode,switchReactionTime,1);
      //Serial.println("d");
      Keyboard.press(switchProperty[3].switchChar);
    }
    else
    {
      Keyboard.releaseAll();
    }
    delay(5);

}

//***MORSE CODE TO MOUSE CONVERT FUNCTION***//

void morseAction(int mode,int switch1,int switch2) {
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
         switchFeedback(1,mode,morseReactionTime,1);                                            //Blink blue once
         previousSwitch1 = LOW;
     } //Dash
     if (switch2 == LOW) {
         switchFeedback(2,mode,morseReactionTime,1);                                            //Blink red once
         previousSwitch2 = LOW;
     }
     isShown = 0;                                                                               //A key is pressed but not Shown yet
     for (i=0; i<3; i++) timeWatcher[i].stop();                                                          //Stop end of char
     
     timeWatcher[0].reset();                                                                             //Reset and start the the timer
     timeWatcher[0].start();                                                                             
   }

 
 // Switch 1 or Dot was released
 if (switch1 == HIGH && previousSwitch1 == LOW) {
   previousSwitch1 = HIGH;
   
   backspaceDone = 0;                                                                           //Backspace is not entered 
   timePressed = timeWatcher[0].elapsed();                                                               //Calculate the time that key was pressed 
   for (i=0; i<3; i++) timeWatcher[i].stop();                                                            //Stop end of character and reset
   for (i=0; i<3; i++) timeWatcher[i].reset();                                                           
   timeWatcher[1].start();

  //Push dot to morse stack if it's pressed for specified time
    if (timePressed >= msMin && timePressed < msMax) {
      morse.push(1);
      isShown = 0; 
    }
 }

   //Switch 2 or Dash was released
   if (switch2 == HIGH && previousSwitch2 == LOW) {

    previousSwitch2 = HIGH;
    backspaceDone = 0;                                                                          //Backspace is not entered 
    timePressed = timeWatcher[0].elapsed();                                                              //Calculate the time that key was pressed 
    for (i=0; i<3; i++) timeWatcher[i].stop();                                                           //Stop end of character and reset
    for (i=0; i<3; i++) timeWatcher[i].reset();
    timeWatcher[1].start();
   
    //Push dash to morse stack if it's pressed for specified time
    if (timePressed >= msMin && timePressed < msMax) {
      morse.push(2);
      isShown = 0;       
    }
   }

 // Processing the backspace key if button 1 or button 2 are hold and pressed for defined time
  if (timePressed >= msMax && timePressed >= msClear && backspaceDone == 0 &&  mode== 1) {
    previousSwitch1 = HIGH;
    previousSwitch2 = HIGH;
    if(mode==1) Keyboard.write(8);                                                               //Press Backspace key
    backspaceDone = 1;                                                                          //Set Backspace done already
    isShown = 1;
    for (i=1; i<3; i++) { timeWatcher[i].stop(); timeWatcher[i].reset(); }                      //Stop and reset end of character
 }

   //End the character if nothing pressed for defined time and nothing is shown already 
   timeNotPressed = timeWatcher[1].elapsed();
    if (timeNotPressed >= msMax && timeNotPressed >= msEnd && isShown == 0 && backspaceDone == 0) {
      //Serial.println(morse.getCharNum()); 
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
      timeWatcher[1].stop();                                                                               //Stop and reset the timer to form a character
      timeWatcher[1].reset();
  }
  
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


//***CHANGE SWITCH MODE FUNCTION***//

void changeSwitchMode(){
    //Update switch mode varia
    switchMode++;
    if (switchMode == (sizeof (modeProperty) / sizeof (modeProperty[0]))+1) {
      switchMode=1;
    } 
    else {
    }

    morseSetup();
    
    //Blink 2 times in modes color 
    //setLedBlink(2,500,modeProperty[switchMode].modeColorNumber,LED_ACTION_BRIGHTNESS);
    modeFeedback(switchMode,500,2);

    //Serial print switch mode
    Serial.print("Switch Mode: ");
    Serial.println(switchMode);
    
    //Save switch mode in flash storage
    #if defined(ARDUINO_SAMD_FEATHER_M0)
      switchModeFlash.write(switchMode);
    #elif defined(__AVR_Atmega32U4__)
      EEPROM.put(24, switchMode);
      delay(5);
    #endif
    delay(25);
}

//***CONFIGURATION MODE ACTIONS FUNCTION***//

void settingsAction(int switch1,int switch2) {
  if(switch1==LOW) {
    decreaseReactionLevel();
  }
  if(switch2==LOW) {
    increaseReactionLevel();
  }
}

//***INCREASE SWITCH REACTION LEVEL FUNCTION***//

void increaseReactionLevel(void) {
  switchReactionLevel++;
  if (switchReactionLevel == 11) {
    //setLedBlink(6,100,3,LED_ACTION_BRIGHTNESS);
    settingsFeedback(3,switchMode,100,6);
    switchReactionLevel = 10;
  } else {
    //setLedBlink(switchReactionLevel,100,4,LED_ACTION_BRIGHTNESS);
    settingsFeedback(1,switchMode,100,6);
    switchReactionTime = ((11-switchReactionLevel)*SWITCH_REACTION_TIME);
    morseReactionTime = (pow(1.5,(11-switchReactionLevel))*MORSE_REACTION_TIME);
    delay(25);
  }
  Serial.print("Reaction level: ");
  Serial.println(switchReactionLevel);
  Serial.print("Reaction Time(ms): ");
  Serial.print(switchReactionTime);
  Serial.print("-");
  Serial.println(morseReactionTime);
  #if defined(ARDUINO_SAMD_FEATHER_M0)
    switchReactionLevelFlash.write(switchReactionLevel);
  #elif defined(__AVR_Atmega32U4__)
    EEPROM.put(26, switchReactionLevel);
    delay(5);
  #endif
  delay(25);
}

//***DECREASE SWITCH REACTION LEVEL FUNCTION***//

void decreaseReactionLevel(void) {
  switchReactionLevel--;
  if (switchReactionLevel == 0) {
    //setLedBlink(6,100,3,LED_ACTION_BRIGHTNESS);
    settingsFeedback(4,switchMode,100,6);
    switchReactionLevel = 1; 
  } else {
    //setLedBlink(switchReactionLevel,100,5,LED_ACTION_BRIGHTNESS);
    settingsFeedback(2,switchMode,100,6);
    switchReactionTime = ((11-switchReactionLevel)*SWITCH_REACTION_TIME);
    morseReactionTime = (pow(1.5,(11-switchReactionLevel))*MORSE_REACTION_TIME);
    delay(25);
  } 
  Serial.print("Reaction level: ");
  Serial.println(switchReactionLevel);
  Serial.print("Reaction Time(ms): ");
  Serial.print(switchReactionTime);
  Serial.print("-");
  Serial.println(morseReactionTime);  
  #if defined(ARDUINO_SAMD_FEATHER_M0)
    switchReactionLevelFlash.write(switchReactionLevel);
  #elif defined(__AVR_Atmega32U4__)
    EEPROM.put(26, switchReactionLevel);
    delay(5);
  #endif
  delay(25);
}
