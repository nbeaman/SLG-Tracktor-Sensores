/* Output-side (LED) Arduino code */
#include "SoftwareSerial.h"  // LOOK FOR ESP32 SERIAL IN ARDUINO COMMON LIBRARIES
#include <Adafruit_NeoPixel.h>

SoftwareSerial XBee(7, 6);    // XIAO

#define LED_PIN    5
#define VIBRATION_PIN 2
#define BUZZER_PIN 1

int VibrationThreshold = 25;
int VibrationValue = 0;
int MotionStatus = 0;

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 7

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);


boolean HoseConnected = false;


void setup()
{
  // Baud rate MUST match XBee settings (as set in XCTU)

  delay(3000);    // TEST TO SEE IF THIS HELPS UPLOADING FILES TO XIAO!!!!!!!!
  Serial.begin(115200);
  
  /* Wait for serial on USB platforms. */
  //while(!Serial);     
  delay(1000);

  pinMode(BUZZER_PIN, OUTPUT); // set the buzzer as output mode
  
  XBee.begin(9600);
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(10); // Set BRIGHTNESS to about 1/5 (max = 255)
      
}

void loop()

{

  if (HoseConnected){
      colorWipe(strip.Color(255,   0,   0), 50); // Red
      strip.show();
      
      VibrationValue = analogRead(VIBRATION_PIN);
      
      if (VibrationValue > VibrationThreshold) {
        MotionStatus = HIGH;
      } else {
        MotionStatus = LOW;
      }

      if (MotionStatus == HIGH){
         Serial.println("Motion Detected");
         BuzzerSound();
      } else {
         Serial.println("No motion"); 
         delay(600);
       } 
           
      strip.clear();
      strip.show();
      
  } else {
      strip.clear();
      strip.show();
  }
  
  if (XBee.available())  
  { 
    char c = XBee.read();
    Serial.println(c);
    if (c == 'H')
    {
      HoseConnected = true;
    }
    if (c=='L')
    {
      HoseConnected = false;
    }
  }
}

void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

void BuzzerSound(){
  
    for (int x = 1100; x < 2700; x=x+30) //"sing" the note one by one
    {
     tone(BUZZER_PIN, x); //output the "x" note
     delay(.01 * x); //rythem of the music,it can be tuned fast and slow by change the number"400"
     noTone(BUZZER_PIN);//stop the current note and go to the next note
    } 
}
