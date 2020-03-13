// -----------------------------------------------------------------------------
/*
  Using a 7 segment digit display, to display numbers from 0-9.
  Using a SN74HC595N shift register for serial to multiple pin outs.

  For a 1 digit display, numbers from 0-9,
  + Only requires 3 digital Arduino pins to control the 8 digital display pins.

  This technic will work for multiple displays, using 1 shift register per display.
  + The shift registers are daisy chained together.
  For a 2 digit display, numbers from 00-99,
  + Only requires 3 digital Arduino pins to control the 16 digital display pins.

  74HC595 is a SIPO (Serial-In-Parallel-Out) shift registers,
  + 74HC595 pin 16: 5V+
  + 74HC595 pin 15: LED 0   DP "."
  + 74HC595 pin 14: Data    pin (SRCLK) to Nano pin 4, data transfer from Nano to 595.
  + 74HC595 pin 13: Ground (-)
  + 74HC595 pin 12: Latch   pin (RCLK) to Nano pin 5, which does the Parallel-Out task to the 8 output pins.
  + 74HC595 pin 11: Clock   pin (SER)  to Nano pin 6, clock signal to say that the data is ready.
  + 74HC595 pin 10: 5V+
  + 74HC595 pin 09: Daisy chain to next 74HC595, pin 14 (data). Not used in single 74HC595.
  + 74HC595 pin 08: Ground (-)
  ------------------------ Segment
  + 74HC595 pin 07: LED 7.  G
  + 74HC595 pin 06: LED 6.  F
  + 74HC595 pin 05: LED 5.  E
  + 74HC595 pin 04: LED 4.  D
  + 74HC595 pin 03: LED 3.  C
  + 74HC595 pin 02: LED 2.  B
  + 74HC595 pin 01: LED 1.  A
  + 74HC595 pin 15: LED 0   DP "."

  Segment pins for common cathode display (-).
    1 2 3 4 5 (Pin 5 is top right when facing the display)
    G F - A B : middle pin goes to resister, to ground.
        A
       ---
     F|   |B
       ---   G is the middle bar.
     E|   |C
       --- .
        D
    E D - C DP
    1 2 3 4 5

----------------------------------------------------------------------
Cable wire mapping from clock segment display pins to the shift register pins.

+ Pins from the back.
12345 67890
12345 12345
BA-FG BA-FG
PC-DE PC-DE

BA-FG BA-FG
12345 12345 - Segment display top pins
12345 67890 - Cable wires
21-67 21-67 - Shift register pins
+ Examples,
++ Cable wire 1, goes from segment #1 top pin 1, to shift register #1 pin 2.
++ Cable wire 2, goes from segment #1 top pin 2, to shift register #1 pin 1.
...
++ Cable wire 6, goes from segment #2 top pin 1, to shift register #2 pin 2.
++ Cable wire 7, goes from segment #2 top pin 2, to shift register #2 pin 1.
...

P  C-DE P  C-DE
1  2345 1  2345 - Segment display bottom pins
1  2345 6  7890 - Cable wires
15 3-45 15 3-45 - Shift register pins

*/
// -----------------------------------------------------------------------------
// Shift Register
const int dataPin = 4;      // Connected to 74HC595 Data  pin 14.
const int latchPin = 5;     // Connected to 74HC595 Latch pin 12.
const int clockPin = 6;     // Connected to 74HC595 Clock pin 11.

byte dataByte = B01010101;

void updateShiftRegister(byte dataByte) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, dataByte);
  digitalWrite(latchPin, HIGH);
}

void updateShiftRegister2(byte dataByte1, byte dataByte2) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, dataByte1);
  shiftOut(dataPin, clockPin, LSBFIRST, dataByte2);
  digitalWrite(latchPin, HIGH);
}

void displayDigit(int theDigit) {
  /*
    Segment pins for common cathode display (-).
      G F - A B : middle pin goes to resister, to ground.
          A
         ---
       F|   |B
         ---   G is the middle bar.
       E|   |C
         --- .
          D
      E D - C DP
  */
  switch (theDigit) {
    case 0:
      //                  0:ABCDEFG
      updateShiftRegister(B01111110);
      break;
    case 1:
      updateShiftRegister(B00110000);
      break;
    case 2:
      updateShiftRegister(B01101101);
      break;
    case 3:
      updateShiftRegister(B01111001);
      break;
    case 4:
      updateShiftRegister(B00110011);
      break;
    case 5:
      updateShiftRegister(B01011011);
      break;
    case 6:
      updateShiftRegister(B00011111);
      break;
    case 7:
      updateShiftRegister(B01110000);
      break;
    case 8:
      updateShiftRegister(B01111111);
      break;
    case 9:
      updateShiftRegister(B01111011);
      break;
    default:
      // Display "E." for error.
      updateShiftRegister(B11001111);
  }
}

// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(1000);        // Give the serial connection time to start before the first print.
  Serial.println(""); // Newline after garbage characters.
  Serial.println("+++ Setup.");

  // ------------------------------------------------------------
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  delay(300);
  Serial.println("+ Connection to the 595 is set, v2b.");

  // ------------------------------------------------------------
  Serial.println("+++ Start program loop.");
}

// -----------------------------------------------------------------------------
// Device Loop.

void loop() {
  for (int digitToDisplay = 0; digitToDisplay < 11; digitToDisplay++) {
    Serial.print("+ digitToDisplay = ");
    Serial.println(digitToDisplay);
    displayDigit(digitToDisplay);
    delay(1000);
  }
}
// -----------------------------------------------------------------------------
