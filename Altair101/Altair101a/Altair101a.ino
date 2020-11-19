// -----------------------------------------------------------------------------
/*
  Processor
  + Using David Hansel's Altair 8800 Simulator code to process machine code instructions.
  + Interactivity is over the Arduino IDE monitor USB serial port.

  Next:
  + WAIT mode, WAIT light,
  ?- altair_wait_step();  // If WAIT mode, loop until getting another serial character or switch input.
  ?- altair_wait_reset();
  ?- altair_isreset();
*/
// -----------------------------------------------------------------------------
#include "Altair101a.h"
#include "cpucore_i8080.h"

// For Byte bit comparisons.
#define BIT(n) (1<<(n))

// Byte bit switch values
#define SW_RUN        0
#define SW_STOP       1
#define SW_STEP       2
#define SW_SLOW       3
#define SW_EXAMINE    4
#define SW_EXNEXT     5
#define SW_DEPOSIT    6
#define SW_DEPNEXT    7
#define SW_RESET      8
#define SW_CLR        9
#define SW_PROTECT   10
#define SW_UNPROTECT 11
#define SW_AUX1UP    12
#define SW_AUX1DOWN  13
#define SW_AUX2UP    14
#define SW_AUX2DOWN  15

// Byte bit status values
#define ST_INT     0x0001
#define ST_WO      0x0002
#define ST_STACK   0x0004
#define ST_HLTA    0x0008
#define ST_OUT     0x0010
#define ST_M1      0x0020
#define ST_INP     0x0040
#define ST_MEMR    0x0080
#define ST_PROT    0x0100
#define ST_INTE    0x0200
#define ST_HLDA    0x0400
#define ST_WAIT    0x0800

// Not implemented at this time.
#define INT_SW_STOP     0x80000000
#define INT_SW_RESET    0x40000000
#define INT_SW_CLR      0x20000000
#define INT_SW_AUX2UP   0x10000000
#define INT_SW_AUX2DOWN 0x08000000
#define INT_SWITCH      0xff000000

// -----------------------------------------------------------------------------
// From Porcessor.ino

#define LOG_MESSAGES 1    // For debugging.

byte readByte = 0;

// Program states
#define LIGHTS_OFF 0
#define PROGRAM_WAIT 1
#define PROGRAM_RUN 2
#define CLOCK_RUN 3
#define PLAYER_RUN 4
#define SERIAL_DOWNLOAD 5
int programState = LIGHTS_OFF;  // Intial, default.

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
void print_panel_serial(bool force = false);

byte opcode = 0xff;

static uint16_t p_regPC = 0xFFFF;
uint16_t cswitch = 0;
uint16_t dswitch = 0;

// -----------------------------------------------------------------------------
uint16_t host_read_status_leds() {
  //  This works for Mega, not Due.
  uint16_t res;
  res = PORTB;
  res |= PORTD & 0x80 ? ST_INTE : 0;
  res |= PORTG & 0x04 ? ST_PROT : 0;
  res |= PORTG & 0x02 ? ST_WAIT : 0;
  res |= PORTG & 0x01 ? ST_HLDA : 0;
  return res;
}

// -----------------------------------------------------------------------------
void altair_set_outputs(uint16_t a, byte v) {
  // Stacy, When not using serial, display on front panel lights.
  host_set_addr_leds(a);
  host_set_data_leds(v);
}

void altair_out(byte port, byte data) {
  // Opcode: out <port>
  // Called from: cpu_OUT()
  host_set_addr_leds(port | port * 256);
  host_set_data_leds(data);
  host_set_status_led_OUT();
  host_set_status_led_WO();
  //
  // stacy io_out(port, data);
  //
  // Actual output of bytes. Example output a byte to the serial port (IDE monitor).
  //
  if ( host_read_status_led_WAIT() ) {
    // If single stepping, need to wait.
    altair_set_outputs(port | port * 256, 0xff);
    altair_wait_step();
  }
  host_clr_status_led_OUT();
  host_clr_status_led_WO();
}

void altair_wait_step() {
  //
  // Stacy, If WAIT mode, return to WAIT loop?
  // Also used in: MEM_READ_STEP(...) and MEM_WRITE_STEP(...).
  //
  cswitch &= BIT(SW_RESET); // clear everything but RESET status
  /* Stacy, here is the loop for waiting when single stepping during WAIT mode.
    while ( host_read_status_led_WAIT() && (cswitch & (BIT(SW_STEP) | BIT(SW_SLOW) | BIT(SW_RESET))) == 0 ) {
    read_inputs();
    delay(10);
    }
  */
  if ( cswitch & BIT(SW_SLOW) ) delay(500);
}


// -----------------------------------------------------------------------------
void read_inputs() {
  byte readByte;
  readByte = "";
  // cswitch = 0;
  // ---------------------------
  // Device read options.
  // read_inputs_panel();
  //
  // dave, implement:
  if ( config_serial_input_enabled() ) {
    read_inputs_serial();
  }
  // ---------------------------
  if (readByte != "") {
    // processWaitSwitch(readByte);
  }
}
void read_inputs_panel() {
  // we react on positive edges on the function switches...
  // cswitch = host_read_function_switches_edge();
  // ...except for the SLOW switch which is active as long as it is held down
  // if ( host_read_function_switch_debounced(SW_SLOW) ) cswitch |= BIT(SW_SLOW);
  // #if STANDALONE==0
  //   dswitch = host_read_addr_switches();
  // #endif
}
void read_inputs_serial() {
  return;
  if (Serial.available() > 0) {
    readByte = Serial.read();    // Read and process an incoming byte.
    processRunSwitch(readByte);
  }
}

byte altair_in(byte port) {
  // Opcode: out <port>
  // cpu_OUT()
  byte data = 0;
  host_set_addr_leds(port | port * 256);
  if ( host_read_status_led_WAIT() ) {
    cswitch &= BIT(SW_RESET); // clear everything but RESET status
    // keep reading input data while we are waiting
    while ( host_read_status_led_WAIT() && (cswitch & (BIT(SW_STEP) | BIT(SW_SLOW) | BIT(SW_RESET))) == 0 ) {
      host_set_status_led_INP();
      // stacy data = io_inp(port);
      altair_set_outputs(port | port * 256, data);
      host_clr_status_led_INP();
      // stacy read_inputs();
      // advance simulation time (so timers can expire)
      // stacy TIMER_ADD_CYCLES(50);
    }
    // if ( cswitch & BIT(SW_SLOW) ) delay(500);
  }
  else {
    host_set_status_led_INP();
    // stacy data = io_inp(port);
    host_set_data_leds(data);
    host_clr_status_led_INP();
  }
  return data;
}

// -----------------------------------------------------------------------------
bool altair_isreset() {
  return (cswitch & BIT(SW_RESET)) == 0;
}

void altair_wait_reset() {
  // prevent printing processor status now (will print after reset)
  p_regPC = regPC;
  // set bus/data LEDs on, status LEDs off
  altair_set_outputs(0xffff, 0xff);
  host_clr_status_led_INT();
  host_set_status_led_WO();
  host_clr_status_led_STACK();
  host_clr_status_led_HLTA();
  host_clr_status_led_OUT();
  host_clr_status_led_M1();
  host_clr_status_led_INP();
  host_clr_status_led_MEMR();
  // host_clr_status_led_PROT();
  // stacy altair_interrupt_disable();

  // wait while RESET switch is held
  // while ( host_read_function_switch_debounced(SW_RESET) );

  // if in single-step mode we need to set a flag so we know
  // to exit out of the currently executing instruction
  if ( host_read_status_led_WAIT() ) cswitch |= BIT(SW_RESET);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void print_panel_serial(bool force)
{
  byte dbus;
  static uint16_t p_dswitch = 0, p_cswitch = 0, p_abus = 0xffff, p_dbus = 0xffff, p_status = 0xffff;
  uint16_t status, abus;

  status = host_read_status_leds();
  abus   = host_read_addr_leds();
  dbus   = host_read_data_leds();

  // Even if n change, print anyway.
  // if ( force || p_cswitch != cswitch || p_dswitch != dswitch || p_abus != abus || p_dbus != dbus || p_status != status ) {

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
  // }
  Serial.println("+ Ready to receive command.");
}

// -----------------------------------------------------------------------------
void altair_hlt() {
  host_set_status_led_HLTA();
  // regPC--;
  // altair_interrupt(INT_SW_STOP);
  programState = PROGRAM_WAIT;
  Serial.println(F("++ HALT"));
  if (!host_read_status_led_WAIT()) {
    print_panel_serial();
  }
  host_set_status_led_WAIT();
}

void processDataOpcode() {
#ifdef LOG_MESSAGES
  Serial.print(F("++ regPC:"));
  Serial.print(regPC);
  Serial.print(F(": data:"));
  printData(MREAD(regPC));
  Serial.println("");
#endif
  //
  host_set_status_leds_READMEM_M1();
  host_set_addr_leds(regPC);
  opcode = MREAD(regPC);
  host_set_data_leds(opcode);
  regPC++;
  //
  host_clr_status_led_M1();
  CPU_EXEC(opcode);
  host_set_status_led_MEMR();
  host_set_status_led_M1();
  host_clr_status_led_WO();
}

void processRunSwitch(byte readByte) {
  switch (readByte) {
    case 's':
      Serial.println(F("+ STOP"));
      programState = PROGRAM_WAIT;
      break;
    case 'R':
      Serial.println(F("+ RESET"));
    // Stacy need to implement.
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
void processWaitSwitch(byte readByte) {
  //
  // Process an address/data toggle.
  //
  int data = readByte;
  if ( data >= '0' && data <= '9' ) {
    dswitch = dswitch ^ (1 << (data - '0'));
  }
  else if ( data >= 'a' && data <= 'f' ) {
    // Stacy, change to uppercase A...F
    dswitch = dswitch ^ (1 << (data - 'a' + 10));
  } else {
    //
    // Process command switches. Tested: RUN, EXAMINE, EXAMINE NEXT, DEPOSIT, DEPOSIT NEXT
    //
    switch (readByte) {
      case 'r':
        Serial.println("+ r, RUN.");
        host_clr_status_led_WAIT();
        runProcessor();
        break;
      case 's':
        Serial.println("+ s, SINGLE STEP: ");
        processDataOpcode();
        break;
      case 'x':
        regPC = dswitch;
        Serial.print("+ x, EXAMINE: ");
        Serial.println(regPC);
        p_regPC = ~regPC;
        altair_set_outputs(regPC, MREAD(regPC));
        break;
      case 'X':
      case 'y':
        regPC = regPC + 1;
        Serial.print("+ y, EXAMINE NEXT: ");
        Serial.println(regPC);
        altair_set_outputs(regPC, MREAD(regPC));
        break;
      case 'p':
        Serial.print("+ p, DEPOSIT to: ");
        Serial.println(regPC);
        MWRITE(regPC, dswitch & 0xff);
        altair_set_outputs(regPC, MREAD(regPC));
        break;
      case 'P':
        regPC++;
        Serial.print("+ P, DEPOSIT NEXT to: ");
        Serial.println(regPC);
        MWRITE(regPC, dswitch & 0xff);
        altair_set_outputs(regPC, MREAD(regPC));
        break;
      case 'R':
        Serial.println("+ R, RESET.");
        // altair_wait_reset();
        // For now, do EXAMINE 0 to reset to the first memory address.
        regPC = 0;
        p_regPC = ~regPC;
        altair_set_outputs(regPC, MREAD(regPC));
        break;
      // -------------------------------------
      case 'h':
        Serial.println("----------------------------------------------------");
        Serial.println("+ h, Help.");
        Serial.println("-------------");
        Serial.println("+ r, RUN.");
        Serial.println("+ s, SINGLE STEP when in WAIT mode.");
        Serial.println("+ s, STOP        when in RUN mode.");
        Serial.println("+ x, EXAMINE switch address.");
        Serial.println("+ X, EXAMINE NEXT address, current address + 1.");
        Serial.println("+ p, DEPOSIT at current address");
        Serial.println("+ P, DEPOSIT NEXT address, current address + 1");
        Serial.println("+ R, RESET, set address to zero.");
        Serial.println("-------------");
        Serial.println("+ l, loaded a simple program.");
        Serial.println("----------------------------------------------------");
        break;
      // -------------------------------------
      case 'l':
        Serial.println("+ l, loaded a simple program.");
        MWRITE(0, B00111110 & 0xff);
        MWRITE(1, B00000110 & 0xff);
        MWRITE(2, B00110010 & 0xff);
        MWRITE(3, B01100000 & 0xff);
        MWRITE(4, B00000000 & 0xff);
        MWRITE(5, B01110110 & 0xff);
        MWRITE(6, B11000011 & 0xff);  // JMP back to start.
        MWRITE(7, B00000000 & 0xff);
        MWRITE(8, B00000000 & 0xff);
        /*
          Program listing:
          Address(lb):databyte :hex:oct > description
          00000000:   00111110 : 3E:076 > opcode: mvi a,6
          00000001:   00000110 : 06:006 > immediate: 6 : 6
          00000010:   00110010 : 32:062 > opcode: sta 96
          00000011:   01100000 : 60:140 > lb: 96
          00000100:   00000000 : 00:000 > hb: 0
          00000101:   01110110 : 76:166 > opcode: hlt
        */
        // Do EXAMINE 0 after the load;
        regPC = 0;
        p_regPC = ~regPC;
        altair_set_outputs(regPC, MREAD(regPC));
        break;
      // -------------------------------------
      case '/':
        Serial.print(F("\r\nSet Addr switches to: "));
        // numsys_read_word(&dswitch);
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
        // New line character.
        print_panel_serial();
        break;
      // -------------------------------------
      default:
        Serial.print("- Ignored <");
        Serial.write(readByte);
        Serial.println(">");
    }
  }
}

void runProcessorWait() {
  Serial.println(F("+ runProcessorWait()"));
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
  //
  Serial.println("+++ Setup.");
  Serial.println("+++ Altair 101a initialized.");
  //
  programState = PROGRAM_WAIT;
  host_set_status_led_WAIT();
  host_read_status_led_WAIT();
  //
  // host_set_status_leds_READMEM_M1();
  host_set_status_led_MEMR();
  host_set_status_led_M1();
  host_clr_status_led_WO();
  opcode = MREAD(regPC);
  host_set_data_leds(opcode);
  //
  print_panel_serial();
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
