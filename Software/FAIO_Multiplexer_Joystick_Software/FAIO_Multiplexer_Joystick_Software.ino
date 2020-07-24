
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

//FAIO_Multiplexer Joystick Software
//FAIO_Multiplexer_Joystick_Software
//Written by: Milad Hajihassan
//Version 1.0 (12/4/2020)
//Github Link: https://github.com/milador/FAIO_Multiplexer

#include "Joystick.h"
#include <StopWatch.h>
#include <math.h>
#include <Adafruit_NeoPixel.h>

//Use FlashStorage library for M0 Boards and EEPROM for Atmega32U4 boards
#if defined(ARDUINO_SAMD_FEATHER_M0)
#include <FlashStorage.h>
#elif defined(__AVR_Atmega32U4__)
#include <EEPROM.h>
#endif


//Can be changed based on the needs of the users 
#define FIXED_SPEED_LEVEL 6
#define FIXED_DELAY 30
#define JOYSTICK_DEADZONE 5
#define JOYSTICK_ENABLED false
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

//Define Joystick pins
#define X_JOYSTICK_PIN A4
#define Y_JOYSTICK_PIN A5  


// Variable Declaration

//Declare switch state variables for each switch
int switch1State;
int switch2State;
int switch3State;
int switch4State;

//Declare joystick variables 
int xJoystick;
int yJoystick;

//Stopwatches array used to time switch presses
StopWatch switch4TimeWatcher[1];


//Declare Switch variables for settings 
int switchConfigured;
int switchReactionTime;
int switchReactionLevel;
int switchMode;


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
  String switchButtonName;
  uint8_t switchMode1ButtonNumber;
  uint8_t switchMode2ButtonNumber;
  uint8_t switchColorNumber;
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
    {1,"X1",1,5,5},
    {2,"X2",2,6,3},
    {3,"A",3,7,1},
    {4,"B",4,8,6}
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
    {1,"Mode 1",8},
    {2,"Mode 2",2},
    {4,"Settings",4}
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
  switchSetup();
  delay(5);
  joystickSetup();
  delay(1000);
  initLedFeedback();                                                          //Led will blink in a color to show the current mode 
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


  //Update status of joystick inputs
  if(JOYSTICK_ENABLED) {
  xJoystick = analogRead(X_JOYSTICK_PIN);
  yJoystick = analogRead(Y_JOYSTICK_PIN);    
  } else {
    xJoystick = 0;
    yJoystick = 0;
  }
  
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
    } else if(switchMode==1 || switchMode==2) {
      joystickAction(switchMode,switch1State,switch2State,switch3State,LOW,xJoystick,yJoystick);
      delay(5);
    }
  }
  //Serial.println(switch1State);
  //Perform joystick actions based on the mode
    switch (switchMode) {
      case 1:
        joystickAction(switchMode,switch1State,switch2State,switch3State,HIGH,xJoystick,yJoystick);    //Switch mode
        break;
      case 2:
        joystickAction(switchMode,switch1State,switch2State,switch3State,HIGH,xJoystick,yJoystick);    //Switch mode
        break;
      case 3:
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
  Serial.println("This is the FAIO Multiplexer Joystick firmware");
  Serial.println(" ");
  Serial.println("VERSION: 1.0 (12 April 2020)");
  Serial.println(" ");
  Serial.println(" --- ");
  Serial.println("Features: Joystick ");
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


//***SETUP SWITCH HID PROFILES FUNCTION (LOAD HID PROFILES)***//

void joystickSetup(){
      //Setup joystick library 
      Joystick.begin(); 

      //Set joystick x,y range
      Joystick.setXAxisRange(-127, 127);
      Joystick.setYAxisRange(-127, 127);

}

//***PERFORM JOYSTICK ACTIONS FUNCTION***//

void joystickAction(int mode, int switch1,int switch2,int switch3,int switch4,int x,int y) {
  
    if(!switch1) {
      switchFeedback(1,mode,switchReactionTime,1);
      ( mode ==1 ) ? Joystick.pressButton(switchProperty[0].switchMode1ButtonNumber-1) : Joystick.pressButton(switchProperty[0].switchMode2ButtonNumber-1);
      //Joystick.pressButton(switchProperty[0].switchMode1ButtonNumber);    
    }  else {
      ( mode ==1 ) ? Joystick.releaseButton(switchProperty[0].switchMode1ButtonNumber-1) : Joystick.releaseButton(switchProperty[0].switchMode2ButtonNumber-1);
      //Joystick.releaseButton(switchProperty[0].switchMode1ButtonNumber);
    }
    
    if(!switch2) {
      switchFeedback(2,mode,switchReactionTime,1);
      ( mode ==1 ) ? Joystick.pressButton(switchProperty[1].switchMode1ButtonNumber-1) : Joystick.pressButton(switchProperty[1].switchMode2ButtonNumber-1);
    }  else {
      ( mode ==1 ) ? Joystick.releaseButton(switchProperty[1].switchMode1ButtonNumber-1) : Joystick.releaseButton(switchProperty[1].switchMode2ButtonNumber-1);
    }
    
    if(!switch3) {
      switchFeedback(3,mode,switchReactionTime,1);
      ( mode ==1 ) ? Joystick.pressButton(switchProperty[2].switchMode1ButtonNumber-1) : Joystick.pressButton(switchProperty[2].switchMode2ButtonNumber-1);
    }  else {
      ( mode ==1 ) ? Joystick.releaseButton(switchProperty[2].switchMode1ButtonNumber-1) : Joystick.releaseButton(switchProperty[2].switchMode2ButtonNumber-1);
    }
    
    if(!switch4) {
      switchFeedback(4,mode,switchReactionTime,1);
      ( mode ==1 ) ? Joystick.pressButton(switchProperty[3].switchMode1ButtonNumber-1) : Joystick.pressButton(switchProperty[3].switchMode2ButtonNumber-1);
    }  else {
      ( mode ==1 ) ? Joystick.releaseButton(switchProperty[3].switchMode1ButtonNumber-1) : Joystick.releaseButton(switchProperty[3].switchMode2ButtonNumber-1);
    }
    
    //x = map(x, 256, 768, -127, 127);
    //y = map(y, 374, 650, -127, 127);

  
    if (x<=JOYSTICK_DEADZONE && x>=-JOYSTICK_DEADZONE) Joystick.setXAxis(0);  
    else Joystick.setXAxis(x);  
    
    if (y<=JOYSTICK_DEADZONE && y>=-JOYSTICK_DEADZONE) Joystick.setYAxis(0);  
    else Joystick.setYAxis(y);  

    delay(10);

}

//***CLEAR JOYSTICK ACTION FUNCTION***//

void joystickClear(){
    Joystick.setXAxis(0);
    Joystick.setYAxis(0);  
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
    Serial.println(switchReactionTime);
    //Calculate switch delay based on switchReactionLevel
    switchReactionTime = ((11-switchReactionLevel)*SWITCH_REACTION_TIME);
}



void initLedFeedback(){
  setLedBlink(2,500,modeProperty[switchMode-1].modeColorNumber,LED_ACTION_BRIGHTNESS);
  delay(5);
  updateLedColor(modeProperty[switchMode-1].modeColorNumber,LED_BRIGHTNESS);
  delay(5);
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
    
    //Blink 2 times in modes color 
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
    settingsFeedback(3,switchMode,100,6);
    switchReactionLevel = 10;
  } else {
    settingsFeedback(1,switchMode,100,6);
    switchReactionTime = ((11-switchReactionLevel)*SWITCH_REACTION_TIME);
    delay(25);
  }
  Serial.print("Reaction level: ");
  Serial.println(switchReactionLevel);
  Serial.print("Reaction Time(ms): ");
  Serial.println(switchReactionTime);
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

    settingsFeedback(4,switchMode,100,6);
    switchReactionLevel = 1; 
  } else {
    settingsFeedback(2,switchMode,100,6);
    switchReactionTime = ((11-switchReactionLevel)*SWITCH_REACTION_TIME);
    delay(25);
  } 
  Serial.print("Reaction level: ");
  Serial.println(switchReactionLevel);
  Serial.print("Reaction Time(ms): ");
  Serial.println(switchReactionTime);
  #if defined(ARDUINO_SAMD_FEATHER_M0)
    switchReactionLevelFlash.write(switchReactionLevel);
  #elif defined(__AVR_Atmega32U4__)
    EEPROM.put(26, switchReactionLevel);
    delay(5);
  #endif
  delay(25);
}
