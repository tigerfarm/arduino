#include "Arduino.h"
#include "PCF8574.h"

// Set i2c HEX address
PCF8574 pcf8574(0x20);
unsigned long timeElapsed;

void setup(){
  Serial.begin(9600);

  pcf8574.pinMode(P0, INPUT);
  pcf8574.pinMode(P1, OUTPUT);

  pcf8574.begin();
}

void loop(){
  uint8_t val = pcf8574.digitalRead(P1);            // Read the value of pin P0        
  if (val == HIGH)  pcf8574.digitalWrite(P0, HIGH); // If Button is Pressed
  else              pcf8574.digitalWrite(P0, LOW);  // When Button is Released
  delay(50);
}
