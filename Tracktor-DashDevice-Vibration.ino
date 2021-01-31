/* Output-side (LED) Arduino code */
#include "SoftwareSerial.h"  // LOOK FOR ESP32 SERIAL IN ARDUINO COMMON LIBRARIES
#include <Adafruit_NeoPixel.h>

SoftwareSerial XBee(7, 6);    // XIAO

#define LED_PIN       5
#define VIBRATION_PIN 2
#define BUZZER_PIN    1
#define BUTTON_PIN    9

#define LED_COUNT     7

int VibrationThreshold  = 25;
int VibrationValue      = 0;
int MotionStatus        = 0;
boolean ShowTankLevel   = true;
boolean BuzzerOn        = true;

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);


boolean HoseConnected = false;

float TankWaterLevel        = 0;
float TankWaterLevel_EMPTY  = 17.46;
float TankWaterLevel_FULL   = 18.56;
float LED_Status_Previouse  = 0;


void setup()
{
  // Baud rate MUST match XBee settings (as set in XCTU)

  delay(3000);    // TEST TO SEE IF THIS HELPS UPLOADING FILES TO XIAO!!!!!!!!
  Serial.begin(115200);
  
  delay(1000);

  pinMode(BUZZER_PIN, OUTPUT); // set the buzzer as output mode
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  XBee.begin(9600);

  strip.setBrightness(10); // Set BRIGHTNESS to about 1/5 (max = 255)
      
}

void loop()

{

  ShowTankLevel = checkButton_ShowTankLevel(ShowTankLevel);
 
  if (HoseConnected){
      colorWipe(strip.Color(255,   0,   0), 50); // Red
      strip.show();

      BuzzerOn = checkButton_BuzzerOnOff(BuzzerOn);
      
      VibrationValue = analogRead(VIBRATION_PIN);
      
      if (VibrationValue > VibrationThreshold) {
        MotionStatus = HIGH;
      } else {
        MotionStatus = LOW;
      }

      if (MotionStatus == HIGH){
         Serial.println("Motion Detected");
         if (BuzzerOn) BuzzerSound();
      } else {
         delay(600);
       } 
           
      clearLEDs(BuzzerOn, ShowTankLevel, HoseConnected);
      
  } else {
    
      if (ShowTankLevel) ShowTankWaterLevel(TankWaterLevel);
  }
  
  if (XBee.available())  
  { 
    char c = XBee.read();

    Serial.println(c);
    
    if (c == 'H') HoseConnected = true;
    if (c == 'L') HoseConnected = false;
    if (c == 'T') HoseConnected = false;
  }
}

boolean checkButton_BuzzerOnOff(boolean buzzer_on){
    
  boolean r;
  
  if (digitalRead(BUTTON_PIN) == LOW){    // if buzzer_on is true, then pushing the button will make it flase
    
    if (buzzer_on){
      strip.setPixelColor(0, strip.Color(255,   255,   0) );
      r = false;
    }else{
      strip.setPixelColor(0, strip.Color(0,   0,   0) );
      r = true;
    }
    strip.show();   
    Serial.println("BuzzerOn Button Pushed: " + String(r)); 
  } else {
    r = buzzer_on;
  }
  
  delay(1000);
  return r;  
}

boolean checkButton_ShowTankLevel(boolean show_tank_level){
  
  boolean r;
  Serial.println(digitalRead(BUTTON_PIN));
  if (digitalRead(BUTTON_PIN) == LOW){
    
    if (show_tank_level){     // if show_tank_level was true, pushing the button will turn off lights and set not to show tank level LEDS
      strip.clear();
      strip.show();
      r = false;
    }else{
      r = true;
    } 
      
    Serial.println("ShowTankLevel Button Pushed:" + String(r)); 
     
  }else{
    r = show_tank_level;
  }
  
  delay(1000); 
  return r;
}

void ShowTankWaterLevel(float wlvalue){
      
      // Code to show level based on wlvalue, if 0 show nothing

      strip.setPixelColor(0, strip.Color(0,   0,   0) );
      strip.setPixelColor(1, strip.Color(0,   0,   255) );
      strip.setPixelColor(2, strip.Color(0,   0,   255) );
      strip.setPixelColor(3, strip.Color(0,   0,   255) );
      strip.setPixelColor(4, strip.Color(0,   0,   255) ); 
      strip.show(); 
}
void colorWipe(uint32_t color, int wait) {
  for(int i=1; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

void clearLEDs(boolean buzzer_on, boolean show_tank_level, boolean hose_connected){
    strip.clear();
    if ( (buzzer_on == false) and (hose_connected)){
      strip.setPixelColor(0, strip.Color(255,   255,   0) );
    }
    strip.show(); 
}

void BuzzerSound(){
  
    for (int x = 1100; x < 2700; x=x+30) //"sing" the note one by one
    {
     tone(BUZZER_PIN, x); //output the "x" note
     delay(.01 * x); //rythem of the music,it can be tuned fast and slow by change the number"400"
     noTone(BUZZER_PIN);//stop the current note and go to the next note
    } 
}

float GetTankWaterLevel(){
  String s = "";
  s = char(XBee.read());
  s = s + char(XBee.read());
  s = s + char(XBee.read());
  s = s + char(XBee.read());
  s = s + char(XBee.read());
  s = s + char(XBee.read());
  Serial.println(TankWaterLevel);
  return float(TankWaterLevel);
}

void UpdateDisplay(float l){

  float LED_ChangeStatus_Every_Value = (TankWaterLevel_FULL - TankWaterLevel_EMPTY) / 7;
  
  strip.clear();
  strip.show();

  //if (LED_Status_Previouse == 0) LED_Status_Previouse = l;                              // Device was just turned on

  //if ( (TankWaterLevel_EMPTY + l) / NumberOfLedsTolight ) > LED_Status_Previouse ) NumberOfLedsTolight = NumberOfLedsTolight + 1;
  
  //if (TankWaterLevel_EMPTY + (LED_ChangeStatus_Every_Value*2);
  
  //int NumberOfLEDsToLight =                                                 // Must calculate a number from 0 to 6;
  
  
  
}
