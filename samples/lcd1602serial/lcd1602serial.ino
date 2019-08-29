// -----------------------------------------------------------------------------
/*
  SDA - A4
  SCL - A5
*/
#include<Wire.h>

// #include<LiquidCrystal_I2C_Hangul.h>
// LiquidCrystal_I2C_Hangul lcd(0x3F,16,2);
#include<LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

// -----------------------------------------------------------------------------
int displayColumns = 16;

int theCounter = 0;
void displayOneSecondCount() {
  delay(992); // when 993, Arduino is sligtly behind.
  lcd.setCursor(8, 1);
  lcd.print(theCounter);
  theCounter++;
}

void displayPrintln(int theRow, String theString) {
  // To overwrite anything on the current line.
  String printString = theString;
  int theRest = displayColumns - theString.length();
  if (theRest < 0) {
    // Shorten to the display column length.
    printString = theString.substring(0, displayColumns);
  } else {
    // Buffer with spaces to the end of line.
    while (theRest < displayColumns) {
      printString = printString + " ";
      theRest++;
    }
  }
  lcd.setCursor(0, theRow);
  lcd.print(printString);
  /*  For testing.
  Serial.print("+ theRest = ");
  Serial.print(theRest);
  Serial.print(", printString :");
  Serial.print(printString);
  Serial.print(":");
  Serial.print(", theString :");
  Serial.print(theString);
  Serial.println(":");
  */
}

// -----------------------------------------------------------------------------
// Device Setup

void setup() {
  Serial.begin(9600);
  // Give the serial connection time to start before the first print.
  delay(1000);
  Serial.println("+++ Setup.");
  //
  lcd.init();
  lcd.backlight();
  Serial.println("+ print: Hello there.");
  //                1234567890123456
  displayPrintln(0,"Hello there.");
  delay(3000);
  lcd.clear();
  //
  Serial.println("+ Start counting.");
  //                1234567890123456
  displayPrintln(0,"Start counting.");
  lcd.setCursor(1, 1);
  lcd.print("Count#");
  displayOneSecondCount();
  displayOneSecondCount();
  displayOneSecondCount();
  displayOneSecondCount();
  displayOneSecondCount();
  displayOneSecondCount();
  displayPrintln(0, "Seconds,");
  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop

void loop() {
  displayOneSecondCount();
}

// -----------------------------------------------------------------------------
// eof
