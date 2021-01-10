#include <Tlv493d.h>
#include "SoftwareSerial.h"  // LOOK FOR ESP32 SERIAL IN ARDUINO COMMON LIBRARIES

float x = 0;
float y = 0;
float z = 0;

float xt =  0;
float yt =  0;
float zt =  0;

float TAPS = 0;
float TAPSNORMALNEGATIVE = -2.5;
float TAPSNORMALPOSITIVE = 2.5;
char STATUS = 'L';
String MESSAGE;

int samplenumber = 10;

// Tlv493d Opject
Tlv493d Tlv493dMagnetic3DSensor = Tlv493d();

SoftwareSerial XBee(7, 6);

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Tlv493dMagnetic3DSensor.begin();
  XBee.begin(9600);

  /* Wait for serial on USB platforms. */
  while (!Serial) {
      delay(10);
      
      }
}

void loop() {


  for (int i=1; i <=samplenumber; i++){

    Tlv493dMagnetic3DSensor.updateData();
    delay(100);
      
   xt = xt + Tlv493dMagnetic3DSensor.getX();
   yt = yt + Tlv493dMagnetic3DSensor.getY();
   zt = zt + Tlv493dMagnetic3DSensor.getZ();
   delay(100);
    
  }

  TAPS = xt + yt + zt;
  
  /* 
  Serial.print("XA = ");
  Serial.print(xt/samplenumber);
  Serial.print(" mT; YA = ");
  Serial.print(yt/samplenumber);
  Serial.print(" mT; ZA = ");
  Serial.print(zt/samplenumber);
  Serial.println(" mT");
  */

  if ((TAPS > TAPSNORMALPOSITIVE) or (TAPS < TAPSNORMALNEGATIVE)){
    STATUS = 'H';
  } else {
    STATUS = 'L';    
  }

  MESSAGE = "*" + String(STATUS) + TAPS + "*";
  
  Serial.println(MESSAGE);
  XBee.print(MESSAGE);

  TAPS =0;
     
  xt=0;
  yt=0;
  zt=0;
}
