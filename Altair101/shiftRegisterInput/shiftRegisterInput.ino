// -----------------------------------------------------------------------------
/*
  Shifting to Expand I/O.

  Using a 74HC595 Shift Register, serial to parallel, to read multiple pin inputs.
  Uses 3 or 4 pins on the microcontroller.

  Wiring,
  + 8 buttons, one side all connected to Nano digital pin: 3.
  ++ If using the interupt method, use Nano pin 2 or 3.
  + Attach a 10K pull down resister to the Nano digital pin.
  + The other side of the buttons, each goes to a diode, then to a sift register output.
  ++ Diodes handle multiple key presses, add a diode to stop shorts.

  Program steps,
  + Shift out 255, to turn the all shift register outputs high: Pins: 15, 1-7 are high.
  ++ Since no button is pressed. the Nano digital input is low.
  + When a button is pressed, the interupt is triggered.
  ++ Or, can use a none interupt, just monitor the change.
  + Then scan the buttons by shifting 1 high bit through each, to find out which button was pressed.
    Shift out B10000000. If high on the Nano pin, then this button was pressed.
    Shift out B01000000. If high on the Nano pin, then this button was pressed.
    Shift out B00100000. If high on the Nano pin, then this button was pressed.
    Shift out B00010000. If high on the Nano pin, then this button was pressed.
    Shift out B00001000. If high on the Nano pin, then this button was pressed.
    Shift out B00000100. If high on the Nano pin, then this button was pressed.
    Shift out B00000010. If high on the Nano pin, then this button was pressed.
    Shift out B00000001. If high on the Nano pin, then this button was pressed.
  + To check for multiple button presses, scan all buttons.
  + To check for a single button presses, scan until one is found.

  + The above technique is of using 74HC595 for inputs, is from the following:
  https://www.youtube.com/watch?v=nXl4fb_LbcI
  https://www.youtube.com/watch?v=1Vb2BpxPnjU
  + Link where I found the video links:
  https://forum.arduino.cc/index.php?topic=163813.0

  + Another method, use 74HC165 for inputs, which is parallel to serial:
  https://www.youtube.com/watch?v=hR6qOhUeKMc
  https://www.theengineeringprojects.com/2018/11/arduino-74hc165-interfacing-increase-input-pins.html
*/
// -----------------------------------------------------------------------------
// Shift Register

const int dataPin = 4;            // 74HC595 Data  pin 12 is connected to Digital pin 4
const int latchPin = 5;           // 74HC595 Latch pin 14 is connected to Digital pin 5
const int clockPin = 6;           // 74HC595 Clock pin 11 is connected to Digital pin 6
const int dataInputPin = 3;       // Nano digital data input check pin.

void updateShiftRegister(byte dataByte) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, dataByte);
  digitalWrite(latchPin, HIGH);
}

// -----------------------------------------------------------------------------
// Only do the action once, don't repeat if the button is held down.
// Don't repeat action if the button is not pressed.
boolean stopButtonState = true;

void printByte(byte b) {
  for (int i = 7; i >= 0; i--)
    Serial.print(bitRead(b, i));
}

void buttonCheck() {
  byte dataByte = B10000000;
  for (int i = 0; i < 8; i++) {
    printByte(dataByte);
    dataByte = dataByte >> 1;
    // digitalWrite(latchPin, LOW);
    // shiftOut(dataPin, clockPin, MSBFIRST, numberToDisplay);
    // digitalWrite(latchPin, HIGH);
    delay(60);
  }
}

void checkButton() {
  if (digitalRead(dataInputPin) == HIGH) {
    if (!stopButtonState) {
      Serial.println(F("+ Button pressed."));
      //
      // Action code.
      buttonCheck();
      //
      stopButtonState = false;
    }
    stopButtonState = true;
  } else {
    if (stopButtonState) {
      stopButtonState = false;
    }
  }
}

// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(1000);        // Give the serial connection time to start before the first print.
  Serial.println(""); // Newline after garbage characters.
  Serial.println("+++ Setup.");

  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(dataInputPin, INPUT);
  delay(300);
  Serial.println("+ Ready for input.");

  // updateShiftRegister(B11111111);
  byte dataByte = B11111111;
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, dataByte);
  digitalWrite(latchPin, HIGH);
  Serial.println("+ All shift register pins set high.");

  Serial.println("+++ Start program loop.");
}

// -----------------------------------------------------------------------------
// Device Loop.

void loop() {
  // Serial.println("+ Looping");
  checkButton();
  delay(60);
}
// -----------------------------------------------------------------------------
