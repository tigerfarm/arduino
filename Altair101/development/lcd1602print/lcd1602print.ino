// -----------------------------------------------------------------------------
/*
   Test displaying characters to a 1602 LCD.

  1602 LCD serial connections,
    LCD - Nano - Mega
    SCL - A5   - SCL
    SDA - A4   - SDA
    VCC - 5V   - 5V
    GND - GND  - GND
*/
#include<Wire.h>

#include<LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

// -----------------------------------------------------------------------------
// 1602 LCD print functions.

int lcdColumn = 0;    // Current print column position.
int lcdRow = 0;       // Current print row/line.
String lcdRow0 = "";  // Buffer for row 0.
String lcdRow1 = "";  // Buffer for row 1.

// ----------------------------------------------
// Print line.

//                        1234567890123456
String clearLineString = "                ";
String theLine = "";
const int displayColumns = 16;

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

void lcdControl(int theChar) {
  // ----------------------------------------------
  if (theChar == 0) {
    Serial.println(F("++ Backlight off."));
    lcd.noBacklight();
    return;
  }
  if (theChar == 1) {
    Serial.println(F("++ Backlight on."));
    lcd.backlight();
    return;
  }
  // ----------------
  if (theChar == 2) {
    Serial.println(F("++ Clear screen."));
    lcd.clear();
    lcd.home();     // Set cursor home (0,0).
    lcdColumn = 0;
    lcdRow = 0;
    return;
  }
}

// ----------------------------------------------
// Print character.

void printLcdChar(String theChar) {
  // ----------------------------------------------
  // New line character
  if (theChar == "\n") {
    // Place cursor at start of the second line.
    lcdColumn = 0;
    lcd.setCursor(0, 1);
    if (lcdRow == 0) {
      lcdRow = 1;
      return;
    }
    // Scroll row 1 to row 0, and clear row 1.
    // Move row 1 to row 0.
    // displayPrintln(0, lcdRow1);
    // Clear row 1.
    lcdRow1 = "";
    displayPrintln(1, clearLineString);
    return;
  }
  // ----------------------------------------------
  // Print character to the display.
  if (lcdColumn >= displayColumns) {
    return;
  }
  if (lcdRow == 0) {
    lcdRow0 = lcdRow0 + theChar;
  } else {
    lcdRow1 = lcdRow1 + theChar;
  }
  lcd.setCursor(lcdColumn, lcdRow);
  lcd.print(theChar);
  lcdColumn++;
}

  // ----------------------------------------------
void readyLcd() {
  lcd.init();
  lcd.backlight();
  //                 1234567890123456
  displayPrintln(0, "Altair 101");
  displayPrintln(1, "LCD ready...");
}

// -----------------------------------------------------------------------------
// Device Setup

void setup() {
  Serial.begin(115200); // 115200 or 9600
  delay(1000);        // Give the serial connection time to start before the first print.
  Serial.println(""); // Newline after garbage characters.
  Serial.println(F("+++ Setup."));

  // ----------------------------------------------------
  readyLcd();
  Serial.println(F("+ LCD ready for output."));

  // ----------------------------------------------------
  delay(2000);
  Serial.println(F("+ LCD test."));
  lcdControl(0);
  delay(2000);
  lcdControl(1);
  delay(2000);
  lcdControl(2);
  delay(2000);
  Serial.println(F("++ Print 'a b c.'"));
  printLcdChar("a");
  delay(300);
  printLcdChar(" ");
  printLcdChar("b");
  delay(300);
  printLcdChar(" ");
  printLcdChar("c");
  printLcdChar(".");
  delay(300);
  Serial.println(F("++ Print new line, '/n'"));
  printLcdChar("\n");
  delay(2000);
  Serial.println(F("++ Print 'd e f.'"));
  printLcdChar("d");
  printLcdChar(" ");
  printLcdChar("e");
  printLcdChar(" ");
  printLcdChar("f");
  printLcdChar(".");
  delay(2000);
  Serial.println(F("++ Print new line character, causing scrolling #1."));
  printLcdChar("\n");
  printLcdChar("N");
  printLcdChar("L");
  printLcdChar(" ");
  printLcdChar("#");
  printLcdChar("1");
  delay(2000);
  Serial.println(F("++ Print new line character, causing scrolling #2."));
  printLcdChar("\n");
  printLcdChar("N");
  printLcdChar("L");
  printLcdChar(" ");
  printLcdChar("#");
  printLcdChar("2");
  delay(2000);
  Serial.println(F("++ Print new line character, causing scrolling #3."));
  printLcdChar("\n");
  printLcdChar("N");
  printLcdChar("L");
  printLcdChar(" ");
  printLcdChar("#");
  printLcdChar("3");
  delay(2000);

  // ----------------------------------------------------
  Serial.println(F("+ Starting the processor loop."));
}

// -----------------------------------------------------------------------------
// Device Loop

void loop() {
  Serial.println(F("+ Looping."));
  delay(3000);
}
// -----------------------------------------------------------------------------
