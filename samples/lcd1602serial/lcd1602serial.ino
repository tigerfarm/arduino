// -----------------------------------------------------------------------------
/*
  SDA - A4
  SCL - A5
 */
#include<Wire.h>

// #include<LiquidCrystal_I2C_Hangul.h>
// LiquidCrystal_I2C_Hangul lcd(0x3F,16,2);
#include<LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,20,4);

// -----------------------------------------------------------------------------
// Device Setup

void setup() {
  Serial.begin(9600);
  delay(100);
  Serial.println("+++ Setup.");
  lcd.init();
  lcd.backlight();
  //
  Serial.println("+ print: Hello there.");
  // lcd.setCursor(0,0);
  //         1234567890123456
  lcd.print("Hello there.");
  delay(3000);
  //
  Serial.println("+++ Go to loop.");
  lcd.clear();
  //         1234567890123456
  lcd.print("Start looping.");
  lcd.setCursor(1,1);
  lcd.print("Loop#");
}

// -----------------------------------------------------------------------------
// Device Loop

int loopCounter = 0;
int iPosition = 0;

void loop() {
  delay(1000);
  ++loopCounter;
  // Serial.print("+ loopCounter = ");
  // Serial.println(loopCounter);
  lcd.setCursor(7,1);
  lcd.print(loopCounter);
}

// -----------------------------------------------------------------------------
