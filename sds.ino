//! Arduino File to Test SDS011 Library

/// @file sds.ino 
/// @author Matthias Budde / Sajjad Hussain
/// @version 0.1

#include "sds011lib.h"

float p10, p25;
uint8_t err;
sds011 my_sds;
HardwareSerial port(2);
int8_t SDS011_TXD = 17;
int8_t SDS011_RXD = 13;

void setup() {
  
  delay(1000);
  Serial.begin(115200);
  Serial.println("PM Sensor Set ");
  my_sds.init(&port, SDS011_RXD, SDS011_TXD);
  Serial.println("PM Sensor Set... ");
}

void loop() 
{
  err = my_sds.read(&p25, &p10);
  if (!err) {
    Serial.println("P2.5: " + String(p25));
    Serial.println("P10:  " + String(p10));
  }
  delay(100);
}
