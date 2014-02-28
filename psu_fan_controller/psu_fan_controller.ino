#include <Wire.h>

int fanPin = 3;     // set output pin for the fan

//void get_ambient();

float maxtemp = 0;
int fanspeed = 0;

void setup()
{
  
  Serial.begin(9600);        
  Serial.println("Sensor Configuration");
  TWBR=400000L;
  init_tempmodule(0x48);
  init_tempmodule(0x49);
  init_tempmodule(0x4A);
  init_tempmodule(0x4B);
  TCCR2B = TCCR2B & 0b11111000 | 0x01;  // set the pwm frequency outside of audiable range
   pinMode(fanPin, OUTPUT);
}

void loop()
{
  maxtemp = 0;
  Serial.print("Temperature 0x48:");Serial.println(get_temperature(0x48));
  Serial.print("Temperature 0x49:");Serial.println(get_temperature(0x49));
  Serial.print("Temperature 0x4A:");Serial.println(get_temperature(0x4A));
  Serial.print("Temperature 0x4B:");Serial.println(get_temperature(0x4B));
  if (get_temperature(0x48) > maxtemp){
     maxtemp = get_temperature(0x48); 
  }
  if (get_temperature(0x49) > maxtemp){
     maxtemp = get_temperature(0x49); 
  }
  if (get_temperature(0x4A) > maxtemp){
     maxtemp = get_temperature(0x4A); 
  }
  if (get_temperature(0x4B) > maxtemp){
     maxtemp = get_temperature(0x4B); 
  }
  
  fanspeed = min(max(70, 7.5 * (max(1, maxtemp - 30))), 255);
  analogWrite(fanPin, fanspeed); 

  Serial.print("Max Temperature:");Serial.println(maxtemp);
  Serial.print("Fan Speed:");Serial.println(fanspeed);
  delay(1000);
}

void init_tempmodule(int Address){
  Wire.begin();              
  Serial.println("Configuring MCP9801");
  
  Wire.beginTransmission(Address);
  Wire.write((byte) 0x01); // point to config reg
  Wire.write((byte) B01100000);
  Wire.endTransmission();

  Wire.beginTransmission(Address);
  Wire.write((byte) 0x01); // point to config reg
  Wire.endTransmission();
  Wire.requestFrom(Address, 1);
  Serial.println("# Configurating MCP9801 Complete");
  
  byte x = 0xff;
  
  if (Wire.available()) {
    x = Wire.read();
  }
  if (x != B01100000) {
    Serial.println("Error configuring MCP9801");
  } else {
    Serial.println("MCP9801 Configuration Complete");
  }
  
  Wire.beginTransmission(Address);
  Wire.write((byte) 0x00);
  Wire.endTransmission();
}

float get_temperature(int Address)
{ 
  Wire.requestFrom(Address, 2);
  if (Wire.available() != 2) {
    Serial.println("Incorrect number of bytes available");
    return 0;
  }
  byte a = Wire.read();
  byte b = Wire.read();
  
  float ambienttemp = a;
  ambienttemp += (float)b / 256. ;
  return ambienttemp;
}
