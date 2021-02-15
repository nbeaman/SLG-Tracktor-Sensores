#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "SoftwareSerial.h"  // LOOK FOR ESP32 SERIAL IN ARDUINO COMMON LIBRARIES

SoftwareSerial XBee(16, 17);    // HUZZAN32

//=========================================
int DBUG = 1;
//=========================================

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

String G_XBeeData           = "";
String G_MagnetReading      = "";
String G_MicPressureReading = "";
String G_DISPLAY            = "";
char c;

void setup() {
  delay(3000);
  Serial.begin(115200);
  delay(3000);
  
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

  if (XBee.available()){                // True only if there is data ready to read on XBee
    
    while(XBee.available()){
      c = XBee.read();
      Serial.print(c);Serial.print("-");
      delay(10);
      G_XBeeData = G_XBeeData + String(c);     
    }

  } else {

    // Look at XBee's last trasmission
    
    if (G_XBeeData != ""){

        if(DBUG) Serial.println("Payload:" + G_XBeeData);

        if((G_XBeeData.indexOf('H')!=-1) and (G_XBeeData.indexOf('T')!=-1)) G_XBeeData = hoseConnectValueOnly(G_XBeeData);
        
        String XBeeIN_TYPE = G_XBeeData.substring( 0, 1 );
        if(DBUG) Serial.println("Type:" + XBeeIN_TYPE);
        String XBeeIN_DATA = G_XBeeData.substring( 1, (G_XBeeData.length()-1) );
        if(DBUG) Serial.println("DATA:" + XBeeIN_DATA);

        if (XBeeIN_TYPE == "H"){
            G_MagnetReading       = G_XBeeData;
        } else {
            G_MicPressureReading  = G_XBeeData;
        } 


        G_DISPLAY = G_MagnetReading;
        
        for (int i=1; i< (12 - G_MagnetReading.length()); i++){
          G_DISPLAY = G_DISPLAY + " ";
        }
        
        G_DISPLAY = G_DISPLAY + G_MicPressureReading;
        Serial.println("G_DISPLAY: " + G_DISPLAY);
        
        display.clearDisplay();
        display.setCursor(0,0);
        Serial.println(G_DISPLAY);
        display.print(G_DISPLAY);  
        display.display(); 
        yield();
                  
    }
            
    G_DISPLAY = "";
    G_XBeeData = "";

    //delay(300);
  }

}


String hoseConnectValueOnly(String strV){ // payload either looks like T14.76HL-4.61 or HL-4.61T14.76 , return only the 'H' data
  
  int stop_index =0;

  int H_index = strV.indexOf("H");
  String strTemp = strV.substring(H_index, strV.length());
  //Serial.println("HERE:" + strTemp);

  for(int i=1; i<strTemp.length(); i++){
    if(strTemp[i]=='T' or strTemp[i]=='H' or strTemp[i]==char(0x0D)){
      stop_index=i;               // end index of H value data
      i=strTemp.length();         // stop the for loop
    }
  }
  //Serial.println(String(stop_index));
  if(stop_index==0) stop_index = strTemp.length();

  //Serial.println(String(H_index) + "," + String(stop_index));
  strTemp = strTemp.substring(0,stop_index);

  return strTemp;
}
