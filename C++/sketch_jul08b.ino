#include <Wire.h>
int address1 = 77; // hexadecimal address of ps

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  int c1 = read_pressure(address1);
  Serial.print("Sensor 1: ");
  Serial.println(c1);
  delay(500);
}

int read_pressure(int address) {
  //start the communication with IC with the address xx
  Wire.beginTransmission(address); 
  //send a bit and ask for register zero
  Wire.write(0);
  //end transmission
  Wire.endTransmission();
  //request 1 byte from address xx
  Wire.requestFrom(address, 1);
  //wait for response
  while(Wire.available() == 0);
  //put the temperature in variable c
  int c = Wire.read();   
  return c;
}
