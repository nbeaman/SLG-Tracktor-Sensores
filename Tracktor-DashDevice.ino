/* Output-side (LED) Arduino code */
#include "SoftwareSerial.h"  // LOOK FOR ESP32 SERIAL IN ARDUINO COMMON LIBRARIES
#include <Adafruit_NeoPixel.h>

// RX: Arduino pin 16, XBee pin DOUT.  TX:  Arduino pin 17, XBee pin DIN
SoftwareSerial XBee(16, 17);

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN    32
#define BUZZER_PIN 33
#define MOTION_PIN 36
int MotionStatus = 0;

//const int button1Pin = 15;
//const int button2Pin = 39;

//int button1State = 0;
//int button2State = 0;

boolean BuzzerActive = true;

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 12

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// BUZZER----------------
int freq = 2000;
int channel = 0;
int resolution = 8;
//-----------------------

boolean HoseConnected = false;


void setup()
{
  // Baud rate MUST match XBee settings (as set in XCTU)
  XBee.begin(9600);
  Serial.begin(115200);
  Serial.println("HERE");

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)

  pinMode(BUZZER_PIN, OUTPUT);    // sets the digital pin BUZZER_PIN as output
  digitalWrite(BUZZER_PIN, LOW);  // sets the digital pin BUZZER_PIN off

  pinMode(MOTION_PIN, INPUT);
  //digitalWrite(MOTION_PIN, LOW);
  
  ledcSetup(channel, freq, resolution);
  ledcAttachPin(BUZZER_PIN, channel);  

  //pinMode(button1Pin, INPUT);
  //pinMode(button2Pin, INPUT);
      
}

void loop()

{

  /*
  button1State = digitalRead(button1Pin);
  button2State = digitalRead(button2Pin);

  if ( button1State == HIGH){
    Serial.println("Button 1 pressed (15)");
    BuzzerActive = false;
  }
   if ( button2State == HIGH){
    Serial.println("Button 2 pressed (39)");
    BuzzerActive = true;
  }
  */
  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
  if (HoseConnected){
      colorWipe(strip.Color(255,   0,   0), 50); // Red
      digitalWrite(MOTION_PIN, HIGH);  // turn LED ON
      strip.show();
      
      MotionStatus = digitalRead(MOTION_PIN);
      
      if (MotionStatus == HIGH) Serial.println("Motion Detected");

      if (MotionStatus == HIGH){
         Serial.println("Motion Detected");
         if ((BuzzerActive)) SoundBuzzer();
      } else {
         Serial.println("No motion"); 
         digitalWrite(MOTION_PIN, LOW);
         delay(600);
       } 
      
      digitalWrite(MOTION_PIN, LOW); // turn LED OFF if we have no motion      
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

void SoundBuzzer(){

  digitalWrite(BUZZER_PIN, HIGH);  // sets the digital pin BUZZER_PIN on
  
  ledcWriteTone(channel, 2000);
  
  for (int dutyCycle = 0; dutyCycle <= 255; dutyCycle=dutyCycle+10){
    ledcWrite(channel, dutyCycle);
    delay(19);
  }
  
  ledcWrite(channel, 125);
  
  for (int freq = 255; freq < 10000; freq = freq + 250){
     ledcWriteTone(channel, freq);
     delay(5);
  }

  digitalWrite(BUZZER_PIN, LOW);  // sets the digital pin BUZZER_PIN off
}
