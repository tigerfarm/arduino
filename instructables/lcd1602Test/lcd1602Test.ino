// -----------------------------------------------------------------------------
/*
 * When syncing with a clock, start 8 seconds before the top,

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
  Clear the LCD screen:
    lcd.clear();
  Each character output to the display pushs the previous characters over by one space.
    lcd.autoscroll();
  Turns off automatic scrolling of the LCD.
    lcd.noAutoscroll()
  Displays the LCD cursor: an underscore (line) at the position of the next character to be printed.
    lcd.cursor()
  Hides the LCD cursor.
    lcd.noCursor()
*/
// -----------------------------------------------------------------------------
#include<Wire.h>

#include<LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

// -----------------------------------------------------------------------------
int displayColumns = 16;

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

int theCounter = 0;
void displayCount() {
  theCounter++;
  lcd.setCursor(10, 1);    // Column, Row
  lcd.print(theCounter);
  Serial.print("+ theCounter = ");
  Serial.println(theCounter);
}

// -----------------------------------------------------------------------------
// Device Setup

void setup() {
  Serial.begin(115200);
  // Give the serial connection time to start before the first print.
  delay(1000);
  Serial.println(""); // Newline after garbage characters.
  Serial.println("+++ Setup.");
  //
  lcd.init();
  lcd.backlight();
  Serial.println("+ LCD ready to use.");
  //                 1234567890123456
  displayPrintln(0, "Tiger Farm Press");
  String theLine = "Counter = " + String(theCounter);
  displayPrintln(1, theLine);
  // lcd.clear();
  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop

void loop() {
  delay(1000);
  displayCount();
}
// -----------------------------------------------------------------------------
