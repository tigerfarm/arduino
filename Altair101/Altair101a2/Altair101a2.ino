// -----------------------------------------------------------------------------
/*
  Serial Communications:
*/

#include "Altair101a.h"
#include "host_mega.h"
#include "mem.h"
#include "cpucore.h"
#include "config.h"

#define BIT(n) (1<<(n))

// -----------------------------------------------------------------------------

byte readByte = 0;

// Program states
#define LIGHTS_OFF 0
#define PROGRAM_WAIT 1
#define PROGRAM_RUN 2
#define CLOCK_RUN 3
#define PLAYER_RUN 4
#define SERIAL_DOWNLOAD 5
int programState = LIGHTS_OFF;  // Intial, default.

// -----------------------------------------------------------------------------
void print_panel_serial(bool force = false);

byte opcode = 0xff;

uint16_t cswitch = 0;
uint16_t dswitch = 0;

static uint16_t p_regPC = 0xFFFF;

word status_wait = false;

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
void altair_wait_reset()
{
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
  host_clr_status_led_PROT();
  // stacy altair_interrupt_disable();

  // wait while RESET switch is held
  // while ( host_read_function_switch_debounced(SW_RESET) );

  // if in single-step mode we need to set a flag so we know
  // to exit out of the currently executing instruction
  if ( host_read_status_led_WAIT() ) cswitch |= BIT(SW_RESET);
}

void altair_set_outputs(uint16_t a, byte v)
{
  host_set_addr_leds(a);
  host_set_data_leds(v);
  if ( mem_is_protected(a) )
    host_set_status_led_PROT();
  else
    host_clr_status_led_PROT();

  if ( host_read_status_led_M1() ) print_dbg_info();
  print_panel_serial();
}

bool altair_isreset()
{
  return (cswitch & BIT(SW_RESET)) == 0;
}

void altair_wait_step()
{
  cswitch &= BIT(SW_RESET); // clear everything but RESET status
  while ( host_read_status_led_WAIT() && (cswitch & (BIT(SW_STEP) | BIT(SW_SLOW) | BIT(SW_RESET))) == 0 )
  {
    // read_inputs();
    delay(10);
  }

  if ( cswitch & BIT(SW_SLOW) ) delay(500);
}

void print_dbg_info()
{
  // if ( config_serial_debug_enabled() && host_read_status_led_WAIT() && regPC != p_regPC )
  cpu_print_registers();
}

void altair_interrupt_disable()
{
  host_clr_status_led_INTE();
  // altair_interrupts_enabled = false;
  // altair_interrupts &= ~INT_DEVICE;
}

void altair_interrupt(uint32_t i, bool set)
{
}

// -----------------------------------------------------------------------------
void altair_hlt()
{
  host_set_status_led_HLTA();
  // in standalone mode it is hard to interact with the panel so for a HLT
  // instruction we just stop the CPU to avoid confusion
  regPC--;
  altair_interrupt(INT_SW_STOP);
}

void altair_out(byte port, byte data)
{
  host_set_addr_leds(port | port * 256);
  host_set_data_leds(data);
  host_set_status_led_OUT();
  host_set_status_led_WO();
  // stacy io_out(port, data);
  if ( host_read_status_led_WAIT() )
  {
    altair_set_outputs(port | port * 256, 0xff);
    altair_wait_step();
  }

  host_clr_status_led_OUT();
}

byte altair_in(byte port)
{
  byte data = 0;
  host_set_addr_leds(port | port * 256);
  if ( host_read_status_led_WAIT() )
  {
    cswitch &= BIT(SW_RESET); // clear everything but RESET status
    // keep reading input data while we are waiting
    while ( host_read_status_led_WAIT() && (cswitch & (BIT(SW_STEP) | BIT(SW_SLOW) | BIT(SW_RESET))) == 0 )
    {
      host_set_status_led_INP();
      // stacy data = io_inp(port);
      altair_set_outputs(port | port * 256, data);
      host_clr_status_led_INP();
      // stacy read_inputs();
      // advance simulation time (so timers can expire)
      // stacy TIMER_ADD_CYCLES(50);
    }
    if ( cswitch & BIT(SW_SLOW) ) delay(500);
  }
  else
  {
    host_set_status_led_INP();
    // stacy data = io_inp(port);
    host_set_data_leds(data);
    host_clr_status_led_INP();
  }
  return data;
}

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
void checkWaitButtons() {
}
void processData() {
  host_set_status_leds_READMEM_M1();
  host_set_addr_leds(regPC);
  opcode = MREAD(regPC);
  host_set_data_leds(opcode);
  regPC++;
  host_clr_status_led_M1();
  CPU_EXEC(opcode);           // defined: cpucore.h
}
// -----------------------------------------------------------------------------
void runProcessor() {
  Serial.println(F("+ runProcessor()"));
  //
  // put PC on address bus LEDs
  host_set_addr_leds(regPC);
  processData();
  programState = PROGRAM_RUN;
  while (programState == PROGRAM_RUN) {
    // processData();
    if (Serial.available() > 0) {
      // For now, require a serial character between instructions.
      // Read and process an incoming byte.
      readByte = Serial.read();
      // Program control: STOP or RESET.
      switch (readByte) {
        case 's':
          Serial.println(F("+ STOP"));
          programState = PROGRAM_WAIT;
          break;
      }
    }
  }
}
// -----------------------------------------------------------------------------
void setup() {

  // Speed for serial read, which matches the sending program.
  Serial.begin(9600);   // 115200 19200
  delay(2000);
  Serial.println(""); // Newline after garbage characters.
  Serial.println("+++ Setup.");
  Serial.println("+++ Altair 101a initialized.");
  print_panel_serial();
}

// -----------------------------------------------------------------------------
// Device Loop
int readByteCount = 0;
void loop() {

  if (Serial.available() > 0) {
    // Read and process an incoming byte.
    readByte = Serial.read();
    memoryData[readByteCount];
    readByteCount++;
    //
    // Set Address/Data toggles
    int data = readByte;
    if ( data >= '0' && data <= '9' ) {
      dswitch = dswitch ^ (1 << (data - '0'));
      print_panel_serial();
    }
    else if ( data >= 'a' && data <= 'f' ) {
      dswitch = dswitch ^ (1 << (data - 'a' + 10));
      print_panel_serial();
    } else {
      // Process command switches. Tested: EXAMINE, EXAMINE NEXT, DEPOSIT, DEPOSIT NEXT
      switch (readByte) {
        case 'x':
          regPC = dswitch;
          Serial.print("+ x, EXAMINE: ");
          Serial.println(regPC);
          p_regPC = ~regPC;
          altair_set_outputs(regPC, MREAD(regPC));
          break;
        case 'X':
          regPC = regPC + 1;
          Serial.print("+ x, EXAMINE NEXT: ");
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
        case 'l':
          MWRITE(0, B00111110 & 0xff);
          MWRITE(1, B00000110 & 0xff);
          MWRITE(2, B00110010 & 0xff);
          MWRITE(3, B01100000 & 0xff);
          MWRITE(4, B00000000 & 0xff);
          MWRITE(5, B01110110 & 0xff);
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
          Serial.println("+ l, loaded a simple program.");
          // Do EXAMINE 0 after the load;
          regPC = 0;
          p_regPC = ~regPC;
          altair_set_outputs(regPC, MREAD(regPC));
          break;
        case 'r':
          Serial.println("+ r, RUN.");
          host_clr_status_led_WAIT();
          runProcessor();
          break;
        case 'z':
          Serial.print("+ Print panel.");
          print_panel_serial();
          break;
        // -------------------------------------
        case '/':
          Serial.print(F("\r\nSet Addr switches to: "));
          // numsys_read_word(&dswitch);
          Serial.println('\n');
          break;
        case 'i':
          Serial.println("+ i: Information.");
          // numsys_toggle();
          // p_regPC = ~regPC;
          // print_dbg_info();
          // cpu_print_registers();
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
