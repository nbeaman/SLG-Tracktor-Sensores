/* Output-side (LED) Arduino code */
#include "SoftwareSerial.h"  // LOOK FOR ESP32 SERIAL IN ARDUINO COMMON LIBRARIES
#include <Adafruit_NeoPixel.h>

SoftwareSerial XBee(7, 6);

#define LED_PIN         5
#define VIBRATION_PIN   2
#define BUZZER_PIN      1
#define BUTTON_PIN      9
#define LED_BRIGHTNESS  10

#define LED_COUNT     7

int VibrationThreshold  = 25;
int VibrationValue      = 0;
int MotionStatus        = 0;
boolean ShowTankLevel   = true;
boolean G_BuzzerOn      = true;
String G_XBeeData = "";
char c;

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

boolean G_HoseConnected = false;

float TankWaterLevel        = 0;
float TankWaterLevel_EMPTY  = 17.46;
float TankWaterLevel_FULL   = 18.56;
float LED_Status_Previouse  = 0;

//=========================================
int DBUG = 1;
//=========================================

void setup()
{
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.setBrightness(LED_BRIGHTNESS); // Set BRIGHTNESS to about 1/5 (max = 255)
  strip.clear();
  strip.show();
  
  // Baud rate MUST match XBee settings (as set in XCTU)
  XBee.begin(9600);
  
  delay(3000);    // TEST TO SEE IF THIS HELPS UPLOADING FILES TO XIAO!!!!!!!!
  if(DBUG) Serial.begin(115200);
  
  delay(1000);

  pinMode(BUZZER_PIN, OUTPUT); // set the buzzer as output mode
  pinMode(BUTTON_PIN, INPUT_PULLUP);
    
}


void loop()

{

  if (!G_HoseConnected) ShowTankLevel = checkButton_ShowTankLevel(ShowTankLevel);

  if (XBee.available()){                // True only if there is data ready to read on XBee

    while(XBee.available()){
    //while(c != char(0x0d)){
      c = XBee.read();
      delay(50);
      G_XBeeData = G_XBeeData + String(c);     
    }

  } else {

    // Look at XBee's last trasmission
    
    if (G_XBeeData != ""){
        if(DBUG) Serial.println("Payload:" + G_XBeeData);

        // received both 'H' and 'T' values in one reading - strip off 'T' and it's value
        // occassionally we will get Payload:T14.76HL-4.61HL-4.12T14.76


        Serial.println("**" + String((G_XBeeData.indexOf('H')!=-1) and (G_XBeeData.indexOf('T')!=-1)));
        if((G_XBeeData.indexOf('H')!=-1) and (G_XBeeData.indexOf('T')!=-1)) G_XBeeData = hoseConnectValueOnly(G_XBeeData); 
                                                                                    
        String XBeeIN_TYPE = G_XBeeData.substring( 0, 1 );
        if(DBUG) Serial.println("Type:" + XBeeIN_TYPE);
        String XBeeIN_DATA = G_XBeeData.substring( 1, (G_XBeeData.length()-1) );
        if(DBUG) Serial.println("DATA:" + XBeeIN_DATA);
        
        if (XBeeIN_TYPE == "H"){
           if (XBeeIN_DATA.substring(0,1) == "H") G_HoseConnected = true;
           if (XBeeIN_DATA.substring(0,1) == "L") G_HoseConnected = false;
        } else {
           if (XBeeIN_TYPE == "T") TankWaterLevel = XBeeIN_DATA.toFloat();
        }       
    }

    G_XBeeData = "";
 
  }



   //================================================================

   if (!G_HoseConnected) ShowTankLevel = checkButton_ShowTankLevel(ShowTankLevel);
    
   if (G_HoseConnected){  
      Serial.println("Hose Connected:" + String(G_HoseConnected));

      G_BuzzerOn = checkButton_G_BuzzerOnOff(G_BuzzerOn);

      showHoseConnectedAlertLEDs(strip.Color(255,   0,   0), 50, G_BuzzerOn); // Red, 50ms, buzzer sound on or off
      
      VibrationValue = analogRead(VIBRATION_PIN);
      
      if (VibrationValue > VibrationThreshold) {
        if(DBUG) Serial.println("Motion Detected");
        if (G_BuzzerOn) BuzzerSound();
      } else {
        delay(600);
      }
                 
      clearLEDs(G_BuzzerOn, ShowTankLevel, G_HoseConnected);
   }

   //flushXBee();
   
}

String hoseConnectValueOnly(String strV){ // payload either looks like T14.76HL-4.61 or HL-4.61T14.76 , return only the 'H' data
  
  int stop_index =0;

  int H_index = strV.indexOf("H");
  String strTemp = strV.substring(H_index, strV.length());
  //Serial.println("HERE:" + strTemp);

  for(int i=1; i<strTemp.length(); i++){
    if(strTemp[i]=='T' or strTemp[i]=='H' or strTemp[i]==char(0x0D)){
      stop_index=i;
      i=strTemp.length();
    }
  }
  //Serial.println(String(stop_index));
  if(stop_index==0) stop_index = strTemp.length();

  //Serial.println(String(H_index) + "," + String(stop_index));
  strTemp = strTemp.substring(0,stop_index);

  return strTemp;
}

void flushXBee(){
    char f;
    while(XBee.available()){
      f = XBee.read();    
      Serial.print("." + String(f));
    }  
}

String getXBeePayload(){

  String strRETURN = "";
  int i = 1;
  char c;
  boolean done = false;

  while (!done){

      c = XBee.read();
      Serial.print(String(c));Serial.print("*");
      strRETURN = strRETURN + String(c);
      if(DBUG) if (c == char(0x0D) ) Serial.println("Found CR");
    
      if ( (c == char(0x0D) ) or ( i > 25 )) {
        done=true; 
        Serial.println("Found CR in getXBeepayload");
      }
      i=i+1;      
  }         

  return strRETURN;
  
}

boolean checkButton_G_BuzzerOnOff(boolean buzzer_on){
    
  boolean r;
  
  if (digitalRead(BUTTON_PIN) == LOW){    // if buzzer_on is true, then pushing the button will make it flase

    LED_AcceptButtonPressed();      // 450ms delay
    
    if (buzzer_on){
      r = false;
    }else{
      r = true;
    } 
    if(DBUG) Serial.println("G_BuzzerOn Button Pushed: " + String(r)); 
  } else {
    r = buzzer_on;
  }
  return r;  
}

boolean checkButton_ShowTankLevel(boolean show_tank_level){
  
  boolean r;

  if (digitalRead(BUTTON_PIN) == LOW){
    
    strip.clear();
    strip.show();
    
    if (show_tank_level){     // if show_tank_level was true, pushing the button will turn off lights and set not to show tank level LEDS
      r = false;
    }else{
      r = true;
    } 
      
    if(DBUG) Serial.println("ShowTankLevel Button Pushed:" + String(r)); 

    strip.setPixelColor(0, strip.Color(0,   255,   0) );
    strip.show();
    delay(300);
    LED_AcceptButtonPressed();      // 450ms delay
    
  }else{
    r = show_tank_level;
  }
  
  return r;
}

void LED_AcceptButtonPressed(){
    for (int k=75; k>0; k--){
      strip.setPixelColor(0, strip.Color(0,   255,   0) );
      strip.setPixelColor(2, strip.Color(0,   255,   0) );
      strip.setPixelColor(3, strip.Color(0,   255,   0) );      
      strip.setPixelColor(5, strip.Color(0,   255,   0) );
      strip.setPixelColor(6, strip.Color(0,   255,   0) );
      strip.setBrightness(k);
      strip.show();
      delay(6);
    }
    strip.setBrightness(LED_BRIGHTNESS);
    strip.clear();
    strip.show();  
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
void showHoseConnectedAlertLEDs(uint32_t color, int wait, boolean buzzer_on) {
  Serial.println("Showing Red Alert LEDs");
  if (buzzer_on){
      strip.setPixelColor(0, strip.Color(0,   0,   0) );
  }else{
      strip.setPixelColor(0, strip.Color(255,   255,   0) );
  }
  for(int i=1; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
  strip.clear();
  strip.show();
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

void UpdateDisplay(float l){

  float LED_ChangeStatus_Every_Value = (TankWaterLevel_FULL - TankWaterLevel_EMPTY) / 7;
  
  strip.clear();
  strip.show();

  //if (LED_Status_Previouse == 0) LED_Status_Previouse = l;                              // Device was just turned on

  //if ( (TankWaterLevel_EMPTY + l) / NumberOfLedsTolight ) > LED_Status_Previouse ) NumberOfLedsTolight = NumberOfLedsTolight + 1;
  
  //if (TankWaterLevel_EMPTY + (LED_ChangeStatus_Every_Value*2);
  
  //int NumberOfLEDsToLight =                                                 // Must calculate a number from 0 to 6;
  
  
  
}
