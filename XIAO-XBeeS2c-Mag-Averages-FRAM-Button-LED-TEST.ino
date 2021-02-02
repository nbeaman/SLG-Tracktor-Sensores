#include <Tlv493d.h>
#include "SoftwareSerial.h"  // LOOK FOR ESP32 SERIAL IN ARDUINO COMMON LIBRARIES
#include "Adafruit_EEPROM_I2C.h"

Adafruit_EEPROM_I2C i2ceeprom;

String DBUG = "FRAM";

//---[ FRAM ]-------------------------------------
// FRAM Variables
#define EEPROM_ADDR         0x50  // the default address of the I2C device
int TAPS_EEPROM_LOCATION =  0;
//------------------------------------------------


const int ledPin        = 0;
const int buttonPin     = 1;
const int XBeeSleepPin  = 2;
const int samplenumber  = 10;

float   xt =  0;
float   yt =  0;
float   zt =  0;

float   TAPS = 0;
float   TAPSNORMALNEGATIVE = -6;
float   TAPSNORMALPOSITIVE = 6;
char    STATUS = 'L';
String  MESSAGE;


// Tlv493d Opject
Tlv493d Tlv493dMagnetic3DSensor = Tlv493d();

SoftwareSerial XBee(7, 6);        // FOR XIAO
//SoftwareSerial XBee(16, 17);    // FOR HUZZHA32

void setup() {
  delay(3000);    // TEST TO SEE IF THIS HELPS UPLOADING FILES TO XIAO!!!!!!!!
  Serial.begin(115200);
  
  /* Wait for serial on USB platforms. */
  //while(!Serial);     
  delay(1000); 

    //---[ FRAM ]-------------------------------------
  if (i2ceeprom.begin(EEPROM_ADDR)) {     // FRAM i2c addr begin
    if (DBUG == "FRAM") Serial.println("Found I2C EEPROM");
  } else {
    if (DBUG == "FRAM") Serial.println("I2C EEPROM not identified ... check your connections?\r\n");
    while (1) delay(10);
  }
  //------------------------------------------------
  
  Tlv493dMagnetic3DSensor.begin();
  XBee.begin(9600);

  pinMode(ledPin, OUTPUT);
  pinMode(XBeeSleepPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  
}

void loop() {

  digitalWrite(XBeeSleepPin, HIGH);  // XBee Sleep
  
  digitalWrite(ledPin, HIGH);

  //String ReadTest = EEPROM_Read(TAPS_EEPROM_LOCATION);
  //if (DBUG == "FRAM") Serial.print("\r\nRead: *" + ReadTest + "*\r\n");
        
  for (int i=1; i <=samplenumber; i++){

   Tlv493dMagnetic3DSensor.updateData();
   delay(100);

   if (digitalRead(buttonPin)) ButtonWasPushed();
   delay(100);

   digitalWrite(ledPin, LOW);
      
   xt = xt + Tlv493dMagnetic3DSensor.getX();
   yt = yt + Tlv493dMagnetic3DSensor.getY();
   zt = zt + Tlv493dMagnetic3DSensor.getZ();
   delay(100);

   //if (digitalRead(buttonPin)) ButtonWasPushed();
   delay(100);
       
  }
  
  TAPS = xt + yt + zt;

  digitalWrite(XBeeSleepPin, LOW);    // XBee Wake up
  delay(300);                         // *** Important to wait - destination XBee sees garbage otherwise
  
  if ((TAPS > TAPSNORMALPOSITIVE) or (TAPS < TAPSNORMALNEGATIVE)){
    STATUS = 'H';
  } else {
    STATUS = 'L';    
  }

  MESSAGE = "H" + String(STATUS);
  if (String(TAPS).substring(0,1) != "-"){
    MESSAGE = MESSAGE + "+";
  }

  MESSAGE = MESSAGE + String(TAPS);
  
  Serial.println(MESSAGE);
  for (int i=0; i < MESSAGE.length(); i++){
    XBee.write(MESSAGE[i]);
  }
  
  XBee.write(char(0x0d));

  TAPS =0;
     
  xt=0;
  yt=0;
  zt=0;
}

void ButtonWasPushed(){

    // turn LED on:
    digitalWrite(ledPin, HIGH);
    delay(2000);
    // turn LED off:
    digitalWrite(ledPin, LOW);
    delay(200);
    digitalWrite(ledPin, HIGH);
    delay(200);
    digitalWrite(ledPin, LOW);
    delay(200);
    digitalWrite(ledPin, HIGH);
    delay(200);
    digitalWrite(ledPin, LOW);
    delay(200);
    digitalWrite(ledPin, HIGH);
    delay(200);
    digitalWrite(ledPin, LOW);
    
}

String EEPROM_Read(int location){

  String Strtmp = "";

  // Store the number of bytes to read just after this location
  int charCount = i2ceeprom.read8(location);
  if (DBUG == "FRAM") Serial.print("Number of chars at location (" + String(location) + "): ");Serial.println(charCount);

  // If data has already been written here (not 0 or 255 b/c that seems to be default for byte values from factory),
  // read that data and return it as string
  if ((charCount != 0) and (charCount != 255)){
    for (int i = (location + 1); i <= (location + charCount); i++){
      uint8_t val = i2ceeprom.read8(i);
      char ch=char(val);
      Strtmp = Strtmp + ch;
      if (DBUG == "FRAM") Serial.println("location: " + String(i) + "=" + ch);
    }    
  }

  return Strtmp;
  
}

void EEPROM_Write(int location, String S){

  int sInx = 0;               // used to access each char of S (S[0...(SLen-1)])
  char SLen = S.length();     // number of bytes in S

  // write the number of bytes in S to the starting location in memory
  i2ceeprom.write8(location,SLen);

  if (DBUG == "FRAM") Serial.println("Number of chars written to location (" + String(location) + "): " + String(S.length()));

  // write the characters in String S to their locations.  i starts at location plus one and continues to the end location of S's length
  for (int i=(location + 1); i<=(location + SLen); i++){
    if (DBUG == "FRAM") Serial.print("Writing to location (" + String(i) + "): " + S[sInx] + "\r\n");
    i2ceeprom.write8(i,S[sInx]);
    sInx++;
  }
}
