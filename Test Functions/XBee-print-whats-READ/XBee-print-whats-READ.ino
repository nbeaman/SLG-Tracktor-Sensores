#include "SoftwareSerial.h"  // LOOK FOR ESP32 SERIAL IN ARDUINO COMMON LIBRARIES

SoftwareSerial XBee(16, 17);


void setup()
{
  Serial.begin(115200);
  
  XBee.begin(9600);

}

void loop()
{
    if (XBee.available()){
      char c = XBee.read();
      Serial.print(String(c));
      delay(10);
    }

}
