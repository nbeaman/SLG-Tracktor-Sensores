#include <Wire.h>
#include "SoftwareSerial.h"  // LOOK FOR ESP32 SERIAL IN ARDUINO COMMON LIBRARIES
#include "Adafruit_MPRLS.h"

// You dont *need* a reset and EOC pin for most uses, so we set to -1 and don't connect
#define RESET_PIN  -1  // set to any GPIO pin # to hard-reset on begin()
#define EOC_PIN    -1  // set to any GPIO pin to read end-of-conversion by pin
Adafruit_MPRLS mpr = Adafruit_MPRLS(RESET_PIN, EOC_PIN);

SoftwareSerial XBee(7, 6);        // FOR XIAO

String  MESSAGE               = "";
float   vPSI                   = 0;

void setup() {
  delay(3000);    // TEST TO SEE IF THIS HELPS UPLOADING FILES TO XIAO!!!!!!!!
  Serial.begin(115200);
  delay(1000);
  XBee.begin(9600);
  
  Serial.println("MPRLS Simple Test");
  if (! mpr.begin()) {
    Serial.println("Failed to communicate with MPRLS sensor, check wiring?");
    while (1) {
      delay(10);
    }
  }
  Serial.println("Found MPRLS sensor");

  
}


void loop() {
  float pressure_hPa = mpr.readPressure();
  Serial.print("Pressure (hPa): "); Serial.println(pressure_hPa);
  Serial.print("Pressure (PSI): "); Serial.println(pressure_hPa / 68.947572932);

  vPSI = pressure_hPa / 68.947572932;
  
  MESSAGE = "T";

  MESSAGE = MESSAGE + String(vPSI);
  
  Serial.println(MESSAGE);
  for (int i=0; i < MESSAGE.length(); i++){
    XBee.write(MESSAGE[i]);
  }
  
  XBee.write(char(0x0d));
    
  delay(4000);
}
