// -----------------------------------------------------------------------------
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
  lcd.print("Keypad test....|");
  //
  lcd.setCursor(0, 1);
  lcd.print("Get ready.");
  //
  delay(3000);
  Serial.println("+++ Go to loop.");
  lcd.setCursor(0, 1);
  lcd.print("Press Key:");
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
  lcd.setCursor(10, 1);
  if (x < 60) {
    lcd.print ("Right ");
  }
  else if (x < 200) {
    lcd.print ("Up    ");
  }
  else if (x < 400) {
    lcd.print ("Down  ");
  }
  else if (x < 600) {
    lcd.print ("Left  ");
  }
  else if (x < 800) {
    lcd.print ("Select");
  }
}

// -----------------------------------------------------------------------------
