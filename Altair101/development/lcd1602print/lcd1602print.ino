// -----------------------------------------------------------------------------
/*
   Test displaying characters to a 1602 LCD.

  1602 LCD serial connections,
    LCD - Nano - Mega
    SCL - A5   - SCL
    SDA - A4   - SDA
    VCC - 5V   - 5V
    GND - GND  - GND

  Reference,
    https://www.makerguides.com/character-i2c-lcd-arduino-tutorial/
  This causes each character output to the display to push previous characters over by one space. If the current text direction is left-to-right (the default), the display scrolls to the left.
    lcd.autoscroll();
  Turns off automatic scrolling of the LCD.
    noAutoscroll()
  Displays the LCD cursor: an underscore (line) at the position of the next character to be printed.
    cursor()
  Hides the LCD cursor.
    noCursor()
*/
#include<Wire.h>

#include<LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// 1602 LCD print functions.

const int displayColumns = 16;  // LCD has 16 columns for printing to.

int lcdColumn = 0;    // Current print column position.
int lcdRow = 0;       // Current print row/line.
String lcdRow0 = "";  // Buffer for row 0.
String lcdRow1 = "";  // Buffer for row 1.

//                        1234567890123456
String clearLineString = "                ";

// ------------------------------------------------
void lcdSetup() {
  lcd.init();
  lcd.backlight();
  //             1234567890123456
  lcdPrintln(0, "Altair 101");
  //             0123456789012345
  lcdPrintln(1, "LCD ready...");
  lcdRow0 = "Altair 101";
  lcdRow1 = "LCD ready...";
  lcd.setCursor(1, 12);
  lcd.cursor();
}

void lcdClear() {
  Serial.println(F("++ Clear screen."));
  lcd.clear();
  lcd.home();     // Set cursor home (0,0).
  lcdColumn = 0;
  lcdRow = 0;
  lcdRow0 = "";
  lcdRow1 = "";
}

void lcdBacklight(int theChar) {
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
}

// ----------------------------------------------
// Print line.

void lcdPrintln(int theRow, String theString) {
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

// ----------------------------------------------
void lcdScroll() {
  //Serial.println(F("+ lcdScroll()"));
  // -----------------------
  // Place cursor at start of the second line.
  lcdColumn = 0;
  if (lcdRow == 0) {
    // If the first time printing to a clear screen, no need to scroll the data.
    lcd.setCursor(0, 1);
    lcdRow = 1;
    return;
  }
  // -----------------------
  // Scroll the data.
  //
  // Clear the screen befor moving row 1 to 0, and row 1 needs to be clear.
  lcd.clear();
  // Screen buffers: scroll row 1, up to row 0.
  lcdRow0 = lcdRow1;
  lcdPrintln(0, lcdRow0);
  // Clear row 1 buffer.
  lcdRow1 = "";
  lcd.setCursor(0, 1);
}

// ----------------------------------------------
// Print character.

void lcdPrintChar(String theChar) {
  Serial.print(F("+ lcdPrintChar :"));
  Serial.print(theChar);
  Serial.println(F(":"));
  // ----------------------------------------------
  // New line character
  if (theChar == "\n") {
    lcdScroll();
    delay(1000);
    return;
  }
  // ----------------------------------------------
  // Print character to the display.
  // Stacy, update for print past end of line (16 characters in a line).
  if (lcdColumn >= displayColumns) {
    return;
  }
  if (lcdRow == 0) {
    lcdRow0 = lcdRow0 + theChar;
  } else {
    lcdRow1 = lcdRow1 + theChar;
  }
  lcd.print(theChar);
  lcdColumn++;
  if (lcdColumn < displayColumns) {
    // Move the cursor forward.
    lcd.setCursor(lcdColumn, lcdRow);
  }
}

// -----------------------------------------------------------------------------
// Device Setup

void setup() {
  Serial.begin(115200); // 115200 or 9600
  delay(1000);        // Give the serial connection time to start before the first print.
  Serial.println(""); // Newline after garbage characters.
  Serial.println(F("+++ Setup."));

  // ----------------------------------------------------
  lcdSetup();
  Serial.println(F("+ LCD ready for output."));

  // ----------------------------------------------------
  delay(2000);
  Serial.println(F("+ LCD test."));
  /*
  lcdBacklight(0);
  delay(2000);
  lcdBacklight(1);
  */
  delay(2000);
  lcdClear();
  delay(2000);
  Serial.println(F("++ Print 'a b c.'"));
  lcdPrintChar("a");
  delay(300);
  lcdPrintChar(" ");
  lcdPrintChar("b");
  delay(300);
  lcdPrintChar(" ");
  lcdPrintChar("c");
  lcdPrintChar(".");
  delay(2000);
  Serial.println(F("++ Print new line, '/n'"));
  lcdPrintChar("\n");
  delay(2000);
  Serial.println(F("++ Print 'd e f.'"));
  lcdPrintChar("d");
  lcdPrintChar(" ");
  lcdPrintChar("e");
  lcdPrintChar(" ");
  lcdPrintChar("f");
  lcdPrintChar(".");
  delay(2000);
  Serial.println(F("++ Print new line character, causing scrolling #1."));
  lcdPrintChar("\n");
  lcdPrintChar("N");
  lcdPrintChar("L");
  lcdPrintChar(" ");
  lcdPrintChar("#");
  lcdPrintChar("1");
  delay(2000);
  Serial.println(F("++ Print new line character, causing scrolling #2."));
  lcdPrintChar("\n");
  lcdPrintChar("N");
  lcdPrintChar("L");
  lcdPrintChar(" ");
  lcdPrintChar("#");
  lcdPrintChar("2");
  delay(2000);
  Serial.println(F("++ Print new line character, causing scrolling #3."));
  lcdPrintChar("\n");
  lcdPrintChar("N");
  lcdPrintChar("L");
  lcdPrintChar(" ");
  lcdPrintChar("#");
  lcdPrintChar("3");
  delay(2000);
  Serial.println(F("++ Print to end of line."));
  lcdPrintChar("\n");
  int pI;
  for (int i = 1; i <= displayColumns; i++) {
    pI = i;
    if (i>9) {
      pI = i-10;
    }
    lcdPrintChar(String(pI));
    delay(500);
  }
  delay(2000);
  Serial.println(F("++ Print new line character, EOL."));
  lcdPrintChar("\n");
  lcdPrintChar("E");
  lcdPrintChar("O");
  lcdPrintChar("L");
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
