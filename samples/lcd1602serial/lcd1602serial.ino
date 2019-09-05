// -----------------------------------------------------------------------------
/*
 * When syncing with a clock, start 8 seconds before the top,
 *  i.e. turn the power on at 52 seconds.

  SDA - A4
  SCL - A5
*/
#include<Wire.h>

// #include<LiquidCrystal_I2C_Hangul.h>
// LiquidCrystal_I2C_Hangul lcd(0x3F,16,2);
#include<LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

#define LCD_LIGHT_PIN A4 // The number of the pin where anode of the display backlight is.

// -----------------------------------------------------------------------------
int displayColumns = 16;
//                        1234567890123456
String clearLineString = "                ";

int theCounter = 0;
int theSyncCounter = 0;
void displayOneSecondCount() {
  //
  // With a delay of 993, my Arduino Nano is a small bit slower than actual time.
  if (theSyncCounter == 500) {
    // Have a longer delay to get back in sync.
    delay(910);
    theSyncCounter = 0;
  } else {
    delay(993);
  }
  //
  lcd.setCursor(8, 1);    // Column, Row
  lcd.print(theCounter);
  theCounter++;
  theSyncCounter++;
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
  displayPrintln(0, "Hello there.");
  delay(3000);
  lcd.clear();
  //
  // Try turning off the red LED.
  // Need to find out how.
  // Set the lcd display backlight anode pin to low - lcd light off.
  digitalWrite(LCD_LIGHT_PIN, LOW);
  //
  Serial.println("+ Start counting.");
  //                 1234567890123456
  displayPrintln(0, "Start counting.");
  lcd.setCursor(1, 1);
  lcd.print("Count#");
  displayOneSecondCount();
  displayOneSecondCount();
  displayOneSecondCount();
  displayOneSecondCount();
  displayOneSecondCount();
  displayOneSecondCount();
  // displayPrintln(0, "Seconds,");
  lcd.setCursor(0, 0);
  lcd.print(clearLineString);
  lcd.setCursor(1, 1);
  // lcd.print("Count#");
  lcd.print("      ");
  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop

void loop() {
  displayOneSecondCount();
}

// -----------------------------------------------------------------------------
// eof
