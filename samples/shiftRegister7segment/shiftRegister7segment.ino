// -----------------------------------------------------------------------------
/*
  Using a 7 segment digit display, to display numbers from 0-9.
  Using a SN74HC595N Shift Register for serial to multiple pin outs.

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

void displayDigit(int theNumber) {
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
  switch (theNumber) {
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
  for (int numberToDisplay = 0; numberToDisplay < 11; numberToDisplay++) {
    Serial.print("+ numberToDisplay = ");
    Serial.println(numberToDisplay);
    displayDigit(numberToDisplay);
    delay(1000);
  }
}
// -----------------------------------------------------------------------------
