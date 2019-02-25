
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

//FAIO 2 XAC Software
//FAIO_XAC_Software
//Written by: Milad Hajihassan
//Version 1.0 (3/2/2019)
//Github Link: https://github.com/milador/FAIO-2


#include "Joystick.h"
#include <math.h>
#include <Adafruit_NeoPixel.h>


//Can be changed based on the needs of the users 
#define FIXED_SPEED_LEVEL 6
#define FIXED_DELAY 30
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


// Variable Declaration

//Declare switch state variables for each switch
int switch1State;
int switch2State;
int switch3State;
int switch4State;

//Declare joystick variables 
int xJoystick;
int yJoystick;

int switchDelay;
int switchSpeedLevel;


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
  String switchName;
  uint8_t joystickButtonNumber;
  String xacButtonName;
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


//Joystick properties
const joystickStruct joystickProperty[] {
    {0,"a",4,"A"},
    {1,"b",5,"B"},
    {2,"c",0,"X1"},
    {3,"d",1,"X2"}    
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
  joystickSetup();                                                             //Setup joystick

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

  //Update status of joystick inputs
  xJoystick = analogRead(X_JOYSTICK_PIN);
  yJoystick = analogRead(Y_JOYSTICK_PIN);

  //Perform joystick actions based on the mode
  joystickAction(switch1State,switch2State,switch3State,switch4State,xJoystick,yJoystick);

  ledPixels.show();                                                                        //Update pixel color through the hardware.

}

//***DISPLAY FEATURE LIST FUNCTION***//

void displayFeatureList(void) {

  Serial.println(" ");
  Serial.println(" --- ");
  Serial.println("This is the FAIO 2 XAC firmware");
  Serial.println(" ");
  Serial.println("VERSION: 1.0 (3 Jan 2019)");
  Serial.println(" ");
  Serial.println(" --- ");
  Serial.println("Features: Joystick");
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

void joystickSetup(){
      //Setup joystick library 
      Joystick.begin(); 

      //Calculate switch delay
      switchDelay = ((11-FIXED_SPEED_LEVEL)*FIXED_DELAY)+50;

      //Set joystick x,y range
      Joystick.setXAxisRange(-127, 127);
      Joystick.setYAxisRange(-127, 127);

       //Blink 2 times to indicate device is ready to use
      ledAction(2,500,3);
}

//***PERFORM JOYSTICK ACTIONS FUNCTION***//

void joystickAction(int switch1,int switch2,int switch3,int switch4,int x,int y) {
    if(!switch1) {
      ledAction(1,50,0);   //Green led blink
      Joystick.pressButton(joystickProperty[0].joystickButtonNumber);    
    }  else {
      Joystick.releaseButton(joystickProperty[0].joystickButtonNumber);
    }
    
    if(!switch2) {
      ledAction(1,50,5);   //Red led blink
      Joystick.pressButton(joystickProperty[1].joystickButtonNumber);   
    }  else {
      Joystick.releaseButton(joystickProperty[1].joystickButtonNumber);
    }
    
    if(!switch3) {
      ledAction(1,50,4);   //Blue led blink
      Joystick.pressButton(joystickProperty[2].joystickButtonNumber); 
    }  else {
      Joystick.releaseButton(joystickProperty[2].joystickButtonNumber);
    }

    if(!switch4) {
      ledAction(1,50,2);   //Yellow led blink
      Joystick.pressButton(joystickProperty[3].joystickButtonNumber); 
    }  else {
      Joystick.releaseButton(joystickProperty[3].joystickButtonNumber);
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
