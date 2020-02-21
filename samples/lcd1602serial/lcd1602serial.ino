// -----------------------------------------------------------------------------
/*
 * When syncing with a clock, start 8 seconds before the top,
 *  i.e. turn the power on at 52 seconds.

-----------------------------------------------------------------------------
  1602 LCD serial connections pins,
    LCD - Nano - Mega - Mega optional
    SCL - A5   - SCL  - 21
    SDA - A4   - SDA  - 20
    VCC - 5V   - 5V
    GND - GND  - GND

-----------------------------------------------------------------------------
  Reference,
    https://www.makerguides.com/character-i2c-lcd-arduino-tutorial/
  Each character output to the display pushs the previous characters over by one space.
    lcd.autoscroll();
  Turns off automatic scrolling of the LCD.
    noAutoscroll()
  Displays the LCD cursor: an underscore (line) at the position of the next character to be printed.
    cursor()
  Hides the LCD cursor.
    noCursor()
    
-----------------------------------------------------------------------------
*/
#include<Wire.h>

#include<LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

// -----------------------------------------------------------------------------
int displayColumns = 16;
//                        1234567890123456
String clearLineString = "                ";

int theCounter = 0;
int theSyncCounter = 0;
int theSyncDelay = 600;
void displayOneSecondCount() {
  //
  // With a delay of 993, my Arduino Nano is a small bit slower than actual time.
  if (theSyncCounter == 500) {
    // Have a longer delay to get back in sync.
    delay(theSyncDelay);
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
  String theLine = "SyncDelay = " + String(theSyncDelay);
  displayPrintln(1, theLine);
  delay(3000);
  lcd.clear();
  //
  // How to turn off the red LED on the LCD serial board?
  //
  Serial.println("+ Start counting.");
  //                 1234567890123456
  displayPrintln(0, "Start counting.");
  displayPrintln(1, "Count#");
  displayOneSecondCount();
  displayOneSecondCount();
  displayOneSecondCount();
  displayOneSecondCount();
  displayOneSecondCount();
  displayOneSecondCount();
  lcd.setCursor(0, 0);  // col, row
  lcd.print(clearLineString);
  lcd.setCursor(0, 1);
  //        "Count#"
  lcd.print("      ");
  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop

void loop() {
  displayOneSecondCount();
}
// -----------------------------------------------------------------------------
