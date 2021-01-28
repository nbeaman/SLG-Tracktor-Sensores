
#include "Adafruit_EEPROM_I2C.h"

Adafruit_EEPROM_I2C i2ceeprom;

String DBUG = "FRAM";

//---[ FRAM ]-------------------------------------
// FRAM Variables
#define EEPROM_ADDR         0x50  // the default address of the I2C device
int TAPS_EEPROM_LOCATION =  0;
//------------------------------------------------

 boolean done = false;
 
 
void setup() {
  // put your setup code here, to run once:
  delay(3000);
  Serial.begin(115200);

  //---[ FRAM ]-------------------------------------
  if (i2ceeprom.begin(EEPROM_ADDR)) {     // FRAM i2c addr begin
    if (DBUG == "FRAM") Serial.println("Found I2C EEPROM");
  } else {
    if (DBUG == "FRAM") Serial.println("I2C EEPROM not identified ... check your connections?\r\n");
    while (1) delay(10);
  }
  //------------------------------------------------
  
}

void loop() {

  //------------------------------------------------
  // Test Code Here (done is used to only run one time since FRAM has a limit of 10,000 writes
  if (!done){

      String ReadTest = "";
      ReadTest = EEPROM_Read(TAPS_EEPROM_LOCATION);
      if (DBUG == "FRAM") Serial.print("\r\nRead: *" + ReadTest + "*\r\n");

      // Write a string to memory (commented out b/c of FRAM number of write limitation of 10,000
      //EEPROM_Write(TAPS_EEPROM_LOCATION, "Beaman");
      
      done = true;
  }
  //------------------------------------------------

  
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

 void EEPROM_SHOW_ALL(){

   // dump the first 256 bytes of memory
    uint8_t val;
    for (uint16_t addr = 0; addr < 256; addr++) {
      val = i2ceeprom.read8(addr);
      if ((addr % 32) == 0) {
        Serial.print("\n 0x"); Serial.print(addr, HEX); Serial.print(": ");
      }
      Serial.print("0x"); 
      if (val < 0x10) 
        Serial.print('0');
      Serial.print(val, HEX); Serial.print(" ");
    }
 }
