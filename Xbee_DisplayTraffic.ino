#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "SoftwareSerial.h"  // LOOK FOR ESP32 SERIAL IN ARDUINO COMMON LIBRARIES

SoftwareSerial XBee(16, 17);    // HUZZAN32

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

// OLED FeatherWing buttons map to different pins depending on board:
#if defined(ESP8266)
  #define BUTTON_A  0
  #define BUTTON_B 16
  #define BUTTON_C  2
#elif defined(ESP32)
  #define BUTTON_A 15
  #define BUTTON_B 32
  #define BUTTON_C 14
#elif defined(ARDUINO_STM32_FEATHER)
  #define BUTTON_A PA15
  #define BUTTON_B PC7
  #define BUTTON_C PC5
#elif defined(TEENSYDUINO)
  #define BUTTON_A  4
  #define BUTTON_B  3
  #define BUTTON_C  8
#elif defined(ARDUINO_FEATHER52832)
  #define BUTTON_A 31
  #define BUTTON_B 30
  #define BUTTON_C 27
#else // 32u4, M0, M4, nrf52840 and 328p
  #define BUTTON_A  9
  #define BUTTON_B  6
  #define BUTTON_C  5
#endif

void setup() {
  Serial.begin(115200);

  XBee.begin(9600);
  
  Serial.println("OLED FeatherWing test");
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32

  Serial.println("OLED begun");

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(1000);

  // Clear the buffer.
  display.clearDisplay();
  display.display();

  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);

display.setTextSize(2);
      display.setTextColor(SSD1306_WHITE);
}

void loop() {
  /*
  if(!digitalRead(BUTTON_A)) display.print("A");
  if(!digitalRead(BUTTON_B)) display.print("B");
  if(!digitalRead(BUTTON_C)) display.print("C");
  delay(10);
  yield();
  display.display();
  */

    
    
   if (XBee.available()){
    
      
      
      String XBeeIN      = getXBeePayload();
      XBeeIN = ReturnHoseMagnetValue(XBeeIN);
      
      
      display.clearDisplay();
      display.setCursor(0,0);
      Serial.println(XBeeIN);
      display.print(XBeeIN);
      
      display.display();

      yield();
      //delay(300);
      //flushXBee();
   }

   

}

void flushXBee(){
  
  boolean done = false;
  char c;
  
  while (!done){
      if (XBee.available()){

        c = XBee.read();
        delay(30);
        Serial.print(String(c));
   
      }else{
        done=true;      
      }   
  }    
  
}

String ReturnHoseMagnetValue(String s){

  boolean HLorHHfound = false;
  String sreturn = "";
  
  for (int i = 0; i < s.length(); i++){
    if(s[i] == 'H' and (s[i+1]=='L' or s[i+1]=='H')) HLorHHfound = true;
    if (HLorHHfound) sreturn = sreturn + s[i];
  }
  return sreturn;
}


String getXBeePayload(){

  String strRETURN = "";
  int i = 1;
  char c;
  boolean done = false;

  while (!done){
      if (XBee.available()){
        c = XBee.read();
        delay(30);
        strRETURN = strRETURN + String(c);
   
      }      
      if ( (c == char(0x0D) ) or ( i > 25 )) done=true; 
      i=i+1;      
  }         

  return strRETURN;
  
}
