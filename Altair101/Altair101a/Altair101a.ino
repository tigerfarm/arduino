// -----------------------------------------------------------------------------
/*
  Serial Communications:
*/

#include "Altair101a.h"
#include "host_mega.h"
#include "mem.h"

// -----------------------------------------------------------------------------
uint16_t host_read_status_leds()
{
  uint16_t res = PORTB;
  res |= PORTD & 0x80 ? ST_INTE : 0;
  res |= PORTG & 0x04 ? ST_PROT : 0;
  res |= PORTG & 0x02 ? ST_WAIT : 0;
  res |= PORTG & 0x01 ? ST_HLDA : 0;
  return res;
}

// -----------------------------------------------------------------------------
void print_panel_serial(bool force = false);

uint16_t cswitch = 0;
uint16_t dswitch = 0;

void print_panel_serial(bool force)
{
  byte dbus;
  static uint16_t p_dswitch = 0, p_cswitch = 0, p_abus = 0xffff, p_dbus = 0xffff, p_status = 0xffff;
  uint16_t status, abus;

  status = host_read_status_leds();
  abus   = host_read_addr_leds();
  dbus   = host_read_data_leds();

  if ( force || p_cswitch != cswitch || p_dswitch != dswitch || p_abus != abus || p_dbus != dbus || p_status != status )
  {
    Serial.print(F("INTE PROT MEMR INP M1 OUT HLTA STACK WO INT  D7  D6  D5  D4  D3  D2  D1  D0\r\n"));

    if ( status & ST_INTE  ) Serial.print(F(" *  "));    else Serial.print(F(" .  "));
    if ( status & ST_PROT  ) Serial.print(F("  *  "));   else Serial.print(F("  .  "));
    if ( status & ST_MEMR  ) Serial.print(F("  *  "));   else Serial.print(F("  .  "));
    if ( status & ST_INP   ) Serial.print(F("  * "));    else Serial.print(F("  . "));
    if ( status & ST_M1    ) Serial.print(F(" * "));     else Serial.print(F(" . "));
    if ( status & ST_OUT   ) Serial.print(F("  * "));    else Serial.print(F("  . "));
    if ( status & ST_HLTA  ) Serial.print(F("  *  "));   else Serial.print(F("  .  "));
    if ( status & ST_STACK ) Serial.print(F("   *  "));  else Serial.print(F("   .  "));
    if ( status & ST_WO    ) Serial.print(F(" * "));     else Serial.print(F(" . "));
    if ( status & ST_INT   ) Serial.print(F("  *"));    else Serial.print(F("  ."));

    if ( dbus & 0x80 )   Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( dbus & 0x40 )   Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( dbus & 0x20 )   Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( dbus & 0x10 )   Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( dbus & 0x08 )   Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( dbus & 0x04 )   Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( dbus & 0x02 )   Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( dbus & 0x01 )   Serial.print(F("   *")); else Serial.print(F("   ."));
    Serial.print(("\r\nWAIT HLDA   A15 A14 A13 A12 A11 A10  A9  A8  A7  A6  A5  A4  A3  A2  A1  A0\r\n"));
    if ( status & ST_WAIT ) Serial.print(F(" *  "));   else Serial.print(F(" .  "));
    if ( status & ST_HLDA ) Serial.print(F("  *   ")); else Serial.print(F("  .   "));
    if ( abus & 0x8000 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( abus & 0x4000 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( abus & 0x2000 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( abus & 0x1000 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( abus & 0x0800 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( abus & 0x0400 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( abus & 0x0200 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( abus & 0x0100 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( abus & 0x0080 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( abus & 0x0040 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( abus & 0x0020 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( abus & 0x0010 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( abus & 0x0008 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( abus & 0x0004 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( abus & 0x0002 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( abus & 0x0001 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    Serial.print(F("\r\n            S15 S14 S13 S12 S11 S10  S9  S8  S7  S6  S5  S4  S3  S2  S1  S0\r\n"));
    Serial.print(F("          "));
    if ( dswitch & 0x8000 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    if ( dswitch & 0x4000 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    if ( dswitch & 0x2000 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    if ( dswitch & 0x1000 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    if ( dswitch & 0x0800 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    if ( dswitch & 0x0400 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    if ( dswitch & 0x0200 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    if ( dswitch & 0x0100 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    if ( dswitch & 0x0080 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    if ( dswitch & 0x0040 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    if ( dswitch & 0x0020 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    if ( dswitch & 0x0010 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    if ( dswitch & 0x0008 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    if ( dswitch & 0x0004 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    if ( dswitch & 0x0002 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    if ( dswitch & 0x0001 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    Serial.print(F("\r\n ------ \r\n"));
    p_cswitch = cswitch;
    p_dswitch = dswitch;
    p_abus = abus;
    p_dbus = dbus;
    p_status = status;
  }
}

// -----------------------------------------------------------------------------
// Memory definitions

const int memoryBytes = 1024;  // When using Mega: 1024, for Nano: 256
byte memoryData[memoryBytes];
unsigned int programCounter = 0;     // Program address value

// -----------------------------------------------------------------------------
void printByte(byte b) {
  for (int i = 7; i >= 0; i--)
    Serial.print(bitRead(b, i));
}

void printOctal(byte b) {
  String sOctal = String(b, OCT);
  for (int i = 1; i <= 3 - sOctal.length(); i++) {
    Serial.print("0");
  }
  Serial.print(sOctal);
}

// -----------------------------------------------------------------------------
void setup() {

  // Speed for serial read, which matches the sending program.
  Serial.begin(115200);   // 115200
  delay(1000);
  Serial.println(""); // Newline after garbage characters.
  Serial.println("+++ Setup.");
  Serial.println("+ Ready for serial communications.");

  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop
byte readByte = 0;
int readByteCount = 0;
void loop() {

  if (Serial.available() > 0) {
    // Read and process an incoming byte.
    readByte = Serial.read();
    memoryData[readByteCount];
    readByteCount++;
    //
    switch (readByte) {
      case 'r':
        Serial.println("+ r: RUN.");
        print_panel_serial();
        break;
      case '1':
        Serial.println("+ 1: Toggle data switch 1.");
        print_panel_serial();
        break;
      case '2':
        Serial.println("+ 2: Toggle data switch 2.");
        print_panel_serial();
        break;
      case '3':
        Serial.println("+ 3: Toggle data switch 3.");
        print_panel_serial();
        break;
      // -------------------------------------
      case 10:
        // Ignore new line character.
        // Serial.println("");
        break;
      // -------------------------------------
      default:
        Serial.print("- Invalid character ignored: ");
        Serial.write(readByte);
        Serial.println("");
    }
    /*  When displaying only binary data.
      Serial.print("++ Byte: ");
      printByte(readByte);
      Serial.print(" = ");
      printOctal(readByte);
      Serial.print(" = ");
      Serial.print(readByte, DEC);
      Serial.print(", Character: ");
      Serial.write(readByte);
    */
  }
  // delay(30); // Arduino sample code, doesn't use a delay.
}
// -----------------------------------------------------------------------------
