// -----------------------------------------------------------------------------
/*
  Connect a KY-040 rotary encoder to a Nano:
  Encoder         Nano
  + "+"           +5v. Also works with 3.3v, example: NodeMCU.
  + GND           Ground.
  + CLK (clock)   Pin 2, the interrupt pin. Also referred to as output A (encoder pin A).
  + DT (data)     Pin 3. Also referred to as output B (encoder pin B).
  + SW (switch)   Pin 4. SW is a push dial switch. Can be used to reset the counter to zero.

  More info and more elegant code and hardware options:
    https://forum.arduino.cc/index.php?topic=242356.0
    This link suggests using a resister and capacitor for debouncing, which I'm not using.
  Connect CLK to Pin 2 on Arduino Board  (CLK is Data Output 1 of KY-040)
  Connect DT  to Pin 3 on Arduino Board  (DT is Data Output 2 of KY-040)
  Connect SW  to to Pin 4 on Arduino Board (Switch - goes LOW when pressed)
  Connect GND to ground
  Connect +5V  (this will pull up CLK and DT with 10 KiloOhm resistors)
  Connect a 10K resistor from +5V to SW (no integrated pullup for SW !!)
*/
// -----------------------------------------------------------------------------
// Rotary Encoder module connections
const int PinCLK = 2; // Generating interrupts using CLK signal
const int PinDT = 3;  // Reading DT signal
const int PinSW = 4;  // Reading Switch signal

// -----------------------------------------------------------------------------
// Interrupt routine runs if rotary encoder CLK pin changes state.

volatile boolean TurnDetected;  // Type volatile for interrupts.
volatile boolean turnRight;

void rotarydetect ()  {
  // Set TurnDetected, and in which direction: turnRight or left (!turnRight).
  TurnDetected = false;
  if (digitalRead(PinDT) == 1) {
    TurnDetected = true;
    turnRight = false;
    if (digitalRead(PinCLK) == 0) {
      turnRight = true;
    }
  }
}

void setup ()  {
  // Speed for serial read, which matches the sending program.
  Serial.begin(115200);         // Baud rates: 9600 19200 57600 115200
  delay(2000);
  Serial.println(); // Newline after garbage characters.
  Serial.println("+++ Setup.");
  
  // ----------------------------------------------------
  pinMode(PinCLK, INPUT);
  pinMode(PinDT, INPUT);
  pinMode(PinSW, INPUT);
  attachInterrupt (0, rotarydetect, CHANGE); // Interrupt 0 is pin 2 on Arduino.

  // ----------------------------------------------------
  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop

boolean switchPressed = false;
void loop ()  {
  static long virtualPosition = 0;  // static is required to count correctly.

  if (TurnDetected)  {
    // Rotory turning.
    TurnDetected = false;  // Reset, until new rotation detected
    if (turnRight) {
      virtualPosition++;
      Serial.print (" > right count = ");
    } else {
      virtualPosition--;
      Serial.print (" < left  count = ");
    }
    Serial.println (virtualPosition);
    delay(30);
  }

  if (!(digitalRead(PinSW))) {
    // Switched pressed.
    if (switchPressed == false) {
      virtualPosition = 0;                  // if YES, then reset counter to ZERO
      Serial.print ("+ Switch, reset = ");  // Using the word RESET instead of COUNT here to find out a buggy encoder
      Serial.println (virtualPosition);
    }
    switchPressed = true;
    delay(200);
  } else {
    switchPressed = false;
  }

  delay(30);
}
// -----------------------------------------------------------------------------
