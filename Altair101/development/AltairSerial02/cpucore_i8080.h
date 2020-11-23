// -----------------------------------------------------------------------------
// Altair 8800 Simulator
// Copyright (C) 2017 David Hansel
// -----------------------------------------------------------------------------

#include <Arduino.h>
#include "Altair101a.h"

void cpucore_i8080_print_registers();

// -----------------------------------------------------------------------------
// mem.h
// Declare memory and memory read and write functions
// -----------------------------------------------------------------------------

// Mega2650: 8k SRAM, use 6k for emulated RAM
// Make sure Arduino IDE says (after compiling) that AT LEAST 310 bytes of RAM are left for local variables!
// #define MEMSIZE (64)            // For Nano or Uno test: Global variables use 1935 bytes (23%) of dynamic memory
// #define MEMSIZE (4096+2048)
#define MEMSIZE (2048)              // Mega testing.

extern byte Mem[MEMSIZE];

// ----------------------------------------
#define MREAD(a)    (Mem[a])
// WARNING: arguments to MEM_READ and MEM_WRITE macros should not have side effects
// (e.g. MEM_READ(addr++)) => any side effects will be executed multiple times!

// If SHOW_MWRITE_OUTPUT enabled, the D0-7 LEDs will show values being output to the data bus
// during memory write operations). This is different from the original
// Altair behavior where the D0-7 LEDs were all on for write operations
// because the LEDs are wired to the DIN bus lines which are floating during
// CPU write). Additionally, enabling this makes sure that the "WO" LED will
// go out (negative logic) AFTER the address and data buses have been set to
// the proper values.

byte MEM_READ_STEP(uint16_t a);
void MEM_WRITE_STEP(uint16_t a, byte v);

#define MEM_READ(a) (host_read_status_led_WAIT() ? MEM_READ_STEP(a) : (host_set_status_leds_READMEM(),  host_set_addr_leds(a), host_set_data_leds(MREAD(a)) ))

// ----------------------------------------
#define MWRITE(a,v) { Mem[a]=v; }

/*
#define SHOW_MWRITE_OUTPUT 1
#if SHOW_MWRITE_OUTPUT>0
// #define MEM_WRITE(a, v) if( host_read_status_led_WAIT() ) MEM_WRITE_STEP(a, v); else { host_set_addr_leds(a); host_set_data_leds(v); host_set_status_leds_WRITEMEM(); MWRITE(a, v); host_clr_status_led_WO();}
#define MEM_WRITE(a, v) if( host_read_status_led_WAIT() ) { host_set_addr_leds(a); host_set_data_leds(v); host_set_status_leds_WRITEMEM(); MWRITE(a, v); singleStepWait();} else MWRITE(a, v);
#else
#define MEM_WRITE(a, v) if( host_read_status_led_WAIT() ) MEM_WRITE_STEP(a, v); else { host_set_status_leds_WRITEMEM(); host_set_addr_leds(a); host_set_data_leds(0xff); MWRITE(a, v); }
#endif
*/
// -----------------------------------------------------------------------------
// cpucore.h
// -----------------------------------------------------------------------------

#define PS_CARRY       0x01
#define PS_PARITY      0x04
#define PS_HALFCARRY   0x10
#define PS_ZERO        0x40
#define PS_SIGN        0x80

// The emulater runs at 2MHz
#define CPU_CLOCK_I8080 2000

extern union unionAF {
  struct {
    byte A, F;
  };
  uint16_t AF;
} regAF;
extern union unionBC {
  struct {
    byte C, B;
  };
  uint16_t BC;
} regBC;
extern union unionDE {
  struct {
    byte E, D;
  };
  uint16_t DE;
} regDE;
extern union unionHL {
  struct {
    byte L, H;
  };
  uint16_t HL;
} regHL;
extern union unionPC {
  struct {
    byte L, H;
  };
  uint16_t PC;
} regPCU;

extern uint16_t regSP;

#define regA  regAF.A
#define regS  regAF.F
#define regB  regBC.B
#define regC  regBC.C
#define regD  regDE.D
#define regE  regDE.E
#define regH  regHL.H
#define regL  regHL.L
#define regPC regPCU.PC

#define PROC_I8080 0

// fixed I8080 CPU
#define cpu_opcodes cpucore_i8080_opcodes
#define cpu_clock_KHz()     CPU_CLOCK_I8080
#define cpu_get_processor() PROC_I8080

typedef void (*CPUFUN)();
extern CPUFUN cpu_opcodes[256];
#define CPU_EXEC(opcode) (cpu_opcodes[opcode])();

// -----------------------------------------------------------------------------
// host.h
// -----------------------------------------------------------------------------

#if defined(__AVR_ATmega2560__)
#define THIS_CPU "Mega 2560"
#elif defined(__SAM3X8E__)
#define THIS_CPU "Due"
#else
#define THIS_CPU "Other:Nano|Uno"
#error requires Arduino Mega2560 or Arduino Due
// Test on Nano or Uno.
//  Sketch uses 39902 bytes (123%) of program storage space. Maximum is 32256 bytes.
//  text section exceeds available space in board
//  Global variables use 1933 bytes (94%) of dynamic memory, leaving 115 bytes for local variables. Maximum is 2048 bytes.
//  Sketch too big; see http://www.arduino.cc/en/Guide/Troubleshooting#size for tips on reducing it.
//  Error compiling for board Arduino Uno.
// -----------------------------------------------------------------------------
#endif  // CPU: Nano|Uno

// -----------------------------------------------------------------------------
// host_mega.h included here rather than another file
// -----------------------------------------------------------------------------
//
// The original was using hardware specific PORT values.
//    I've removed them in favor of memory bytes which are not hardware specific.
// Microcontroller Port Registers:
// Port registers allow for lower-level and faster manipulation of the i/o pins.
// https://www.arduino.cc/en/Reference/PortManipulation
//    D PORTD maps to digital pins 0 to 7.
//    B PORTB maps to digital pins 8 to 13.
//    C PORTC maps to analog input pins 0 to 5.
// Requires including Arduino.h.
//    A statusByteA maps to digital pins 22 to 29 (physical pins).
//                  Digital pin 50 (MISO), Digital pin 10, 11, 12, 13 (PWM)?
//                  Serial.println(bitRead(statusByteA.IN, 0)); // print the state of pin 2 (statusByteA, bit 0)
// https://forum.arduino.cc/index.php?topic=52534.0
//

inline void host_set_addr_leds(uint16_t v) {
  statusByteA = (v & 0xff); // 0xff : Turn ON, 0x00 : Turn OFF
  statusByteC = (v / 256);
}
#define host_set_data_leds(v)  statusByteL=(v)
#define host_read_data_leds()  statusByteL
#define host_read_addr_leds(v) (statusByteA | (statusByteC * 256))

// uint16_t host_read_status_leds();

#define host_read_status_led_WAIT()   status_wait
#define host_set_status_led_WAIT()  { /* digitalWrite(40, HIGH);*/ status_wait = true; }
#define host_clr_status_led_WAIT()  { /* digitalWrite(40, LOW); */ status_wait = false; }
#define host_set_status_led_HLDA()    digitalWrite(41, HIGH)
#define host_clr_status_led_HLDA()    digitalWrite(41, LOW)

#define host_read_status_led_HLTA()   statusByteB&0x08
#define host_read_status_led_M1()     statusByteB&0x20
#define host_read_status_led_INTE()   statusByteD&0x80

// #define host_set_status_led_INTE()    digitalWrite(38, HIGH);
#define host_set_status_led_MEMR()    statusByteB |=  0x80
#define host_set_status_led_INP()     statusByteB |=  0x40
#define host_set_status_led_M1()      statusByteB |=  0x20
#define host_set_status_led_OUT()     statusByteB |=  0x10
#define host_set_status_led_HLTA()    statusByteB |=  0x08
#define host_set_status_led_STACK()   statusByteB |=  0x04
#define host_set_status_led_WO()      statusByteB &= ~0x02
#define host_set_status_led_INT()     statusByteB |=  0x01

// #define host_clr_status_led_INTE()    digitalWrite(38, LOW);
#define host_clr_status_led_MEMR()    statusByteB &= ~0x80
#define host_clr_status_led_INP()     statusByteB &= ~0x40
#define host_clr_status_led_M1()      statusByteB &= ~0x20
#define host_clr_status_led_OUT()     statusByteB &= ~0x10
#define host_clr_status_led_HLTA()    statusByteB &= ~0x08
#define host_clr_status_led_STACK()   statusByteB &= ~0x04
#define host_clr_status_led_WO()      statusByteB |=  0x02
#define host_clr_status_led_INT()     statusByteB &= ~0x01

#define host_set_status_leds_READMEM()       statusByteB |=  0x82
#define host_set_status_leds_READMEM_M1()    statusByteB |=  0xA2;
#define host_set_status_leds_READMEM_STACK() statusByteB |=  0x86;
#define host_set_status_leds_WRITEMEM()      statusByteB &= ~0x82

inline byte host_mega_read_switches(byte highlow) {
  byte b = 0;
  return b;
}
#define host_read_sense_switches() host_mega_read_switches(true)
#define host_read_addr_switches() (host_mega_read_switches(true) * 256 | host_mega_read_switches(false))
bool host_read_function_switch(byte inputNum);
#define host_check_interrupts() { if( Serial.available() ) serial_receive_host_data(0, Serial.read()); }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// config.h
// -----------------------------------------------------------------------------

// Enables throttling of CPU speed.
// #define USE_THROTTLE 1        // Set for faster Due CPU.
#define USE_THROTTLE 0        // Set for slower Mega CPU.

#define CF_THROTTLE     0x01
#define CF_SERIAL_INPUT 0x08

#define NUM_SERIAL_DEVICES 4

extern uint32_t config_flags, config_flags2;
inline bool config_serial_input_enabled()     {
  return (config_flags & CF_SERIAL_INPUT) != 0;
}

// -----------------------------------------------------------------------------
// numsys.h
// -----------------------------------------------------------------------------

#ifndef NUMSYS_H
#define NUMSYS_H

#define NUMSYS_OCT 0
#define NUMSYS_DEC 1
#define NUMSYS_HEX 2

void     numsys_set(byte sys);
byte     numsys_get();
byte     numsys_get_byte_length();
void     numsys_toggle();
void     numsys_print_word(uint16_t w);
void     numsys_print_byte(byte b);
void     numsys_print_byte_bin(byte b);
void     numsys_print_byte_oct(byte b);
void     numsys_print_byte_dec(byte b);
void     numsys_print_byte_hex(byte b);
void     numsys_print_mem(uint16_t addr, byte num, bool printBrackets);
bool     numsys_read_byte(byte *w);
bool     numsys_read_word(uint16_t *w);
bool     numsys_read_dword(uint32_t *w);
uint32_t numsys_read_dword(bool *ESC = NULL);
byte     numsys_read_hex_byte();
uint16_t numsys_read_hex_word();

#endif

// -----------------------------------------------------------------------------
// timer.h
// -----------------------------------------------------------------------------

extern uint32_t timer_cycle_counter, timer_cycle_counter_offset, timer_next_expire_cycles;

typedef void (*TimerFnTp)();
void timer_setup(byte tid, uint32_t microseconds, TimerFnTp timer_fn);
void timer_start(byte tid, uint32_t microseconds = 0, bool recurring = false);
void timer_stop(byte tid);
bool timer_running(byte tid);
uint32_t timer_get_period(byte tid);
void timer_check();
void timer_setup();

#define timer_get_cycles() (timer_cycle_counter_offset+timer_cycle_counter)

#define TIMER_ADD_CYCLES(n) if( (timer_cycle_counter+=(n)) >= timer_next_expire_cycles ) timer_check()

// -----------------------------------------------------------------------------
