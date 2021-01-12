#include "SoftwareSerial.h"
SoftwareSerial XBee(16, 17);  // On The HUZZAH32 Feather.

char incomingByte = 0; // for incoming serial data
String XBeeResponded = "";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  XBee.begin(9600);  
}

void loop() {
  // put your main code here, to run repeatedly:

  String command = readSerialInput();

  if (command.length() >0){
    Serial.print("Command: *"); Serial.print(command); Serial.println("*");
    XBeeResponded = SendATtoXBee(command);
    command = "";
    Serial.println(XBeeResponded);
  }
  
}


String readSerialInput(){
  char incomingByte = 0; // for incoming serial data
  String mdata = "";
  while (Serial.available() >0){
    incomingByte = Serial.read();
    mdata = mdata + incomingByte;
  }
  return mdata;
}

String SendATtoXBee(String M){
  String Response;
  bool bOK = false;

  if (M.substring(0,3) == "+++"){
    Serial.println("Triple Pluse");
    delay(1200);
    XBee.print("+++");
    delay(900);
  } else {
    for (int i=0; i < M.length(); i++){
      Serial.print("-");
      Serial.print(M.charAt(i));
      XBee.write(M.charAt(i));
    }
    XBee.write(0x0D);
  }
  
  delay(300);

  while (XBee.available() > 0) {
    Serial.write(XBee.read());
     bOK = true;
  }

  /*
  while (XBee.available() > 0) {
      int din = XBee.read();
      delay(200);
      Serial.write(din);   
    }
    

  while (XBee.available() > 0) {
    char din = XBee.read();
    delay(200);
    Serial.write(din);   
    Response = Response + din;
  }  
*/

  return Response;
}
