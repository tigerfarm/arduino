// -----------------------------------------------------------------------------
/*
  Shifting to Expand I/O.

  Using a 74HC595 Shift Register for serial to multiple pin outs.

  + Using 74HC595 for inputs:
  https://www.theengineeringprojects.com/2018/11/arduino-74hc165-interfacing-increase-input-pins.html

  + Using 74HC595 for inputs:
  https://forum.arduino.cc/index.php?topic=163813.0
  https://www.youtube.com/watch?v=nXl4fb_LbcI
  https://www.youtube.com/watch?v=hR6qOhUeKMc
*/
// -----------------------------------------------------------------------------
// Shift Register

#include <SPI.h>
byte Input, Output, Check = 1;
int j;

void pin_read() {
  for (j = 0; j < 50; j++) {
    delayMicroseconds(1000);
  }
  Check = 1;
  for (j = 0; j < 8; j++) {
    SPI.transfer(Check);
    SPI.transfer(Output);
    digitalWrite(4, HIGH);
    digitalWrite(4, LOW);
    delayMicroseconds(500);
    if (digitalRead(2) == HIGH)
      bitWrite(Output, j, 1);
    else
      bitWrite(Output, j, 0);
    Check = Check << 1;
  }
  SPI.transfer(255);
  SPI.transfer(Output);
  digitalWrite(4, HIGH);
  digitalWrite(4, LOW);
}

// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(1000);        // Give the serial connection time to start before the first print.
  Serial.println(""); // Newline after garbage characters.
  Serial.println("+++ Setup.");

  pinMode(13, OUTPUT);    //clock
  pinMode(11, OUTPUT);    //data
  pinMode(4, OUTPUT);     //latch
  pinMode(2, INPUT);      //Input from buttons
  //
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.begin();
  SPI.transfer(255);
  SPI.transfer(0);
  digitalWrite(4, HIGH);
  digitalWrite(4, LOW);
  
  attachInterrupt(0, pin_read, RISING);
  Serial.println("+ Ready for input.");

  Serial.println("+++ Start program loop.");
}

// -----------------------------------------------------------------------------
// Device Loop.

void loop() {
  Serial.println("+ Looping");
  delay(1000);
}
// -----------------------------------------------------------------------------
