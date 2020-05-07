// -----------------------------------------------------------------------------
// Uno keypad 1602 LCD shield.
// Reference,
//    https://create.arduino.cc/projecthub/electropeak/using-1602-lcd-keypad-shield-w-arduino-w-examples-e02d95
//
// Others with Select button not working:
//  http://forums.netduino.com/index.php?/topic/4035-freetronics-lcd-select-keypad-problem/
//    bend the A0 pin on the terminal shield and leave it sticking out (or under),
//    use the prototyping area to wire a resistor between A0 and A1, and a second resistor between A1 and ground,
//    (5600 and 2700 resisters)
//    change the Netduino code to use A1 as the input.

#include<LiquidCrystal.h>

//LCD pin to Arduino
const int pin_RS = 8;
const int pin_EN = 9;
const int pin_d4 = 4;
const int pin_d5 = 5;
const int pin_d6 = 6;
const int pin_d7 = 7;
const int pin_BL = 10;
LiquidCrystal lcd( pin_RS,  pin_EN,  pin_d4,  pin_d5,  pin_d6,  pin_d7);

const int pin_SL = A0;

// -----------------------------------------------------------------------------
// Device Setup

void setup() {
  Serial.begin(9600);
  delay(100);
  Serial.println("+++ Setup.");
  //
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  //         1234567890123456
  lcd.print("Press keypad key");
  lcd.setCursor(0, 1);
  lcd.print("Key:");
  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop

int loopCounter = 0;
int iPosition = 0;

void loop() {
  int x;
  x = analogRead(0);
  //
  // If noting is pressed, x = 1023
  // delay(500);
  // Serial.print("+ x = ");
  // Serial.println(x);
  //
  lcd.setCursor(6, 1);
  if (x < 60) {
    //     1234567890123456
    lcd.print ("Right      ");
    lcd.setCursor(13, 1);
    lcd.print (x);
  }
  else if (x < 200) {
    //     1234567890123456
    lcd.print ("Up         ");
    lcd.setCursor(13, 1);
    lcd.print (x);
  }
  else if (x < 400) {
    //     1234567890123456
    lcd.print ("Down       ");
    lcd.setCursor(13, 1);
    lcd.print (x);
  }
  else if (x < 600) {
    //     1234567890123456
    lcd.print ("Left       ");
    lcd.setCursor(13, 1);
    lcd.print (x);
  }
  else if (x < 920) {
    // Doesn't work for me.
    //     1234567890123456
    lcd.print ("Select .   ");
    lcd.setCursor(13, 1);
    lcd.print (x);
  }
  else if (x != 1023) {
    //     1234567890123456
    lcd.print ("key?       ");
    lcd.setCursor(13, 1);
    lcd.print (x);
  }
  delay(100);
}

// -----------------------------------------------------------------------------
