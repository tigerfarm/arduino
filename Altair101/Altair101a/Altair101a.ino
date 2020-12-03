// -----------------------------------------------------------------------------
/*
  Altair 101a Processor program

  + Serial interactivity
  ++ Using the Arduino IDE monitor USB serial port.
  ++ Or VT100 terminal.
  + Uses David Hansel's Altair 8800 Simulator code to process machine code instructions.
  ++ cpucore_i8080.cpp and cpucore_i8080.h.

  Differences to the origanal:
  + HLT goes into STOP state which allows RUN to restart the process from where it halted.
  + When SINGLE STEP read and write, I've chosen to show the actual data value,
    rather than having all the data lights on, which is what the original Altair 8800 does.

  ---------------------------------------------------------
  Next:

  More testing for: singleStepWait().

  +++ Integration steps to merge this code with Processor.ino.

  Continue testing Altair101a.
  Then upload to the Altair 101 machine and test with lights.
  Then merge with Processor.ino code.

  ---------------------------------------------------------
  Remove processor functions and variables from Processor.ino.
  + Leave player, clock, and counter functions intact.
  + Keep hardware switching intact, which is not inmplemented in this program.

  ---------------------------------------------------------
  Other Nexts:

  + When single stepping, M1 stays on but should be off, when HLT is executed.
  + Should be on: MEMR, HLTA, WO.
  + On the Altair 101, only HLTA light is on.

  + In RUN mode, consider having all data lights on, same as the Altair 8800.

  + Prevent lockup when using PUSH A, i.e. PUSH called before setting SP.
  ++ In the PUSH process, if SP < 4, error.

  For efficiency, I could replace the following:
    host_read_status_led_*();
    host_set_status_led_*();
    host_clr_status_led_*();
  by using global varialbes, same as used in Processor.ino. For example,
    statusByte = MEMR_ON | MI_ON | WO_ON;
    statusByte &= MI_OFF;
  But the above is not required.
  The host_set and host_clr makes sense in the other hardware configuration
    because each status light has pin to turn it on and off. Not the case in Altair 101.

  ---------------------------------------------------------
    VT100 reference:
       http://ascii-table.com/ansi-escape-sequences-vt-100.php
    Esc[H  Move cursor to upper left corner, example: Serial.print("\033[H");
    Esc[J  Clear screen from cursor down, example: Serial.print("\033[J");
    Esc[2J  Clear entire screen, example: Serial.print("\033[H");
    Example: Serial.print("\033[H\033[2J");  Move home and clear entire screen.
    Esc[nA  Move cursor up n lines.
    Example: Serial.print("\033[3A");  Cursor Up 3 lines.
    Esc[nB  Move cursor down n lines.
    Example: Serial.print("\033[6B");  Cursor down 6 lines.
    Esc[K  Clear line from cursor right

  ---------------------------------------------------------
*/
// -----------------------------------------------------------------------------
#include "Altair101a.h"
#include "cpucore_i8080.h"

#define LOG_MESSAGES 1    // For debugging.
// #define LOG_OPCODES  1    // Print each called opcode.

byte opcode = 0xff;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
char charBuffer[17];

void printByte(byte b) {
  for (int i = 7; i >= 0; i--)
    Serial.print(bitRead(b, i));
}
void printOctal(byte b) {
  String sValue = String(b, OCT);
  for (int i = 1; i <= 3 - sValue.length(); i++) {
    Serial.print("0");
  }
  Serial.print(sValue);
}
void printData(byte theByte) {
  sprintf(charBuffer, "%3d = ", theByte);
  Serial.print(charBuffer);
  printOctal(theByte);
  Serial.print(F(" = "));
  printByte(theByte);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Front Panel declarations

int programState = LIGHTS_OFF;  // Intial, default.

void ledFlashSuccess() {}

// ------------------------------
// Status Indicator LED lights

byte readByte = 0;

// Program wait status.
const int WAIT_PIN = A9;      // Processor program wait state: off/LOW or wait state on/HIGH.

// HLDA : 8080 processor goes into a hold state because of other hardware running.
const int HLDA_PIN = A10;     // Emulator processing (off/LOW) or clock/player processing (on/HIGH).

// Use OR to turn ON. Example:
const byte MEMR_ON =    B10000000;  // MEMR   The memory bus will be used for memory read data.
const byte INP_ON =     B01000000;  // INP    The address bus containing the address of an input device. The input data should be placed on the data bus when the data bus is in the input mode
const byte M1_ON =      B00100000;  // M1     Machine cycle 1, fetch opcode.
const byte OUT_ON =     B00010000;  // OUT    The address contains the address of an output device and the data bus will contain the out- put data when the CPU is ready.
const byte HLTA_ON =    B00001000;  // HLTA   Machine opcode hlt, has halted the machine.
const byte STACK_ON =   B00000100;  // STACK  Stack process
const byte WO_ON =      B00000010;  // WO     Write out (inverse logic)
const byte INT_ON =     B00000001;  // INT    Interrupt

// -----------------------------------------------------------------------------
// Front Panel Status LEDs

// Output LED lights shift register(SN74HC595N) pins
//          Mega/Nano pins       74HC595 Pins
const int dataPinLed  = 5;    // pin ? Data pin.
const int latchPinLed = 6;    // pin ? Latch pin.
const int clockPinLed = 7;    // pin ? Clock pin.

// -------------------------------------------------
void lightsStatusAddressData( byte status8bits, unsigned int address16bits, byte data8bits) {
  digitalWrite(latchPinLed, LOW);
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, status8bits);
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, data8bits);
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, lowByte(address16bits));
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, highByte(address16bits));
  digitalWrite(latchPinLed, HIGH);
  //
#ifdef LOG_MESSAGES
  Serial.print(F("+ lightsStatusAddressData, status:"));
  printByte(status8bits);
  Serial.print(" address:");
  sprintf(charBuffer, "%5d", address16bits);
  Serial.print(charBuffer);
  Serial.print(" dataByte:");
  printByte(data8bits);
  Serial.println();
#endif
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Output Front Panel Information.

// Types of output:
boolean LED_IO = false;
boolean SERIAL_IO_VT100 = false;
boolean SERIAL_IO = true;

void printFrontPanel() {
  if (LED_IO) {
    lightsStatusAddressData(fpStatusByte, host_read_addr_leds(), host_read_data_leds());
  }
  if (SERIAL_IO_VT100) {
    serialPrintFrontPanel();
#ifdef LOG_MESSAGES
    // Serial.print("\033[2K"); // Clear line from cursor right
    Serial.print(F("+ printFrontPanel SERIAL_IO_VT100, status:"));
    printByte(fpStatusByte);
    //
    uint16_t theAddress = host_read_addr_leds();
    Serial.print(" address:");
    sprintf(charBuffer, "%5d", theAddress);
    Serial.print(charBuffer);
    Serial.print("=");
    printByte(highByte(theAddress));
    Serial.print(":");
    printByte(lowByte(theAddress));
    //
    Serial.print(" dataByte:");
    printByte(host_read_data_leds());
    Serial.println();
    // Serial.print("\033[2K"); // Clear line
#endif
  } else {
    if (SERIAL_IO) {
      if (host_read_status_led_WAIT()) {
        serialPrintFrontPanel();
      } else {
        Serial.print(F("+ printFrontPanel SERIAL_IO, status:"));
        printByte(fpStatusByte);
        Serial.print(" address:");
        sprintf(charBuffer, "%5d", host_read_addr_leds());
        Serial.print(charBuffer);
        Serial.print(" dataByte:");
        printByte(host_read_data_leds());
        Serial.println();
      }
    }
  }
}

// -----------------------------------------------------------------------------
// Print the front panel to either the Arduino IDE monitor, or to the VT00 terminal.
// In a VT00 terminal, the front panel stays in the top left of the terminal screen.
// In the Arduino IDE monitor, the front panel is reprinted and scrolls.

// byte prev_fpStatusByte = 250;     // This will take more time to figure out.
byte prev_fpDataByte = 1;
uint16_t prev_fpAddressWord = 0;
uint16_t fpAddressToggleWord = 0;
uint16_t prev_fpAddressToggleWord = 1;

void serialPrintFrontPanel() {
  //
  // --------------------------
  // Status
  if (!SERIAL_IO_VT100) {
    Serial.print(F("INTE MEMR INP M1 OUT HLTA STACK WO INT        D7  D6   D5  D4  D3   D2  D1  D0\r\n"));
    // Note, PROT is not implemented.
  } else {
    // No need to rewrite the title.
    Serial.print("\033[H");   // Move cursor home
    Serial.print("\033[1B");  // Cursor down
  }
  if ( host_read_status_led_INTE() ) Serial.print(F(" *  "));  else Serial.print(F(" .  "));
  if ( fpStatusByte & MEMR_ON  )     Serial.print(F("  * ")); else Serial.print(F("  . "));
  if ( fpStatusByte & INP_ON   )     Serial.print(F("   * ")); else Serial.print(F("   . "));
  if ( fpStatusByte & M1_ON    )     Serial.print(F(" * "));   else Serial.print(F(" . "));
  if ( fpStatusByte & OUT_ON   )     Serial.print(F("  * "));  else Serial.print(F("  . "));
  if ( fpStatusByte & HLTA_ON  )     Serial.print(F("  * "));  else Serial.print(F("  . "));
  if ( fpStatusByte & STACK_ON )     Serial.print(F("   * ")); else Serial.print(F("   . "));
  if ( fpStatusByte & WO_ON    )     Serial.print(F("   * ")); else Serial.print(F("   . "));
  if ( fpStatusByte & INT_ON   )     Serial.print(F("  *"));   else Serial.print(F("  ."));
  Serial.print(F("       "));
  //
  // --------------------------
  // Data
  if ((prev_fpDataByte != fpDataByte) || (!SERIAL_IO_VT100)) {
    // If VT100 and no change, don't reprint.
    prev_fpDataByte = fpDataByte;
    if ( fpDataByte & 0x80 )   Serial.print(F("  *" )); else Serial.print(F("  ." ));
    if ( fpDataByte & 0x40 )   Serial.print(F("   *")); else Serial.print(F("   ."));
    Serial.print(F(" "));
    if ( fpDataByte & 0x20 )   Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( fpDataByte & 0x10 )   Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( fpDataByte & 0x08 )   Serial.print(F("   *")); else Serial.print(F("   ."));
    Serial.print(F(" "));
    if ( fpDataByte & 0x04 )   Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( fpDataByte & 0x02 )   Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( fpDataByte & 0x01 )   Serial.print(F("   *")); else Serial.print(F("   ."));
  }
  // --------------------------
  // WAIT, HLDA, and Address
  //
  if (SERIAL_IO_VT100) {
    // No need to rewrite the title.
    Serial.println();
    Serial.print("\033[1B");  // Cursor down
  } else {
    Serial.print(("\r\nWAIT HLDA   A15 A14 A13 A12 A11 A10  A9  A8   A7  A6   A5  A4  A3   A2  A1  A0"));
    Serial.print(("\r\n"));
  }
  // WAIT and HLDA
  if ( host_read_status_led_WAIT() ) Serial.print(F(" *  "));   else Serial.print(F(" .  "));
  if ( host_read_status_led_HLDA() ) Serial.print(F("  *   ")); else Serial.print(F("  .   "));
  //
  // Address
  if ((prev_fpAddressWord != fpAddressWord) || (!SERIAL_IO_VT100)) {
    // If VT100 and no change, don't reprint.
    prev_fpAddressWord = fpAddressWord;
    if ( fpAddressWord & 0x8000 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( fpAddressWord & 0x4000 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( fpAddressWord & 0x2000 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( fpAddressWord & 0x1000 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( fpAddressWord & 0x0800 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( fpAddressWord & 0x0400 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( fpAddressWord & 0x0200 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( fpAddressWord & 0x0100 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    Serial.print(F(" "));
    if ( fpAddressWord & 0x0080 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( fpAddressWord & 0x0040 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    Serial.print(F(" "));
    if ( fpAddressWord & 0x0020 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( fpAddressWord & 0x0010 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( fpAddressWord & 0x0008 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    Serial.print(F(" "));
    if ( fpAddressWord & 0x0004 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( fpAddressWord & 0x0002 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( fpAddressWord & 0x0001 ) Serial.print(F("   *")); else Serial.print(F("   ."));
  }
  // --------------------------
  // Address/Data switches
  if (SERIAL_IO_VT100) {
    // No need to rewrite the title.
    Serial.println();
    Serial.print("\033[1B");  // Cursor down
  } else {
    Serial.print(F("\r\n            S15 S14 S13 S12 S11 S10  S9  S8   S7  S6   S5  S4  S3   S2  S1  S0\r\n"));
  }
  if ((prev_fpAddressToggleWord != fpAddressToggleWord) || (!SERIAL_IO_VT100)) {
    // If VT100 and no change, don't reprint.
    prev_fpAddressToggleWord = fpAddressToggleWord;
    Serial.print(F("          "));
    if ( fpAddressToggleWord & 0x8000 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    if ( fpAddressToggleWord & 0x4000 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    if ( fpAddressToggleWord & 0x2000 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    if ( fpAddressToggleWord & 0x1000 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    if ( fpAddressToggleWord & 0x0800 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    if ( fpAddressToggleWord & 0x0400 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    if ( fpAddressToggleWord & 0x0200 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    if ( fpAddressToggleWord & 0x0100 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    Serial.print(F(" "));
    if ( fpAddressToggleWord & 0x0080 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    if ( fpAddressToggleWord & 0x0040 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    Serial.print(F(" "));
    if ( fpAddressToggleWord & 0x0020 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    if ( fpAddressToggleWord & 0x0010 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    if ( fpAddressToggleWord & 0x0008 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    Serial.print(F(" "));
    if ( fpAddressToggleWord & 0x0004 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    if ( fpAddressToggleWord & 0x0002 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    if ( fpAddressToggleWord & 0x0001 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
  }
  if (SERIAL_IO_VT100) {
    // No need to rewrite the prompt.
    Serial.print("\033[2B");  // Cursor down 2 lines.
    Serial.println();
  } else {
    Serial.print(F("\r\n ------ \r\n"));
    Serial.println("+ Ready to receive command.");
  }

}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Process functions

// -----------------------------------------------------------------------------
// Pause during a SINGLE STEP process.
byte singleStepWaitByte = 0;
void singleStepWait() {
  Serial.println(F("+ singleStepWait()"));
  if (singleStepWaitByte != 0) {         // Complete the instruction cycles and process the byte, example r, to RUN the program.
    return;
  }
  printFrontPanel();                // Status, data/address lights already set.
  bool singleStepWaitLoop = true;
  while (singleStepWaitLoop) {
    if (Serial.available() > 0) {
      readByte = Serial.read();    // Read and process an incoming byte.
      switch (readByte) {
        case 's':
          singleStepWaitLoop = false;
          break;
        case 'i':
        case 'p':
          Serial.print("++ readByte: ");
          Serial.write(readByte);
          processWaitSwitch(readByte);
          break;
        case 10:
          Serial.println("++ Ignore serial input line feed character.");
          break;
        default:
          singleStepWaitLoop = false;
          singleStepWaitByte = readByte;
          Serial.print("+ Complete the machine instruction and exit SINGLE STEP mode. Input byte: ");
          Serial.write(singleStepWaitByte);
          Serial.println();
      }
    }
  }
}

// -----------------------------------------------------------------------------
// IN opcode, input processing to get a byte from an input port.
/*
  Opcode: out <port#>
  Called from: cpu_IN() { regA = altair_in(MEM_READ(regPC)); ... }
    The input byte is loaded into register A.
  A byte value of 0, implies no input on that port at the time of being called.
*/

byte inputBytePort2 = 0;
byte altair_in(byte portDataByte) {
  byte inputDataByte;
  //
  // Set status lights and get an input byte from the input port.
  host_clr_status_led_MEMR();
  host_set_status_led_INP();
  //
  switch (portDataByte) {
    case B11111111:
      // From hardware Sense switches. Not implemented.
      inputDataByte = 0;
      break;
    case 2:
      // USB serial input sense switches: 8,9,a...f.
      // Input(inputBytePort2) comes from the RUN mode loop.
      if (inputBytePort2 == '8') {
        inputDataByte = B00000001;
      } else if (inputBytePort2 == '9') {
        inputDataByte = B00000010;
      } else if (inputBytePort2 == 'a') {
        inputDataByte = B00000100;
      } else if (inputBytePort2 == 'b') {
        inputDataByte = B00001000;
      } else if (inputBytePort2 == 'c') {
        inputDataByte = B00010000;
      } else if (inputBytePort2 == 'd') {
        inputDataByte = B00100000;
      } else if (inputBytePort2 == 'e') {
        inputDataByte = B01000000;
      } else if (inputBytePort2 == 'f') {
        inputDataByte = B10000000;
      } else if (inputBytePort2 == 3) {
        inputDataByte = 3;                // Ctrl+c
      }
      if (inputDataByte > 0) {
        inputBytePort2 = 0;
#ifdef LOG_MESSAGES
        Serial.print(F("+ Input sense switch byte: B"));
        printByte(inputDataByte);
        Serial.println();
#endif
      }
      break;
    case 3:
      // Input(inputBytePort2) comes from the RUN mode loop.
      // Only keep the most recent input, not queueing the inputs at this time.
      inputDataByte = inputBytePort2;
      inputBytePort2 = 0;
      break;
    case 16:
      // Test port: 20Q (octal: 020).
      // Return byte value of 2, which is used in the status light video sample program.
      inputDataByte = 2;
      break;
    default:
      inputDataByte = 0;
  }
  //
#ifdef LOG_MESSAGES
  if (inputDataByte > 0) {
    // No input at this time.
    // > Input port# 3 inputDataByte value =  3, printByte=00000011, writeInByte=''
    // Samples, Ctrl+c=3
    Serial.print(F("> Input port# "));
    Serial.print(portDataByte);
    Serial.print(" inputDataByte value = ");
    sprintf(charBuffer, "%3d", inputDataByte);
    Serial.print(charBuffer);
    Serial.print(", printByte=");
    printByte(inputDataByte);
    Serial.print(", writeInByte=");
    // Special characters.
    if (inputDataByte == 10) {
      Serial.print("<LF>");
    } else if (inputDataByte == 13) {
      Serial.print("<CR>");
    } else {
      Serial.print("'");
      Serial.write(inputDataByte);
      Serial.print("'");
    }
    Serial.println();
  }
#endif
  host_set_data_leds(inputDataByte);
  host_set_addr_leds(portDataByte + portDataByte * 256); // As does the origanal Altair 8800: lb and hb set to the port#.
  if (host_read_status_led_WAIT()) {
    singleStepWait();
  } else {
    printFrontPanel();
  }
  host_clr_status_led_INP();
  return inputDataByte;
}

// -----------------------------------------------------------------------------
void altair_out(byte portDataByte, byte regAdata) {
  // Opcode: out <port>
  // Called from: cpu_OUT() { altair_out(MEM_READ(regPC), regA); ... }
#ifdef LOG_MESSAGES
  Serial.print(F("< OUT, port# "));
  Serial.print(portDataByte);
  Serial.print(" regA=");
  Serial.print(regAdata);
  Serial.println(".");
#endif
  host_set_data_leds(regAdata);
  host_set_addr_leds(portDataByte + portDataByte * 256); // The low and high bytes are each set to the portDataByte.
  host_clr_status_led_MEMR();
  host_set_status_led_OUT();
  host_set_status_led_WO();
  //
  // Write output byte to the output port.
  switch (portDataByte) {
    case 2:
      // Out to the USB serial port.
      Serial.write(regAdata);
      break;
    case 16:
      // Actual output of bytes. Example output a byte to the serial port (IDE monitor).
      // Test port: 20Q (octal: 020).
      Serial.print("++ Test output port, byte output to USB serial port:");
      Serial.print(regAdata);         // Write regAdata to serial port.
      Serial.println(":");
      break;
    default:
      Serial.print("-- Output port not found: ");
      Serial.print(portDataByte);
      Serial.println(".");
  }
  if (host_read_status_led_WAIT()) {
    singleStepWait();
  } else {
    printFrontPanel();
  }
  host_clr_status_led_OUT();
  host_clr_status_led_WO();
}

void setAddressData(uint16_t addressWord, byte dataByte) {
#ifdef LOG_MESSAGES
  Serial.print(F("+ altair_set_outputs, address:"));
  Serial.print(addressWord);
  Serial.print(" dataByte:");
  Serial.println(dataByte);
#endif
  host_set_addr_leds(addressWord);
  host_set_data_leds(dataByte);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Processor RUN mode: Running machine instructions

void processDataOpcode() {
#ifdef LOG_OPCODES
  Serial.print(F("++ processDataOpcode(), regPC:"));
  Serial.print(regPC);
  Serial.print(F(": data, opcode:"));
  printData(MREAD(regPC));
  Serial.println("");
#endif
  host_set_status_leds_READMEM_M1();
  opcode = MREAD(regPC);
  host_clr_status_led_M1();       // dave testing
  host_set_addr_leds(regPC);
  host_set_data_leds(opcode);
  if (!SERIAL_IO || (programState == PROGRAM_RUN)) {
    printFrontPanel();
  }
  regPC++;
  CPU_EXEC(opcode);
  //
  // Set for next opcode read:
  if (!host_read_status_led_HLTA()) {
    host_set_status_leds_READMEM_M1();
  }
}

void processRunSwitch(byte readByte) {
  if (SERIAL_IO_VT100) {
    Serial.print("\033[J");     // From cursor down, clear the screen, .
  }
  switch (readByte) {
    // Could add, if serial VT100:
    //  case  3 (Crtl+c) instead of case 's'.
    //  case 26 (Crtl+z) instead of case 'R'.
    case 's':
      Serial.println(F("+ s, STOP"));
      programState = PROGRAM_WAIT;
      host_set_status_led_WAIT();
      host_set_status_leds_READMEM_M1();
      if (!SERIAL_IO) {
        printFrontPanel();  // <LF> will refresh the display.
      }
      break;
    case 'R':
      Serial.println(F("+ R, RESET"));
      // Set to the first memory address.
      regPC = 0;
      setAddressData(regPC, MREAD(regPC));
    // Then continue running, if in RUN mode.
    // -------------------------------------
    default:
      // Load the byte for use by cpu_IN();
      inputBytePort2 = readByte;
      break;
  }
}

void runProcessor() {
  Serial.println(F("+ runProcessor()"));
  // Serial.println(F("+ Send serial character 's' to STOP running."));
  host_set_addr_leds(regPC);
  programState = PROGRAM_RUN;
  while (programState == PROGRAM_RUN) {
    processDataOpcode();
    if (Serial.available() > 0) {
      readByte = Serial.read();    // Read and process an incoming byte.
      processRunSwitch(readByte);
    }
  }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Processor WAIT mode: Load sample programs.

String loadProgramName = "";
void loadProgram() {
  int cnt;
  cnt = 0;
#ifdef LOG_MESSAGES
  Serial.println(F("+ loadProgram()"));
#endif
  if (loadProgramName != "") {
    Serial.print(F("+ Program previously loaded: "));
    Serial.println(loadProgramName);
  }
  programState = PROGRAM_LOAD;
  while (programState == PROGRAM_LOAD) {
    if (Serial.available() > 0) {
      readByte = Serial.read();    // Read and process an incoming byte.
      switch (readByte) {
        case 'i':
          loadProgramName = "Input loop";
          Serial.println("+ i, load: Input loop, 'x' to halt the loop.");
          programState = PROGRAM_WAIT;
          if (SERIAL_IO_VT100) {
            Serial.print("\033[J");     // From cursor down, clear the screen, .
          }
          MWRITE( cnt++, B11011011 & 0xff);  // ++ opcode:in:11011011:SERIAL_PORT
          MWRITE( cnt++, B00000011 & 0xff);  // ++ immediate:SERIAL_PORT:3
          MWRITE( cnt++, B11111110 & 0xff);  // ++ opcode:cpi:11111110:'x'
          MWRITE( cnt++, B01111000 & 0xff);  // ++ immediate:'x':120
          MWRITE( cnt++, B11001010 & 0xff);  // ++ opcode:jz:11001010:HaltLoop
          MWRITE( cnt++, B00001111 & 0xff);  // ++ lb:HaltLoop:15
          MWRITE( cnt++, B00000000 & 0xff);  // ++ hb:0
          MWRITE( cnt++, B11111110 & 0xff);  // ++ opcode:cpi:11111110:3
          MWRITE( cnt++, B00000011 & 0xff);  // ++ immediate:3:3
          MWRITE( cnt++, B11001010 & 0xff);  // ++ opcode:jz:11001010:HaltLoop
          MWRITE( cnt++, B00001111 & 0xff);  // ++ lb:HaltLoop:15
          MWRITE( cnt++, B00000000 & 0xff);  // ++ hb:0
          MWRITE( cnt++, B11000011 & 0xff);  // ++ opcode:jmp:11000011:GetByte
          MWRITE( cnt++, B00000000 & 0xff);  // ++ lb:GetByte:0
          MWRITE( cnt++, B00000000 & 0xff);  // ++ hb:0
          MWRITE( cnt++, B01110110 & 0xff);  // ++ opcode:hlt:01110110
          MWRITE( cnt++, B11000011 & 0xff);  // ++ opcode:jmp:11000011:GetByte
          MWRITE( cnt++, B00000000 & 0xff);  // ++ lb:GetByte:0
          MWRITE( cnt++, B00000000 & 0xff);  // ++ hb:0
          break;
        case 'j':
          loadProgramName = "Kill the Bit";
          Serial.println("+ j, load: Kill the Bit version for serial input.");
          programState = PROGRAM_WAIT;
          if (SERIAL_IO_VT100) {
            Serial.print("\033[J");     // From cursor down, clear the screen, .
          }
          MWRITE( cnt++, B00100001 & 0xff);  // ++ opcode:lxi:00100001:h:0
          MWRITE( cnt++, B00000000 & 0xff);  // ++ lb:0:0
          MWRITE( cnt++, B00000000 & 0xff);  // ++ hb:0
          MWRITE( cnt++, B00010110 & 0xff);  // ++ opcode:mvi:00010110:d:080h
          MWRITE( cnt++, B10000000 & 0xff);  // ++ immediate:080h:128
          MWRITE( cnt++, B00000001 & 0xff);  // ++ opcode:lxi:00000001:b:500h
          MWRITE( cnt++, B00000000 & 0xff);  // ++ lb:500h:0  ; Speed, lower number, faster.
          MWRITE( cnt++, B00000101 & 0xff);  // ++ hb:5                   Changed from 101, now faster for Serial demo.
          MWRITE( cnt++, B00011010 & 0xff);  // ++ opcode:ldax:00011010:d ---------------- Label: Begin, address 8
          MWRITE( cnt++, B00011010 & 0xff);  // ++ opcode:ldax:00011010:d
          MWRITE( cnt++, B00011010 & 0xff);  // ++ opcode:ldax:00011010:d
          // Only blink address hb value 3 times(ldax d), then check for input and reset register d.
          MWRITE( cnt++, B11011011 & 0xff);  // ++ opcode:in:11011011:0ffh
          MWRITE( cnt++, B00000010 & 0xff);  // ++ immediate:2:2            USB serial input.
          MWRITE( cnt++, B10101010 & 0xff);  // ++ opcode:xra:10101010:d
          MWRITE( cnt++, B00001111 & 0xff);  // ++ opcode:rrc:00001111
          MWRITE( cnt++, B01010111 & 0xff);  // ++ opcode:mov:01010111:d:a
          MWRITE( cnt++, B11000011 & 0xff);  // ++ opcode:jmp:11000011:Begin
          MWRITE( cnt++, B00001000 & 0xff);  // ++ lb:Begin:8            ----------------
          MWRITE( cnt++, B00000000 & 0xff);  // ++ hb:0                     Address 23
          break;
        case 'k':
          loadProgramName = "Kill the Bit";
          Serial.println("+ k, load: Kill the Bit.");
          programState = PROGRAM_WAIT;
          if (SERIAL_IO_VT100) {
            Serial.print("\033[J");     // From cursor down, clear the screen, .
          }
          MWRITE( cnt++, B00100001 & 0xff);  // ++ opcode:lxi:00100001:h:0
          MWRITE( cnt++, B00000000 & 0xff);  // ++ lb:0:0
          MWRITE( cnt++, B00000000 & 0xff);  // ++ hb:0
          MWRITE( cnt++, B00010110 & 0xff);  // ++ opcode:mvi:00010110:d:080h
          MWRITE( cnt++, B10000000 & 0xff);  // ++ immediate:080h:128
          MWRITE( cnt++, B00000001 & 0xff);  // ++ opcode:lxi:00000001:b:500h
          MWRITE( cnt++, B00000000 & 0xff);  // ++ lb:500h:0  ; Speed, lower number, faster.
          MWRITE( cnt++, B00000101 & 0xff);  // ++ hb:5                   Changed from 101, now faster for Serial demo.
          MWRITE( cnt++, B00011010 & 0xff);  // ++ opcode:ldax:00011010:d ---------------- Label: Begin, address 8
          MWRITE( cnt++, B00011010 & 0xff);  // ++ opcode:ldax:00011010:d
          MWRITE( cnt++, B00011010 & 0xff);  // ++ opcode:ldax:00011010:d
          MWRITE( cnt++, B00011010 & 0xff);  // ++ opcode:ldax:00011010:d
          MWRITE( cnt++, B00001001 & 0xff);  // ++ opcode:dad:00001001:b
          MWRITE( cnt++, B11010010 & 0xff);  // ++ opcode:jnc:11010010:Begin
          MWRITE( cnt++, B00001000 & 0xff);  // ++ lb:Begin:8             ----------------
          MWRITE( cnt++, B00000000 & 0xff);  // ++ hb:0                     Address 15
          MWRITE( cnt++, B11011011 & 0xff);  // ++ opcode:in:11011011:0ffh
          MWRITE( cnt++, B00000010 & 0xff);  // ++ immediate:2:2            USB serial input.
          MWRITE( cnt++, B10101010 & 0xff);  // ++ opcode:xra:10101010:d
          MWRITE( cnt++, B00001111 & 0xff);  // ++ opcode:rrc:00001111
          MWRITE( cnt++, B01010111 & 0xff);  // ++ opcode:mov:01010111:d:a
          MWRITE( cnt++, B11000011 & 0xff);  // ++ opcode:jmp:11000011:Begin
          MWRITE( cnt++, B00001000 & 0xff);  // ++ lb:Begin:8            ----------------
          MWRITE( cnt++, B00000000 & 0xff);  // ++ hb:0                     Address 23
          break;
        case 'm':
          loadProgramName = "Registers MVI test";
          Serial.println("+ m, load: MVI testing to the setting of registers.");
          programState = PROGRAM_WAIT;
          if (SERIAL_IO_VT100) {
            Serial.print("\033[J");     // From cursor down, clear the screen, .
          }
          MWRITE( cnt++, B00111110 & 0xff);  // ++ opcode:mvi:00111110:a:1
          MWRITE( cnt++, B00000001 & 0xff);  // ++ immediate:1:1
          MWRITE( cnt++, B00000110 & 0xff);  // ++ opcode:mvi:00000110:b:2
          MWRITE( cnt++, B00000010 & 0xff);  // ++ immediate:2:2
          MWRITE( cnt++, B00001110 & 0xff);  // ++ opcode:mvi:00001110:c:3
          MWRITE( cnt++, B00000011 & 0xff);  // ++ immediate:3:3
          MWRITE( cnt++, B00010110 & 0xff);  // ++ opcode:mvi:00010110:d:4
          MWRITE( cnt++, B00000100 & 0xff);  // ++ immediate:4:4
          MWRITE( cnt++, B00011110 & 0xff);  // ++ opcode:mvi:00011110:e:5
          MWRITE( cnt++, B00000101 & 0xff);  // ++ immediate:5:5
          MWRITE( cnt++, B00100110 & 0xff);  // ++ opcode:mvi:00100110:h:6
          MWRITE( cnt++, B00000110 & 0xff);  // ++ immediate:6:6
          MWRITE( cnt++, B00101110 & 0xff);  // ++ opcode:mvi:00101110:l:7
          MWRITE( cnt++, B00000111 & 0xff);  // ++ immediate:7:7
          MWRITE( cnt++, B01110110 & 0xff);  // ++ opcode:hlt:01110110
          MWRITE( cnt++, B11000011 & 0xff);  // ++ opcode:jmp:11000011:Start
          MWRITE( cnt++, B00000000 & 0xff);  // ++ lb:Start:0
          MWRITE( cnt++, B00000000 & 0xff);  // ++ hb:0
          break;
        case 'w':
          loadProgramName = "Write byte to memory location: 96";
          Serial.println("+ w, load: Write byte to memory location: 96, increment byte and loop.");
          if (SERIAL_IO_VT100) {
            Serial.print("\033[J");     // From cursor down, clear the screen, .
          }
          programState = PROGRAM_WAIT;
          MWRITE( cnt++, B00111110 & 0xff);  // ++ opcode:mvi:00111110:a:6
          MWRITE( cnt++, B00000110 & 0xff);  // ++ immediate:6:6
          MWRITE( cnt++, B00110010 & 0xff);  // ++ opcode:sta:00110010:96
          MWRITE( cnt++, B01100000 & 0xff);  // ++ lb:96:96
          MWRITE( cnt++, B00000000 & 0xff);  // ++ hb:0
          MWRITE( cnt++, B00111110 & 0xff);  // ++ opcode:mvi:00111110:a:0
          MWRITE( cnt++, B00000000 & 0xff);  // ++ immediate:0:0
          MWRITE( cnt++, B01110110 & 0xff);  // ++ opcode:hlt:01110110
          MWRITE( cnt++, B00111010 & 0xff);  // ++ opcode:lda:00111010:96
          MWRITE( cnt++, B01100000 & 0xff);  // ++ lb:96:96
          MWRITE( cnt++, B00000000 & 0xff);  // ++ hb:0
          MWRITE( cnt++, B01110110 & 0xff);  // ++ opcode:hlt:01110110
          MWRITE( cnt++, B00111100 & 0xff);  // ++ opcode:inr:00111100:a
          MWRITE( cnt++, B11000011 & 0xff);  // ++ opcode:jmp:11000011:Store
          MWRITE( cnt++, B00000010 & 0xff);  // ++ lb:Store:2
          MWRITE( cnt++, B00000000 & 0xff);  // ++ hb:0
          break;
        case 's':
          loadProgramName = "Front panel status light test";
          Serial.println("+ s, load: Front panel status light test.");
          if (SERIAL_IO_VT100) {
            Serial.print("\033[J");     // From cursor down, clear the screen, .
          }
          programState = PROGRAM_WAIT;
          // -----------------------------
          // Front panel status light test: https://www.youtube.com/watch?v=3_73NwB6toY
          //
          MWRITE(    32, B11101011 & 0xff);  // The video has 235(octal 353, B11101011) in address 32(B00100000).
          init_regS();      // Currently, difference is the flag byte (regS) is B01000000, where my flag byte is B00000010
          //
          MWRITE( cnt++, B00111010 & 0xff);  // ++ opcode:lda:00111010:32
          MWRITE( cnt++, B00100000 & 0xff);  // ++ lb:32:32
          MWRITE( cnt++, B00000000 & 0xff);  // ++ hb:0
          MWRITE( cnt++, B00110010 & 0xff);  // ++ opcode:sta:00110010:33
          MWRITE( cnt++, B00100001 & 0xff);  // ++ lb:33:33
          MWRITE( cnt++, B00000000 & 0xff);  // ++ hb:0
          MWRITE( cnt++, B00110001 & 0xff);  // ++ opcode:lxi:00110001:sp:32
          MWRITE( cnt++, B00100000 & 0xff);  // ++ lb:32:32
          MWRITE( cnt++, B00000000 & 0xff);  // ++ hb:0
          MWRITE( cnt++, B11110101 & 0xff);  // ++ opcode:push:11110101:a
          MWRITE( cnt++, B11110001 & 0xff);  // ++ opcode:pop:11110001:a
          MWRITE( cnt++, B11011011 & 0xff);  // ++ opcode:in:11011011:16
          MWRITE( cnt++, B00010000 & 0xff);  // ++ immediate:16:16
          MWRITE( cnt++, B11010011 & 0xff);  // ++ opcode:out:11010011:16
          MWRITE( cnt++, B00010000 & 0xff);  // ++ immediate:16:16
          MWRITE( cnt++, B11111011 & 0xff);  // ++ opcode:ei:11111011
          MWRITE( cnt++, B11110011 & 0xff);  // ++ opcode:di:11110011
          MWRITE( cnt++, B01110110 & 0xff);  // ++ opcode:hlt:01110110
          MWRITE( cnt++, B11000011 & 0xff);  // ++ opcode:jmp:11000011:Start
          MWRITE( cnt++, B00000000 & 0xff);  // ++ lb:Start:0
          MWRITE( cnt++, B00000000 & 0xff);  // ++ hb:0
          break;
        // -------------------------------------
        case 'x':
          Serial.println("< x, Exit load program.");
          programState = PROGRAM_WAIT;
          if (SERIAL_IO_VT100) {
            Serial.print("\033[J");     // From cursor down, clear the screen, .
          }
          break;
        default:
          if (SERIAL_IO_VT100) {
            Serial.print("\033[9;1H");  // Move cursor to below the prompt: line 9, column 1.
            Serial.print("\033[J");     // From cursor down, clear the screen, .
          }
          Serial.println("+ i, Input loop.");
          Serial.println("+ j, Kill the Bit version for serial VT100 input.");
          Serial.println("+ k, Kill the Bit.");
          Serial.println("+ m, MVI testing to the setting of registers.");
          Serial.println("+ w, Write byte to memory location: 96, increment byte and loop.");
          Serial.println("+ s, Front panel status light test.");
          Serial.println("----------");
          Serial.println("+ x, Exit: don't load a program.");
      }
    }
  }
  if (readByte != 'x') {
    // Do EXAMINE 0 after the load;
    regPC = 0;
    setAddressData(regPC, MREAD(regPC));
    if (SERIAL_IO_VT100) {
      serialPrintFrontPanel();
    }
  }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Processor WAIT mode: process switch byte options.

void processWaitSwitch(byte readByte) {
  uint16_t cnt;
  //
  if (SERIAL_IO_VT100) {
    Serial.print("\033[9;1H");  // Move cursor to below the prompt: line 9, column 1.
    Serial.print("\033[J");     // From cursor down, clear the screen.
  }
  //
  // -------------------------------
  // Process an address/data toggle.
  //
  int data = readByte;
  if ( data >= '0' && data <= '9' ) {
    // Serial input, not hardware input.
    fpAddressToggleWord = fpAddressToggleWord ^ (1 << (data - '0'));
    if (!SERIAL_IO) {
      printFrontPanel();
    }
    return;
  }
  if ( data >= 'a' && data <= 'f' ) {
    // Serial input, not hardware input.
    fpAddressToggleWord = fpAddressToggleWord ^ (1 << (data - 'a' + 10));
    if (!SERIAL_IO) {
      printFrontPanel();
    }
    return;
  }
  // -------------------------------
  // The following can either initiate from a serial connection or from a hardware switch.
  // Process command switches: RUN, SINGLE STEP, EXAMINE, EXAMINE NEXT, DEPOSIT, DEPOSIT NEXT, RESET.
  //
  switch (readByte) {
    case 'r':
      Serial.println("+ r, RUN.");
      if (fpStatusByte & HLTA_ON) {
        // If previous instruction was HLT, step to next instruction after HLT, then continue.
        regPC++;
        host_clr_status_led_HLTA();
      }
      host_clr_status_led_WAIT();
      programState = PROGRAM_RUN;
      break;
    case 's':
      singleStepWaitByte = 0;   // Used to identify if there was command input during a SINGLE STEP cycle.
      if (fpStatusByte & HLTA_ON) {
        Serial.println("+ s, SINGLE STEP, from HLT.");
        regPC++;
        setAddressData(regPC, MREAD(regPC));
        host_clr_status_led_HLTA();
      } else {
        Serial.println("+ s, SINGLE STEP, processDataOpcode()");
        processDataOpcode();
      }
      host_set_status_leds_READMEM_M1();
      setAddressData(regPC, MREAD(regPC));
      if (!SERIAL_IO) {
        printFrontPanel();  // <LF> will refresh the display.
      }
      break;
    case 'x':
      regPC = fpAddressToggleWord;
      Serial.print("+ x, EXAMINE: ");
      Serial.println(regPC);
      setAddressData(regPC, MREAD(regPC));
      if (!SERIAL_IO) {
        printFrontPanel();  // <LF> will refresh the display.
      }
      break;
    case 'X':
      regPC = regPC + 1;
      Serial.print("+ y, EXAMINE NEXT: ");
      Serial.println(regPC);
      setAddressData(regPC, MREAD(regPC));
      if (!SERIAL_IO) {
        printFrontPanel();  // <LF> will refresh the display.
      }
      break;
    case 'p':
      Serial.print("+ p, DEPOSIT to: ");
      Serial.println(regPC);
      MWRITE(regPC, fpAddressToggleWord & 0xff);
      setAddressData(regPC, MREAD(regPC));
      if (!SERIAL_IO) {
        printFrontPanel();  // <LF> will refresh the display.
      }
      break;
    case 'P':
      regPC++;
      Serial.print("+ P, DEPOSIT NEXT to: ");
      Serial.println(regPC);
      MWRITE(regPC, fpAddressToggleWord & 0xff);
      setAddressData(regPC, MREAD(regPC));
      if (!SERIAL_IO) {
        printFrontPanel();  // <LF> will refresh the display.
      }
      break;
    case 'R':
      Serial.println("+ R, RESET.");
      // For now, do EXAMINE 0 to reset to the first memory address.
      //
      regA = 0;
      regB = 0;
      regC = 0;
      regD = 0;
      regE = 0;
      regH = 0;
      regL = 0;
      regPC = 0;
      regSP = 0;
      //
      setAddressData(regPC, MREAD(regPC));
      host_clr_status_led_HLTA();
      host_set_status_leds_READMEM_M1();
      if (!SERIAL_IO) {
        printFrontPanel();  // <LF> will refresh the display.
      }
      break;
    case 'C':
      byte readConfirmByte;
      Serial.println("+ C, CLR: Clear memory, set registers to zero, and program counter address to zero.");
      Serial.println("+ Confirm CLR: y/n");
      readConfirmByte = 's';
      while (!(readConfirmByte == 'y' || readConfirmByte == 'n')) {
        if (Serial.available() > 0) {
          readConfirmByte = Serial.read();    // Read and process an incoming byte.
        }
        delay(60);
      }
      if (readConfirmByte != 'y') {
        Serial.println("+ CLR cancelled.");
        break;
      }
      Serial.println("+ CLR confirmed.");
      Serial.print("+ Clear memory: ");
      Serial.println(MEMSIZE);
      for (int i = 0; i < MEMSIZE; i++) {
        MWRITE(i, 0);
      }
      regA = 0;
      regB = 0;
      regC = 0;
      regD = 0;
      regE = 0;
      regH = 0;
      regL = 0;
      regPC = 0;
      regSP = 0;
      //
      host_set_status_leds_READMEM_M1();
      fpDataByte = 0;           // Data
      fpAddressWord = 0;        // Address lb
      fpAddressToggleWord = 0;  // Reset all toggles to off.
      //
      setAddressData(regPC, MREAD(regPC));
      if (!SERIAL_IO) {
        printFrontPanel();  // <LF> will refresh the display.
      }
      break;
    // -------------------------------------------------------------------
    // The following requires a VT100 terminal such as a Macbook terminal.
    // The following doesn't work on the Ardino monitor.
    //
    case 'v':
      SERIAL_IO_VT100 = false;
      SERIAL_IO = true;
      Serial.println("+ v, VT100 escapes are disabled and block cursor on.");
      Serial.print("\033[0m\033[?25h");       // Block cursor display: on.
      break;
    case 'V':
      // Insure the previous value is different to current, to insure an intial printing.
      prev_fpDataByte = host_read_data_leds() + 1;
      prev_fpAddressToggleWord = host_read_addr_leds() + 1;;
      fpAddressToggleWord = 0;      // Set all toggled off.
      prev_fpAddressToggleWord = 1; // Set to different value.
      Serial.print("\033[0m\033[?25l");       // Block cursor display: off.
      Serial.print("\033[H\033[2J");          // Cursor home and clear the screen.
      serialPrintFrontPanel();                // Print the complete front panel: labels and indicators.
      SERIAL_IO_VT100 = true;                 // Must be after serialPrintFrontPanel(), to have the labels printed.
      SERIAL_IO = false;                      // Insure it's disabled.
      Serial.println("+ V, VT100 escapes are enabled and block cursor off.");
      break;
    // -------------------------------------------------------------------
    case 'w':
      SERIAL_IO = false;
      Serial.println("+ w, USB serial output is disabled.");
      break;
    case 'W':
      SERIAL_IO = true;
      Serial.println("+ W, USB serial output is enabled.");
      break;
    // -------------------------------------
    case 'o':
      Serial.println("+ o, disable output to LED lights.");
      LED_IO = false;
      break;
    case 'O':
      Serial.println("+ O, enable output to LED lights.");
      LED_IO = true;
      break;
    //
    // -------------------------------------------------------------------
    case 'z':
      Serial.print("++ Cursor off.");
      Serial.print("\033[0m\033[?25l");
      break;
    case 'Z':
      Serial.print("++ Cursor on.");
      Serial.print("\033[0m\033[?25h");
      break;
    //
    // -------------------------------------
    case 'l':
      Serial.println("+ l, load a sample program.");
      loadProgram();
      if (!SERIAL_IO) {
        printFrontPanel();  // <LF> will refresh the display.
      }
      break;
    case 'i':
      Serial.println("+ i: Information.");
      cpucore_i8080_print_registers();
      Serial.println("------------");
      Serial.print(F("++ LED_IO = "));
      Serial.print(LED_IO);
      Serial.print(F(" SERIAL_IO_VT100 = "));
      Serial.print(SERIAL_IO_VT100);
      Serial.print(F(" SERIAL_IO = "));
      Serial.println(SERIAL_IO);
      break;
    case 'h':
      Serial.println("----------------------------------------------------");
      Serial.println("+++ Commands");
      Serial.println("-------------");
      Serial.println("+ h, Help         Print this help menu.");
      Serial.println("-------------");
      Serial.println("+ 0...9, a...f    Toggle address switches:  A0...A9, A10...A15.");
      Serial.println("-------------");
      Serial.println("+ r, RUN mode.");
      Serial.println("+ s, STOP         When in RUN mode, change to WAIT mode.");
      Serial.println("+ s, SINGLE STEP  When in WAIT mode.");
      Serial.println("+ x, EXAMINE      current switch address.");
      Serial.println("+ X, EXAMINE NEXT address, current address + 1.");
      Serial.println("+ p, DEPOSIT      current address");
      Serial.println("+ P, DEPOSIT NEXT address, current address + 1");
      Serial.println("+ R, RESET        Set program counter address to zero.");
      Serial.println("+ C, CLR          Clear memory, set registers and program counter address to zero.");
      Serial.println("-------------");
      Serial.println("                  Can enter a series of values when using the Arduino IDE monitor.");
      Serial.println("+ 013x            Example, toggle switches 0,1,3 (00001011), and EXAMINE the address.");
      Serial.println("-------------");
      Serial.println("+ i, info         Information print of registers.");
      Serial.println("+ l, load         Load a sample program.");
      Serial.println("-------------");
      Serial.println("+ o/O LEDs        Disable/enable LED light output.");
      Serial.println("+ v/V VT100       Disable/enable USB serial VT100 output.");
      Serial.println("+ w/W USB serial  Disable/enable USB serial output");
      Serial.println("+ z/Z cursor off  VT100 block cursor off/on.");
      Serial.println("+ Enter key       Refresh USB serial output front panel display.");
      Serial.println("----------------------------------------------------");
      break;
    // -------------------------------------
    case 10:
      // case 10: is for USB serial
      // case 13: is for USB serial VT100. Not needed here.
      serialPrintFrontPanel();
      break;
    case 'y':
      Serial.println(F("+ y, Refresh front panel."));
      if (SERIAL_IO_VT100) {
        Serial.print("\033[H\033[2J");  // Cursor home and clear the screen.
      }
      printFrontPanel();                // Print the complete front panel: labels and indicators.
      break;
    // -------------------------------------
    default:
      Serial.print("- Ignored <");
      Serial.write(readByte);
      Serial.println(">");
  }
}

void runProcessorWait() {
#ifdef LOG_MESSAGES
  Serial.println(F("+ runProcessorWait()"));
#endif
  while (programState == PROGRAM_WAIT) {
    // Program control: RUN, SINGLE STEP, EXAMINE, EXAMINE NEXT, Examine previous, RESET.
    // And other options such as enable VT100 output enabled or load a sample program.
    if (Serial.available() > 0) {
      readByte = Serial.read();    // Read and process an incoming byte.
      processWaitSwitch(readByte);
      if (singleStepWaitByte) {
        // This handles inputs during a SINGLE STEP cycle that hasn't finished.
        processWaitSwitch(readByte);
      }
    }
    delay(60);
  }
}

// -----------------------------------------------------------------------------
void setup() {
  // Speed for serial read, which matches the sending program.
  Serial.begin(9600);   // 115200 19200
  delay(2000);
  Serial.println(""); // Newline after garbage characters.
  Serial.println("+++ Setup.");
  //
  // ----------------------------------------------------
  // ----------------------------------------------------
  // Front panel LED lights.

  // System application status LED lights
  pinMode(WAIT_PIN, OUTPUT);    // Indicator: program WAIT state: LED on or LED off.
  pinMode(HLDA_PIN, OUTPUT);    // Indicator: clock or player process (LED on) or emulator (LED off).
  digitalWrite(WAIT_PIN, HIGH); // Default to wait state.
  digitalWrite(HLDA_PIN, HIGH); // Default to emulator.
  // ------------------------------
  // Set status lights.
  Serial.println(F("+ Initialized: statusByte, programCounter & curProgramCounter, dataByte."));
  pinMode(latchPinLed, OUTPUT);
  pinMode(clockPinLed, OUTPUT);
  pinMode(dataPinLed, OUTPUT);
  delay(300);
  ledFlashSuccess();
  Serial.println(F("+ Front panel LED lights are initialized."));
  //
  // ----------------------------------------------------
  // ----------------------------------------------------
  programState = PROGRAM_WAIT;
  host_set_status_led_WAIT();
  host_set_status_leds_READMEM_M1();
  regPC = 0;
  opcode = MREAD(regPC);
  host_set_addr_leds(regPC);
  host_set_data_leds(opcode);
  printFrontPanel();
  Serial.println("+++ Altair 101a initialized.");
  // ----------------------------------------------------
  Serial.println(F("+ Starting the processor loop."));
}

// -----------------------------------------------------------------------------
// Device Loop
void loop() {
  switch (programState) {
    // ----------------------------
    case PROGRAM_RUN:
      runProcessor();
      break;
    // ----------------------------
    case PROGRAM_WAIT:
      runProcessorWait();
      break;
  }
  delay(30);
}
// -----------------------------------------------------------------------------
