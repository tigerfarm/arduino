// -----------------------------------------------------------------------------
/*
  Test infrared receiver.

  + Post messages to the serial port, which can be displayed using the Arduino Tools/Serial Monitor.

  Connect infrared receiver, pins from top left to right:
    Left most (next to the X) - Nano pin A1
    Center - 5V
    Right  - ground

   A1 + -   - Nano pin connections
    | | |   - Infrared receiver pins
  ---------
  |S      |
  |       |
  |  ---  |
  |  | |  |
  |  ---  |
  |       |
  ---------

  ------------------------------------------------------------------------------
  Install library: IRremote by Shirriff, I installed version: 2.2.3.
  Note, I have stayed with 2.2.3 because the syntax matches the Due library (IRremote2).

  GitHub:
    https://github.com/Arduino-IRremote/Arduino-IRremote
  Arduino info:
    https://www.arduino.cc/reference/en/libraries/irremote/
  For reference, converting a program from 2x to 3x.
    https://github.com/Arduino-IRremote/Arduino-IRremote#converting-your-2x-program-to-the-3x-version
*/
// -----------------------------------------------------------------------------
// For the infrared receiver.
#include <IRremote.h>
int IR_PIN = A1;
IRrecv irrecv(IR_PIN);
decode_results results;

void infraredSwitch() {
  // Serial.println("+ infraredSwitch");
  switch (results.value) {
    case 0xFFFFFFFF:
      // Ignore. This is from holding the key down.
      break;
    // -----------------------------------
    case 0xFF10EF:
    case 0xE0E0A659:
      Serial.println("+ Key < - previous");
      break;
    case 0xFF5AA5:
    case 0xE0E046B9:
      Serial.println("+ Key > - next");
      break;
    case 0xFF18E7:
    case 0xE0E006F9:
      Serial.println("+ Key up");
      break;
    case 0xFF4AB5:
    case 0xE0E08679:
      Serial.println("+ Key down");
      break;
    case 0xFF38C7:
    case 0xE0E016E9:
      Serial.println("+ Key OK - Toggle");
      break;
    // -----------------------------------
    case 0xFF9867:
    case 0xE0E08877:
      Serial.print("+ Key 0:");
      Serial.println("");
      break;
    case 0xFFA25D:
      Serial.print("+ Key 1: ");
      Serial.println("");
      break;
    case 0xFF629D:
      Serial.print("+ Key 2: ");
      Serial.println("");
      break;
    case 0xFFE21D:
      Serial.print("+ Key 3: ");
      Serial.println("");
      break;
    case 0xFF22DD:
      Serial.print("+ Key 4: ");
      Serial.println("");
      break;
    case 0xFF02FD:
      Serial.print("+ Key 5: ");
      Serial.println("");
      break;
    case 0xFFC23D:
      Serial.print("+ Key 6: ");
      Serial.println("");
      break;
    case 0xFFE01F:
      Serial.print("+ Key 7: ");
      Serial.println("");
      break;
    case 0xFFA857:
      Serial.print("+ Key 8: ");
      Serial.println("");
      break;
    case 0xFF906F:
      Serial.print("+ Key 9: ");
      Serial.println("");
      break;
    // -----------------------------------
    case 0xFF6897:
    case 0xE0E01AE5:
      Serial.println("+ Key * (Return)");
      break;
    case 0xFFB04F:
    case 0xE0E0B44B:
      Serial.println("+ Key # (Exit)");
      break;
    // -----------------------------------
    default:
      Serial.print("+ Result value: ");
      Serial.println(results.value, HEX);
      // -----------------------------------
  } // end switch

}

// -----------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  // Give the serial connection time to start before the first print.
  delay(1000);
  Serial.println(""); // Newline after garbage characters.
  Serial.println("+++ Setup.");

  irrecv.enableIRIn();
  Serial.println("+ Initialized the infrared receiver.");

  Serial.println("++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop

void loop() {
  delay(60);
  // ---------------------------------------------------------------------
  // Process infrared key presses.
  if (irrecv.decode(&results)) {
    infraredSwitch();
    irrecv.resume();
  }
}
// -----------------------------------------------------------------------------
