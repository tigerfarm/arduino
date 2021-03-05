// -----------------------------------------------------------------------------
/*
  Test infrared receiver codes for:
  + Toshiba VCR remote
  + Small remote

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
    case 0xFF10EF:
    case 0xE0E0A659:                        // Toshiba VCR remote
      Serial.println("+ Key < - previous");
      break;
    case 0xFF5AA5:
    case 0xE0E046B9:                        // Toshiba VCR remote
      Serial.println("+ Key > - next");
      break;
    case 0xFF18E7:
    case 0xE0E006F9:                        // Toshiba VCR remote
      Serial.println("+ Key up");
      break;
    case 0xFF4AB5:
    case 0xE0E08679:                        // Toshiba VCR remote
      Serial.println("+ Key down");
      break;
    case 0xFF38C7:
    case 0xE0E016E9:                        // Toshiba VCR remote
      Serial.println("+ Key OK - Toggle");
      break;
    // -----------------------------------
    case 0x1163EEDF:                        // Toshiba VCR remote
    case 0x20F63C23:                        // After pressing TV
    case 0xFF9867:                          // Small remote
      Serial.print("+ Key 0:");
      Serial.println("");
      break;
    case 0x718E3D1B:                        // Toshiba VCR remote
    case 0xB16A8E1F:                        // After pressing TV
    case 0xFFA25D:                          // Small remote
      Serial.print("+ Key 1: ");
      Serial.println("");
      break;
    case 0xF8FB71FB:                        // Toshiba VCR remote
    case 0x38D7C2FF:                        // After pressing TV
    case 0xFF629D:
      Serial.print("+ Key 2: ");
      Serial.println("");
      break;
    case 0xE9E0AC7F:                        // Toshiba VCR remote
    case 0x29BCFD83:                        // After pressing TV
    case 0xFFE21D:
      Serial.print("+ Key 3: ");
      Serial.println("");
      break;
    case 0x38BF129B:                        // Toshiba VCR remote
    case 0x789B639F:                        // After pressing TV
    case 0xFF22DD:
      Serial.print("+ Key 4: ");
      Serial.println("");
      break;
    case 0x926C6A9F:                        // Toshiba VCR remote
    case 0xD248BBA3:                        // After pressing TV
    case 0xFF02FD:
      Serial.print("+ Key 5: ");
      Serial.println("");
      break;
    case 0xE66C5C37:                        // Toshiba VCR remote
    case 0x2648AD3B:                        // After pressing TV
    case 0xFFC23D:
      Serial.print("+ Key 6: ");
      Serial.println("");
      break;
    case 0xD75196BB:                        // Toshiba VCR remote
    case 0x172DE7BF:                        // After pressing TV
    case 0xFFE01F:
      Serial.print("+ Key 7: ");
      Serial.println("");
      break;
    case 0x72FD3AFB:                        // Toshiba VCR remote
    case 0xB2D98BFF:                        // After pressing TV
    case 0xFFA857:
      Serial.print("+ Key 8: ");
      Serial.println("");
      break;
    case 0xCCAA92FF:                        // Toshiba VCR remote
    case 0xC86E403:                         // After pressing TV
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
    // Toshiba VCR remote
    case 0xDA529B37:                        // After pressing VCR
    case 0x1A2EEC3B:                        // After pressing TV
      Serial.println("+ Key POWER");
      break;
    case 0x3694275F:                        // After pressing VCR
    case 0x5C6068E3:                        // After pressing TV
      Serial.println("+ Key INPUT/SELECT");
      break;
    case 0x6D8BBC17:                        // After pressing VCR
    case 0xAD680D1B:                        // After pressing TV
      Serial.println("+ Key CHANNEL UP");
      break;
    case 0xCDFC965B:                        // After pressing VCR
    case 0xDD8E75F:                         // After pressing TV
      Serial.println("+ Key CHANNEL DOWN");
      break;
    case 0x2B8BE5F:
      Serial.println("+ Key VOLUME UP");
      break;
    case 0x1CF3ACDB:
      Serial.println("+ Key VOLUME DOWN");
      break;
    case 0xC4CC6436:                        // After pressing VCR
    case 0x6F46633F:                        // After pressing TV
      Serial.println("+ Key DISPLAY");
      break;
    case 0xBC133D9B:                        // Toshiba VCR remote
    case 0xDD8E75F:                         // After pressing TV
      Serial.println("+ Key 100");
      break;
    case 0xA7DCBAFB:
      Serial.println("+ Key COUNTER");
      break;
    case 0x953EEEBC:
      Serial.println("+ Key CLEAR");
      break;
    case 0x85CF699F:
      Serial.println("+ Key TV/VCR");
      break;
    case 0xD79EEBC3:
      Serial.println("+ Key SLOW");
      break;
    case 0x2C22119B:
      Serial.println("+ Key PAUSE/STILL");
      break;
    case 0x7E23117B:
      Serial.println("+ Key REW");
      break;
    case 0x7538143B:
      Serial.println("+ Key FF");
      break;
    case 0x8AA3C35B:
      Serial.println("+ Key PLAY");
      break;
    case 0xFA2F715F:
      Serial.println("+ Key STOP");
      break;
    case 0xC473DE3A:
      Serial.println("+ Key EJECT");
      break;
    case 0xA02E4EBF:
      Serial.println("+ Key A.SELECT");
      break;
    case 0x9AAF8DFF:
      Serial.println("+ Key PROG");
      break;
    case 0x82D6EC17:
      Serial.println("+ Key ENTER");
      break;
      
    // -----------------------------------
    default:
      Serial.print("+ Result value: 0x");
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
