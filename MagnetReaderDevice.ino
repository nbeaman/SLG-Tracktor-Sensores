#include "Adafruit_MLX90393.h"
#include "SoftwareSerial.h"  // LOOK FOR ESP32 SERIAL IN ARDUINO COMMON LIBRARIES
#include <EEPROM.h>
#define EEPROM_SIZE 9

Adafruit_MLX90393 sensor = Adafruit_MLX90393();
#define MLX90393_CS 10

int led = 32;
int buttonPin = 15;

int buttonState = 0;
int i=0;

//int XNormal = -6;
//int YNormal = -78;
//int ZNormal = 25;

int XNormal = -30;
int YNormal = -6;
int ZNormal = 33;


SoftwareSerial XBee(16, 17);

void setup(void)
{
  pinMode(led, OUTPUT);
  pinMode(buttonPin, INPUT);
  Serial.begin(115200);
  XBee.begin(9600);

  EEPROM.begin(EEPROM_SIZE);
  
  /* Wait for serial on USB platforms. */
  while (!Serial) {
      delay(10);
  }

  Serial.println("Starting Adafruit MLX90393 Demo");

  if (! sensor.begin_I2C()) {          // hardware I2C mode, can pass in address & alt Wire
  //if (! sensor.begin_SPI(MLX90393_CS)) {  // hardware SPI mode
    Serial.println("No sensor found ... check your wiring?");
    while (1) { delay(10); }
  }
  Serial.println("Found a MLX90393 sensor");

  sensor.setGain(MLX90393_GAIN_2_5X);
  // You can check the gain too
  Serial.print("Gain set to: ");
  switch (sensor.getGain()) {
    case MLX90393_GAIN_1X: Serial.println("1 x"); break;
    case MLX90393_GAIN_1_33X: Serial.println("1.33 x"); break;
    case MLX90393_GAIN_1_67X: Serial.println("1.67 x"); break;
    case MLX90393_GAIN_2X: Serial.println("2 x"); break;
    case MLX90393_GAIN_2_5X: Serial.println("2.5 x"); break;
    case MLX90393_GAIN_3X: Serial.println("3 x"); break;
    case MLX90393_GAIN_4X: Serial.println("4 x"); break;
    case MLX90393_GAIN_5X: Serial.println("5 x"); break;
  }

  // Set resolution, per axis
  sensor.setResolution(MLX90393_X, MLX90393_RES_19);
  sensor.setResolution(MLX90393_Y, MLX90393_RES_19);
  sensor.setResolution(MLX90393_Z, MLX90393_RES_16);

  // Set oversampling
  sensor.setOversampling(MLX90393_OSR_2);

  // Set digital filtering
  sensor.setFilter(MLX90393_FILTER_6);

  float ftmp = EEPROM_ReadX();
  if (ftmp !=255 ) XNormal = ftmp;

  ftmp = EEPROM_ReadY();
  if (ftmp !=255 ) YNormal = ftmp;

  ftmp = EEPROM_ReadZ();
  if (ftmp !=255 ) ZNormal = ftmp;
  
}

void loop(void) {
  float x, y, z;

  buttonState = digitalRead(buttonPin);

  if (buttonState == HIGH) {
    EEPROM_SaveXYZ(x,y,z);
  }
  
  // get X Y and Z data at once
  if (sensor.readData(&x, &y, &z)) {
    
      Serial.print("X: "); Serial.print(x, 4); Serial.println(" uT");
      Serial.print("Y: "); Serial.print(y, 4); Serial.println(" uT");
      Serial.print("Z: "); Serial.print(z, 4); Serial.println(" uT");

      Serial.print("MX = "); Serial.println(XNormal);
      Serial.print("MY = "); Serial.println(YNormal);
      Serial.print("MZ = "); Serial.println(ZNormal);
      
      //if (z > 39){
      if ( (y < (YNormal - 6)) or (z < (ZNormal -8)) or (z > (ZNormal + 5)) ){
        Serial.println("Magnet close");
        XBee.print('H');
        digitalWrite(led, HIGH); // Turn the LED off
        delay(3000);
      } else {
        XBee.print('L');
        digitalWrite(led, HIGH); // Turn the LED on
        delay(500);
        digitalWrite(led, LOW); // Turn the LED off
        delay(2500);
      }
  } else {
      Serial.println("Unable to read XYZ data from the sensor.");
  }

  delay(500);
 
}

float EEPROM_ReadX(){
  byte PN = EEPROM.read(0);
  byte V = EEPROM.read(1);
  float f = float(V);
  if (PN == 0) f = f * -1;
  return f;
}

float EEPROM_ReadY(){
  byte PN = EEPROM.read(2);
  byte V = EEPROM.read(3);
  float f = float(V);
  if (PN == 0) f = f * -1;
  return f;
}

float EEPROM_ReadZ(){
  byte PN = EEPROM.read(4);
  byte V = EEPROM.read(5);
  float f = float(V);
  if (PN == 0) f = f * -1;
  return f;
}

byte floatToPositiveByte(float f){
  if (f <0) f = f * -1;
  int itemp = (int) round(f);
  String stemp1 = String(f);
  int indexofdot = stemp1.indexOf(".");
  String stemp2 = stemp1.substring(0,indexofdot); 
  itemp = stemp2.toInt();
  byte btemp = char(itemp);
  Serial.print(f); Serial.print("="); Serial.println(btemp);
  return btemp;
}
void EEPROM_SaveXYZ(float x,float y,float z){

    Serial.print("BUTTON PRESSED");
    for (i=0; i <= 20; i++){
      digitalWrite(led, HIGH); // Turn the LED on
      delay(50);
      digitalWrite(led, LOW); // Turn the LED off
      delay(50);
    }
    delay(2000);
    
  if (x < 0){
    EEPROM.write(0,0);
  }else {EEPROM.write(0,1);
  }
  byte bX = floatToPositiveByte(x);
  EEPROM.write(1,bX);
  if (y < 0){
    EEPROM.write(2,0);
  }else {EEPROM.write(2,1);
  }  
  byte bY = floatToPositiveByte(y);
  EEPROM.write(3,bY);
  if (z < 0){
    EEPROM.write(4,0);
  }else {EEPROM.write(4,1);
  }  
  byte bZ = floatToPositiveByte(z);
  EEPROM.write(5,bZ);
  EEPROM.commit();

  XNormal = x;
  YNormal = y;
  ZNormal = z;
}
