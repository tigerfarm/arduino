// -----------------------------------------------------------------------------
/*
  Processor
  + Serial interactivity using the Arduino IDE monitor USB serial port.
  + Using David Hansel's Altair 8800 Simulator code to process machine code instructions.

  + When SINGLE STEP reads and write, I've chose to show the actual values versus
    having all the data lights on which is what the original Altair 8800 does.

  ---------------------------------------------------------
  Next:

  + Have the status lights work the same for RUN and WAIT modes.
  ++ In run mode, consider having all data lights on, same as the Altair 8800.

  + Get Kill the Bit to run.

  + When single stepping, M1 stays on but should be off, when HLT is executed.
  + Should be on: MEMR, HLTA, WO.
  + On the Altair 101, only HLTA light is on.

  + Implement, printFrontPanel(), which calls: lightsStatusAddressData(Status,Address,Data).
  ++ Can handle LED_IO, once running on the Altair 101.
  + Implement: lightsStatusAddressData(Status,Address,Data)
  ++ Use optional outputs: serial, and/not LED lights (on/off using serial command).

  + Prevent lockup when using PUSH A, i.e. PUSH called before setting SP.
  ++ In the PUSH process, if SP < 4, error.

  ---------------------------------------------------------
*/
// -----------------------------------------------------------------------------
#include "Altair101a.h"
#include "cpucore_i8080.h"

#define LOG_MESSAGES 1    // For debugging.

// -----------------------------------------------------------------------------
// For Byte bit comparisons.
#define BIT(n) (1<<(n))

// -----------------------------------------------------------------------------
byte opcode = 0xff;
static uint16_t p_regPC = 0xFFFF;
uint16_t controlSwitch = 0;
uint16_t addressSwitch = 0;

// Byte bit status values
#define ST_WAIT    0x0800
#define ST_HLDA    0x0400

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
// From Processor.ino

// Program states
#define LIGHTS_OFF 0
#define PROGRAM_WAIT 1
#define PROGRAM_RUN 2
#define CLOCK_RUN 3
#define PLAYER_RUN 4
#define SERIAL_DOWNLOAD 5
int programState = LIGHTS_OFF;  // Intial, default.

void ledFlashSuccess() {}

// ------------------------------
// Status Indicator LED lights

byte readByte = 0;
byte statusByte = B00000000;        // By default, all are OFF.

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

// Use AND to turn OFF. Example:
//  statusByte = statusByte & M1_OFF;
const byte MEMR_OFF =   ~MEMR_ON;
const byte INP_OFF =    ~INP_ON;
const byte M1_OFF =     ~M1_ON;
const byte OUT_OFF =    ~OUT_ON;
const byte HLTA_OFF =   ~HLTA_ON;
const byte STACK_OFF =  ~STACK_ON;
const byte WO_OFF =     ~WO_ON;
const byte INT_OFF =    ~INT_ON;

// -----------------------------------------------------------------------------
// Front Panel Status LEDs

// -------------------------------------------------
// Output LED lights shift register(SN74HC595N) pins
//           Mega/Nano pins        74HC595 Pins
const int dataPinLed  = 5;    // pin 5 (was pin A14) Data pin.
const int latchPinLed = 6;    // pin 6 (was pin A12) Latch pin.
const int clockPinLed = 7;    // pin 7 (was pin A11) Clock pin.

void lightsStatusAddressData( byte status8bits, unsigned int address16bits, byte data8bits) {
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
  digitalWrite(latchPinLed, LOW);
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, status8bits);
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, data8bits);
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, lowByte(address16bits));
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, highByte(address16bits));
  digitalWrite(latchPinLed, HIGH);
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
    lightsStatusAddressData(statusByteB, host_read_addr_leds(), host_read_data_leds());
  }
  if (SERIAL_IO_VT100) {
    // For now
    print_panel_serial();
#ifdef LOG_MESSAGES
    Serial.print("\033[2K"); // Clear line from cursor right
    Serial.print(F("+ printFrontPanel SERIAL_IO_VT100, status:"));
    printByte(statusByteB);
    Serial.print(" address:");
    sprintf(charBuffer, "%5d", host_read_addr_leds());
    Serial.print(charBuffer);
    Serial.print(" dataByte:");
    printByte(host_read_data_leds());
    Serial.println();
    Serial.print("\033[2K"); // Clear line
#endif
  } else {
    if (SERIAL_IO) {
      if (programState == PROGRAM_RUN) {
        Serial.print(F("+ printFrontPanel SERIAL_IO, status:"));
        printByte(statusByteB);
        Serial.print(" address:");
        sprintf(charBuffer, "%5d", host_read_addr_leds());
        Serial.print(charBuffer);
        Serial.print(" dataByte:");
        printByte(host_read_data_leds());
        Serial.println();
      } else {
        // WAIT mode.
        print_panel_serial();
      }
    }
  }
}

// -----------------------------------------------------------------------------
// Print the front panel to either the Arduino IDE monitor, or to the VT00 terminal.
// A VT00 terminal is much nicer because the front panel stays in the top left of the terminal screen.
// Where as the front panel is reprinted and scrolls in the Arduino IDE monitor.

// byte prev_statusByteB = 250;     // This will take more time to figure out.
byte prev_dataBus = 250;
uint16_t prev_addressSwitch = 32000;
uint16_t prev_addressBus = 32000;

void print_panel_serial() {
  //
  // Status
  if (!SERIAL_IO_VT100) {
    Serial.print(F("INTE MEMR INP M1 OUT HLTA STACK WO INT        D7  D6   D5  D4  D3   D2  D1  D0\r\n"));
  } else {
    // No need to rewrite the title.
    Serial.print("\033[H");   // Move cursor home
    Serial.print("\033[1B");  // Cursor down
  }
  if ( host_read_status_led_INTE() ) Serial.print(F(" *  ")); else Serial.print(F(" .  "));
  if ( statusByteB & MEMR_ON  ) Serial.print(F("  *  "));  else Serial.print(F("  . "));
  if ( statusByteB & INP_ON   ) Serial.print(F("  * "));   else Serial.print(F("  . "));
  if ( statusByteB & M1_ON    ) Serial.print(F(" * "));    else Serial.print(F(" . "));
  if ( statusByteB & OUT_ON   ) Serial.print(F("  * "));   else Serial.print(F("  . "));
  if ( statusByteB & HLTA_ON  ) Serial.print(F("  * "));   else Serial.print(F("  . "));
  if ( statusByteB & STACK_ON ) Serial.print(F("   * "));  else Serial.print(F("   . "));
  if ( statusByteB & WO_ON    ) Serial.print(F("   * "));  else Serial.print(F("   . "));
  if ( statusByteB & INT_ON   ) Serial.print(F("  *"));     else Serial.print(F("  ."));
  Serial.print(F("       "));
  //
  // Data
  byte dataBus = host_read_data_leds();
  if (prev_dataBus != dataBus) {
    // If no change, don't reprint.
    prev_dataBus = dataBus;
    if ( dataBus & 0x80 )   Serial.print(F("  *" )); else Serial.print(F("  ." ));
    if ( dataBus & 0x40 )   Serial.print(F("   *")); else Serial.print(F("   ."));
    Serial.print(F(" "));
    if ( dataBus & 0x20 )   Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( dataBus & 0x10 )   Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( dataBus & 0x08 )   Serial.print(F("   *")); else Serial.print(F("   ."));
    Serial.print(F(" "));
    if ( dataBus & 0x04 )   Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( dataBus & 0x02 )   Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( dataBus & 0x01 )   Serial.print(F("   *")); else Serial.print(F("   ."));
  }
  // WAIT and HLDA
  if (!SERIAL_IO_VT100) {
    Serial.print(("\r\nWAIT HLDA   A15 A14 A13 A12 A11 A10  A9  A8   A7  A6   A5  A4  A3   A2  A1  A0\r\n"));
  } else {
    // No need to rewrite the title.
    Serial.println();
    Serial.print("\033[1B");  // Cursor down
  }
  if ( host_read_status_led_WAIT() ) Serial.print(F(" *  "));   else Serial.print(F(" .  "));
  if ( false ) Serial.print(F("  *   ")); else Serial.print(F("  .   "));
  //
  // Address
  uint16_t addressBus = host_read_addr_leds();
  if (prev_addressBus != addressBus) {
    // If no change, don't reprint.
    prev_addressBus = addressBus;
    if ( addressBus & 0x8000 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( addressBus & 0x4000 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( addressBus & 0x2000 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( addressBus & 0x1000 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( addressBus & 0x0800 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( addressBus & 0x0400 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( addressBus & 0x0200 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( addressBus & 0x0100 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    Serial.print(F(" "));
    if ( addressBus & 0x0080 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( addressBus & 0x0040 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    Serial.print(F(" "));
    if ( addressBus & 0x0020 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( addressBus & 0x0010 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( addressBus & 0x0008 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    Serial.print(F(" "));
    if ( addressBus & 0x0004 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( addressBus & 0x0002 ) Serial.print(F("   *")); else Serial.print(F("   ."));
    if ( addressBus & 0x0001 ) Serial.print(F("   *")); else Serial.print(F("   ."));
  }
  // Address/Data switches
  if (!SERIAL_IO_VT100) {
    Serial.print(F("\r\n            S15 S14 S13 S12 S11 S10  S9  S8   S7  S6   S5  S4  S3   S2  S1  S0\r\n"));
  } else {
    // No need to rewrite the title.
    Serial.println();
    Serial.print("\033[1B");  // Cursor down
  }
  if (prev_addressSwitch != addressSwitch) {
    // If no change, don't reprint.
    prev_addressSwitch = addressSwitch;
    Serial.print(F("          "));
    if ( addressSwitch & 0x8000 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    if ( addressSwitch & 0x4000 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    if ( addressSwitch & 0x2000 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    if ( addressSwitch & 0x1000 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    if ( addressSwitch & 0x0800 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    if ( addressSwitch & 0x0400 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    if ( addressSwitch & 0x0200 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    if ( addressSwitch & 0x0100 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    Serial.print(F(" "));
    if ( addressSwitch & 0x0080 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    if ( addressSwitch & 0x0040 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    Serial.print(F(" "));
    if ( addressSwitch & 0x0020 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    if ( addressSwitch & 0x0010 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    if ( addressSwitch & 0x0008 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    Serial.print(F(" "));
    if ( addressSwitch & 0x0004 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    if ( addressSwitch & 0x0002 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
    if ( addressSwitch & 0x0001 ) Serial.print(F("   ^")); else Serial.print(F("   v"));
  }
  //
  if (!SERIAL_IO_VT100) {
    Serial.print(F("\r\n ------ \r\n"));
    Serial.println("+ Ready to receive command.");
  } else {
    // No need to rewrite the prompt.
    Serial.print("\033[2B");  // Cursor down 2 lines.
    Serial.println();
  }

}

// -----------------------------------------------------------------------------
void singleStepWait() {
  // dave
  Serial.println(F("+ singleStepWait()"));
  print_panel_serial();           // Status, data/address lights already set.
  bool singleStepWaitLoop = true;
  while (singleStepWaitLoop) {
    if (Serial.available() > 0) {
      readByte = Serial.read();    // Read and process an incoming byte.
      if (readByte == 's') {
        singleStepWaitLoop = false;
        // processRunSwitch(readByte);
      }
    }
  }
}

// -----------------------------------------------------------------------------
void altair_hlt() {
  host_set_status_led_HLTA();
  host_clr_status_led_M1();
  regPC--;
  // altair_interrupt(INT_SW_STOP);
  programState = PROGRAM_WAIT;
  Serial.print(F("++ HALT, host_read_status_led_WAIT() = "));
  Serial.println(host_read_status_led_WAIT());
  if (!host_read_status_led_WAIT()) {
    host_set_status_led_WAIT();
    print_panel_serial();
  }
}

void altair_interrupt_enable() {
  // altair_interrupts_enabled = true;
  host_set_status_led_INTE();
}

void altair_interrupt_disable() {
  // altair_interrupts_enabled = false;
  host_clr_status_led_INTE();
}

// -----------------------------------------------------------------------------
byte altair_in(byte portDataByte) {
  // Opcode: out <port>
  // Called from: cpu_IN() {
  //    regA = altair_in(MEM_READ(regPC)); ... }
  // Testing is port 16(octal 020).
  // Status
  byte inputDataByte;
  //
  // Get an input byte from the input port.
  switch (portDataByte) {
    case 1:
      // From Sense switches.
      inputDataByte = 1;
      break;
    case 2:
      // From serial port.
      inputDataByte = 2;
      break;
    case 16:
      // Test port: 20Q (octal: 020).
      inputDataByte = 2;  // Video input byte value.
      break;
    default:
      inputDataByte = 0;
  }
  //
#ifdef LOG_MESSAGES
  Serial.print(F("> Input port# "));
  Serial.print(portDataByte);
  Serial.print(" inputDataByte=");
  Serial.print(inputDataByte);
  Serial.println(" which is put into regA, cpucore_i8080.cpp: cpu_IN().");
#endif
  host_clr_status_led_MEMR();
  host_set_status_led_INP();
  host_set_data_leds(inputDataByte);
  host_set_addr_leds(portDataByte + portDataByte * 256); // The low and high bytes are each set to the portDataByte.
  if (host_read_status_led_WAIT()) {
    singleStepWait();
  }
  host_clr_status_led_INP();
  return inputDataByte;
}

void read_inputs() {
  byte readByte;
  readByte = "";
  // controlSwitch = 0;
  // ---------------------------
  // Device read options.
  // read_inputs_panel();
  //
  // dave, implement:
  if ( config_serial_input_enabled() ) {
    if (Serial.available() > 0) {
      readByte = Serial.read();    // Read and process an incoming byte.
    }
  }
  // ---------------------------
  if (readByte != "") {
    // processWaitSwitch(readByte);
  }
}
void read_inputs_panel() {
  // we react on positive edges on the function switches...
  // controlSwitch = host_read_function_switches_edge();
  // ...except for the SLOW switch which is active as long as it is held down
  // if ( host_read_function_switch_debounced(SW_SLOW) ) controlSwitch |= BIT(SW_SLOW);
  // #if STANDALONE==0
  //   addressSwitch = host_read_addr_switches();
  // #endif
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
    case 16:
      // Actual output of bytes. Example output a byte to the serial port (IDE monitor).
      // Test port: 20Q (octal: 020).
      Serial.print("++ Output byte to test port(serial port):");
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
  }
  host_clr_status_led_OUT();
  host_clr_status_led_WO();
}

void altair_set_outputs(uint16_t addressWord, byte dataByte) {
  // Stacy, When not using serial, display on front panel lights.
#ifdef LOG_MESSAGES
  Serial.print(F("+ altair_set_outputs, address:"));
  Serial.print(addressWord);
  Serial.print(" dataByte:");
  Serial.println(dataByte);
#endif
  host_set_addr_leds(addressWord);
  host_set_data_leds(dataByte);
  //
  // host_set_status_leds_READMEM_M1();
  /* Opcode memory read(M1):
    host_set_status_led_MEMR();
    host_clr_status_led_INP();
    host_set_status_led_M1();
    host_clr_status_led_OUT();
    host_clr_status_led_HLTA();
    host_clr_status_led_STACK();
    host_clr_status_led_WO();
    host_clr_status_led_INT();
  */
  // print_panel_serial();
}

// -----------------------------------------------------------------------------
// Running machine instructions

void processDataOpcode() {
#ifdef LOG_MESSAGES
  Serial.print(F("++ regPC:"));
  Serial.print(regPC);
  Serial.print(F(": data:"));
  printData(MREAD(regPC));
  Serial.println("");
#endif
  if (SERIAL_IO_VT100) {
    print_panel_serial();
  }
  //
  host_set_status_leds_READMEM_M1();
  opcode = MREAD(regPC);
  host_clr_status_led_M1();
  host_set_addr_leds(regPC);
  host_set_data_leds(opcode);
  regPC++;
  CPU_EXEC(opcode);
  //
  // Set for next opcode read:
  host_set_status_led_MEMR();
  host_set_status_led_M1();
  host_clr_status_led_WO();
}

void processRunSwitch(byte readByte) {
  switch (readByte) {
    case 's':
      Serial.println(F("+ STOP"));
      programState = PROGRAM_WAIT;
      host_set_status_led_WAIT();
      break;
    case 'R':
      Serial.println(F("+ RESET"));
      // Stacy, For now, do EXAMINE 0 to reset to the first memory address.
      regPC = 0;
      p_regPC = ~regPC;
      altair_set_outputs(regPC, MREAD(regPC));
    // Then continue running.
    // -------------------------------------
    case 10:
      // New line character.
      // For testing, require enter key from serial to run the next opcode. This is like a step operation.
      // processRunSwitch(readByte);
      break;
    default:
      // Serial.println(F("+ Default"));
      break;
  }
}

void runProcessor() {
  Serial.println(F("+ runProcessor()"));
  // put PC on address bus LEDs
  host_set_addr_leds(regPC);
  // Serial.println(F("+ Send serial character, example hit enter key, to process first opcode. Send 's' to STOP running."));
  programState = PROGRAM_RUN;
  while (programState == PROGRAM_RUN) {
    processDataOpcode(); // For now, require an serial character to process each opcode.
    if (Serial.available() > 0) {
      readByte = Serial.read();    // Read and process an incoming byte.
      processRunSwitch(readByte);
    }
  }
}

// -----------------------------------------------------------------------------
// Process WAIT mode inputs: switches.

void processWaitSwitch(byte readByte) {
  uint16_t cnt;
  //
  Serial.print("\033[9;1H");  // Move cursor to below the prompt: line 9, column 1.
  Serial.print("\033[J");     // Clear previous message on the screen, from cursor down.
  //
  // -------------------------------
  // Process an address/data toggle.
  //
  int data = readByte;
  if ( data >= '0' && data <= '9' ) {
    addressSwitch = addressSwitch ^ (1 << (data - '0'));
    if (SERIAL_IO_VT100) {
      print_panel_serial();
    }
    return;
  }
  if ( data >= 'a' && data <= 'f' ) {
    addressSwitch = addressSwitch ^ (1 << (data - 'a' + 10));
    if (SERIAL_IO_VT100) {
      print_panel_serial();
    }
    return;
  }
  // -------------------------------
  // Process command switches. Tested: RUN, SINGLE STEP, EXAMINE, EXAMINE NEXT, DEPOSIT, DEPOSIT NEXT, RESET
  //
  switch (readByte) {
    case 'r':
      Serial.println("+ r, RUN.");
      if (statusByteB & HLTA_ON) {
        // If previous instruction was HLT, step to next instruction after HLT.
        regPC++;
        host_clr_status_led_HLTA();
      }
      host_clr_status_led_WAIT();
      programState = PROGRAM_RUN;
      break;
    case 's':
      if (statusByteB & HLTA_ON) {
        Serial.println("+ s, SINGLE STEP, from HLT.");
        regPC++;
        host_clr_status_led_HLTA();
      } else {
        Serial.println("+ s, SINGLE STEP, processDataOpcode()");
        processDataOpcode();
      }
      altair_set_outputs(regPC, MREAD(regPC));
      altair_set_outputs(regPC, MREAD(regPC));
      if (SERIAL_IO_VT100) {
        print_panel_serial();
      }
      break;
    case 'x':
      regPC = addressSwitch;
      Serial.print("+ x, EXAMINE: ");
      Serial.println(regPC);
      p_regPC = ~regPC;
      altair_set_outputs(regPC, MREAD(regPC));
      if (SERIAL_IO_VT100) {
        print_panel_serial();
      }
      break;
    case 'X':
      regPC = regPC + 1;
      Serial.print("+ y, EXAMINE NEXT: ");
      Serial.println(regPC);
      altair_set_outputs(regPC, MREAD(regPC));
      if (SERIAL_IO_VT100) {
        print_panel_serial();
      }
      break;
    case 'p':
      Serial.print("+ p, DEPOSIT to: ");
      Serial.println(regPC);
      MWRITE(regPC, addressSwitch & 0xff);
      altair_set_outputs(regPC, MREAD(regPC));
      if (SERIAL_IO_VT100) {
        print_panel_serial();
      }
      break;
    case 'P':
      regPC++;
      Serial.print("+ P, DEPOSIT NEXT to: ");
      Serial.println(regPC);
      MWRITE(regPC, addressSwitch & 0xff);
      altair_set_outputs(regPC, MREAD(regPC));
      if (SERIAL_IO_VT100) {
        print_panel_serial();
      }
      break;
    case 'R':
      Serial.println("+ R, RESET.");
      // For now, do EXAMINE 0 to reset to the first memory address.
      regPC = 0;
      p_regPC = ~regPC;
      altair_set_outputs(regPC, MREAD(regPC));
      //
      host_clr_status_led_HLTA();
      if (SERIAL_IO_VT100) {
        print_panel_serial();
      }
      //
      // p_regPC = regPC;
      // Actual RESET action ?- set bus/data LEDs on, status LEDs off: altair_set_outputs(0xffff, 0xff);
      //
      break;
    // -------------------------------------
    case 'h':
      Serial.println("----------------------------------------------------");
      Serial.println("+++ Commands");
      Serial.println("-------------");
      Serial.println("+ h, Help         Print this help menu.");
      Serial.println("-------------");
      Serial.println("+ 0...9           Toggles:  A0...A9  address switches.");
      Serial.println("+ a...f           Toggles: A10...A15 address switches.");
      Serial.println("-------------");
      Serial.println("+ r, RUN mode.");
      Serial.println("+ s, STOP         when in RUN mode, change to WAIT mode.");
      Serial.println("+ s, SINGLE STEP  when in WAIT mode.");
      Serial.println("+ x, EXAMINE      current switch address.");
      Serial.println("+ X, EXAMINE NEXT address, current address + 1.");
      Serial.println("+ p, DEPOSIT      current address");
      Serial.println("+ P, DEPOSIT NEXT address, current address + 1");
      Serial.println("+ R, RESET        set program counter address to zero.");
      Serial.println("-------------");
      Serial.println("+ o, LEDs off     Do not output to LED lights.");
      Serial.println("+ O, LEDs on      Output to LED lights.");
      Serial.println("+ v, VT100 off    Don't use VT100 esacpes.");
      Serial.println("+ V, VT100 on     Use VT100 esacpes.");
      Serial.println("+ z, cursor off   VT100 cursor off.");
      Serial.println("+ Z, cursor on    VT100 cursor on.");
      Serial.println("-------------");
      Serial.println("+ i, info         Information print of registers.");
      Serial.println("+ l, load         Load a sample program.");
      Serial.println("-------------");
      Serial.println("+ l013x, Load program. Toggle switches 0,1,3 (00001011), and EXAMINE the address.");
      Serial.println("----------------------------------------------------");
      break;
    // -------------------------------------
    case 'v':
      Serial.println("+ v, VT100 escapes are disabled.");
      SERIAL_IO_VT100 = false;
      break;
    case 'V':
      // Reset the previous value to insure an intial printing.
      prev_dataBus = 250;
      prev_addressSwitch = 32000;
      prev_addressBus = 32000;
      Serial.print("\033[0m\033[?25l");   // Block cursor display: off.
      Serial.print("\033[H\033[2J");      // Cursor home and clear the screen.
      print_panel_serial();
      SERIAL_IO_VT100 = true;
      Serial.println("+ V, VT100 escapes are enabled.");
      break;
    // -------------------------------------
    case 'o':
      Serial.println("+ o, do not output to LED lights.");
      LED_IO = false;
      break;
    case 'O':
      Serial.println("+ O, output to LED lights.");
      LED_IO = true;
      break;
    //
    // -------------------------------------------------------------------
    // The following requires a VT100 terminal such as a Macbook terminal.
    // The following doesn't work on the Ardino monitor.
    // VT100 reference:
    //  http://ascii-table.com/ansi-escape-sequences-vt-100.php
    //
    // Esc[H  Move cursor to upper left corner, example: Serial.print("\033[H");
    // Esc[J  Clear screen from cursor down, example: Serial.print("\033[J");
    // Esc[2J  Clear entire screen, example: Serial.print("\033[H");
    // Example: Serial.print("\033[H\033[2J");  // Move home and clear entire screen.
    // Esc[nA  Move cursor up n lines.
    // Example: Serial.print("\033[3A");  // Cursor Up 3 lines.
    // Esc[nB  Move cursor down n lines.
    // Example: Serial.print("\033[6B");  // Cursor down 6 lines.
    // Esc[K  Clear line from cursor right
    //
    case 'z':
      Serial.print("++ Cursor off.");
      Serial.print("\033[0m\033[?25l");
      break;
    case 'Z':
      Serial.print("++ Cursor on.");
      Serial.print("\033[0m\033[?25h");
      break;
    case 'y':
      // Esc[2J  Clear entire screen, cursor stays where is.
      // Serial.print("\033[2J");
      // Rewrite the front panel LEDs and switches.
      Serial.print("\033[H");  // Move cursor home
      Serial.print("\033[1B");  // Cursor down
      // Print status and data lights.
      Serial.println(F(" .    *    .  .   .   .    .  *   .           .   .    *   .   .    .   *   ."));
      Serial.print("\033[1B");  // Cursor down
      // Print address lights.
      Serial.println(F(" *    .      .   .   .   .   .   .   .   .    .   .    .   .   *    .   .   *"));
      Serial.print("\033[1B");  // Cursor down
      // print address toggles.
      Serial.println(F("             v   v   v   v   v   v   v   v    v   v    v   v   ^    v   v   ^"));
      Serial.print("\033[2B");  // Cursor down
      break;
    // -------------------------------------
    case 'l':
      Serial.println("+ l, loaded a simple program.");
      cnt = 0;
      // MVI testing to the setting of registers.
      /*
        MWRITE(   cnt++, B00111110 & 0xff);  // ++ opcode:mvi:00111110:a:6
        MWRITE(   cnt++, B00000110 & 0xff);  // ++ immediate:6:6
        MWRITE(   cnt++, B00000110 & 0xff);  // ++ opcode:mvi:00000110:b:0
        MWRITE(   cnt++, B00000000 & 0xff);  // ++ immediate:0:0
        MWRITE(   cnt++, B00001110 & 0xff);  // ++ opcode:mvi:00001110:c:1
        MWRITE(   cnt++, B00000001 & 0xff);  // ++ immediate:1:1
        MWRITE(   cnt++, B00010110 & 0xff);  // ++ opcode:mvi:00010110:d:2
        MWRITE(   cnt++, B00000010 & 0xff);  // ++ immediate:2:2
        MWRITE(   cnt++, B00011110 & 0xff);  // ++ opcode:mvi:00011110:e:3
        MWRITE(   cnt++, B00000011 & 0xff);  // ++ immediate:3:3
        MWRITE(   cnt++, B00100110 & 0xff);  // ++ opcode:mvi:00100110:h:4
        MWRITE(   cnt++, B00000100 & 0xff);  // ++ immediate:4:4
        MWRITE(   cnt++, B00101110 & 0xff);  // ++ opcode:mvi:00101110:l:5
        MWRITE(   cnt++, B00000101 & 0xff);  // ++ immediate:5:5
        MWRITE(   cnt++, B01110110 & 0xff);  // ++ opcode:hlt:01110110
        MWRITE(   cnt++, B11000011 & 0xff);  // ++ opcode:jmp:11000011:Start
        MWRITE(   cnt++, B00000000 & 0xff);  // ++ lb:0
        MWRITE(   cnt++, B00000000 & 0xff);  // ++ hb:0
      */
      // Write to a memory location.
      /*
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
      */
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
      MWRITE( cnt++, B00111110 & 0xff);  // ++ opcode:mvi:00111110:a:235
      MWRITE( cnt++, B11101011 & 0xff);  // ++ immediate:235:235
      MWRITE( cnt++, B11100011 & 0xff);  // ++ opcode:out:11100011:37
      MWRITE( cnt++, B00100101 & 0xff);  // ++ immediate:37:37
      MWRITE( cnt++, B00111010 & 0xff);  // ++ opcode:lda:00111010:32
      MWRITE( cnt++, B00100000 & 0xff);  // ++ lb:32:32
      MWRITE( cnt++, B00000000 & 0xff);  // ++ hb:0
      MWRITE( cnt++, B11000011 & 0xff);  // ++ opcode:jmp:11000011:Start
      MWRITE( cnt++, B00000000 & 0xff);  // ++ lb:Start:0
      MWRITE( cnt++, B00000000 & 0xff);  // ++ hb:0
      //
      // Do EXAMINE 0  after the load;
      regPC = 0;
      p_regPC = ~regPC;
      altair_set_outputs(regPC, MREAD(regPC));
      if (SERIAL_IO_VT100) {
        print_panel_serial();
      }
      break;
    // -------------------------------------
    case '/':
      Serial.print(F("\r\nSet Addr switches to: "));
      // numsys_read_word(&addressSwitch);
      Serial.println('\n');
      break;
    case 'i':
      Serial.println("+ i: Information.");
      Serial.print(F("++ CPU: "));
      Serial.println(THIS_CPU);
      Serial.print(F("++ host_read_status_led_WAIT()="));
      Serial.println(host_read_status_led_WAIT());
      cpucore_i8080_print_registers();
      break;
    // -------------------------------------
    case 10:
    case 13:
      Serial.println(F("+ New line(Arduino IDE monitory) or carriage return(VT100) character."));
      print_panel_serial();
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
    if (Serial.available() > 0) {
      readByte = Serial.read();    // Read and process an incoming byte.
      processWaitSwitch(readByte);
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
  Serial.println("+++ Altair 101a initialized.");
  // ----------------------------------------------------
  // ----------------------------------------------------
  // Front panel LED lights.

  // System application status LED lights
  pinMode(WAIT_PIN, OUTPUT);    // Indicator: program wait state: LED on or LED off.
  pinMode(HLDA_PIN, OUTPUT);    // Indicator: clock process (LED on) or emulator (LED off).
  digitalWrite(WAIT_PIN, HIGH); // Default to wait state.
  digitalWrite(HLDA_PIN, HIGH); // Default to emulator.

  // ------------------------------
  // Set status lights.
  statusByte = host_set_status_leds_READMEM_M1();
  // programCounter and curProgramCounter are 0 by default.
  // dataByte = memoryData[curProgramCounter];
  Serial.println(F("+ Initialized: statusByte, programCounter & curProgramCounter, dataByte."));
  //
  pinMode(latchPinLed, OUTPUT);
  pinMode(clockPinLed, OUTPUT);
  pinMode(dataPinLed, OUTPUT);
  delay(300);
  ledFlashSuccess();
  Serial.println(F("+ Front panel LED lights are initialized."));
  //
  // ----------------------------------------------------
  // ----------------------------------------------------
  // host_set_status_leds_READMEM_M1();
  host_set_status_led_MEMR();
  host_set_status_led_M1();
  host_clr_status_led_WO();
  regPC = 0;
  opcode = MREAD(regPC);
  host_set_addr_leds(regPC);
  host_set_data_leds(opcode);
  print_panel_serial();
  programState = PROGRAM_WAIT;
  host_set_status_led_WAIT();
  // ----------------------------------------------------
  // programLights();    // Uses: statusByte, curProgramCounter, dataByte
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
  // delay(30); // Arduino sample code, doesn't use a delay.
}
// -----------------------------------------------------------------------------
