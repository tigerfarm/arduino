// -----------------------------------------------------------------------------
/*
  Test infrared receiver.

  + Post messages to the serial port, which can be displayed using the Arduino Tools/Serial Monitor.

  Connect infrared receiver, pins from top left to right:
    Left most (next to the S) - Arduino pin A1 (or using another analog or digital pin)
    Center - VCC
    Right  - GND (ground)

   A1 + -   - Arduino pin connections
    | | |   - Infrared receiver pins
  ---------
  |S     -|
  |       |
  |  ---  |
  |  | |  |
  |  ---  |
  |       |
  ---------
    Front

  ------------------------------------------------------------------------------
  Install library for Due.
  Project:
    https://github.com/enternoescape/Arduino-IRremote-Due/
    Code dropdown has as Download Zip option.
  On my Mac,
    Move the unzipped files directory, into the directory: Users/.../Documents/Arduino/libraries.
    Unzip the file.
*/
// -----------------------------------------------------------------------------
// For the infrared receiver.

// This is the only difference when using the non-Due infrared library.
// The non-Due infrared library is: #include <IRremote1.h>
#include <IRremote2.h>

// Other pins can be used. I've use both analog and digital pins.
// int IR_PIN = A1;
int IR_PIN = 24;

IRrecv irrecv(IR_PIN);
decode_results results;

void infraredSwitch() {
  // Serial.println("+ infraredSwitch");
  switch (results.value) {
    case 0xFFFFFFFF:
      // Ignore. This is from holding the key down.
      break;
    // -----------------------------------
    case 0x7E23117B:
    case 0xFF10EF:
      Serial.println("+ Key < - previous");
      break;
    case 0x7538143B:
    case 0xFF5AA5:
      Serial.println("+ Key > - next");
      break;
    case 0x8AA3C35B:
    case 0xFF18E7:
      Serial.println("+ Key up");
      break;
    case 0xFA2F715F:
    case 0xFF4AB5:
      Serial.println("+ Key down");
      break;
    case 0x82D6EC17:
    case 0xFF38C7:
      Serial.println("+ Key OK|Enter - Toggle");
      break;
    case 0x2C22119B:
      Serial.println("+ Pause");
      break;
    // -----------------------------------
    case 0x1163EEDF:
    case 0xFF9867:
      Serial.print("+ Key 0:");
      Serial.println("");
      break;
    case 0x718E3D1B:
    case 0xFFA25D:
      Serial.print("+ Key 1: ");
      Serial.println("");
      break;
    case 0xF8FB71FB:
    case 0xFF629D:
      Serial.print("+ Key 2: ");
      Serial.println("");
      break;
    case 0xE9E0AC7F:
    case 0xFFE21D:
      Serial.print("+ Key 3: ");
      Serial.println("");
      break;
    case 0x38BF129B:
    case 0xFF22DD:
      Serial.print("+ Key 4: ");
      Serial.println("");
      break;
    case 0x926C6A9F:
    case 0xFF02FD:
      Serial.print("+ Key 5: ");
      Serial.println("");
      break;
    case 0xE66C5C37:
    case 0xFFC23D:
      Serial.print("+ Key 6: ");
      Serial.println("");
      break;
    case 0xD75196BB:
    case 0xFFE01F:
      Serial.print("+ Key 7: ");
      Serial.println("");
      break;
    case 0x72FD3AFB:
    case 0xFFA857:
      Serial.print("+ Key 8: ");
      Serial.println("");
      break;
    case 0xCCAA92FF:
    case 0xFF906F:
      Serial.print("+ Key 9: ");
      Serial.println("");
      break;
    // -----------------------------------
    case 0xFF6897:
      Serial.println("+ Key * (Return)");
      break;
    case 0xFFB04F:
      Serial.println("+ Key # (Exit)");
      break;
    // -----------------------------------
    case 0x6D8BBC17:
      Serial.println("+ Key Channel ^");
      break;
    case 0xCDFC965B:
      Serial.println("+ Key Channel v");
      break;
    case 0x1CF3ACDB:
      Serial.println("+ Key Volume ^");
      break;
    case 0x2B8BE5F:
      Serial.println("+ Key Volume v");
      break;
    // -----------------------------------
    case 0xDA529B37:
      Serial.println("+ Power");
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
