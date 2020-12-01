// -----------------------------------------------------------------------------
// Altair 8800 Simulator
// Copyright (C) 2017 David Hansel
// -----------------------------------------------------------------------------

#include <Arduino.h>
#include "Altair101a.h"

// -----------------------------------------------------------------------------
// CPU,  Set to Intel 8080

#define PROC_I8080 0
#define cpu_opcodes cpucore_i8080_opcodes
#define cpu_clock_KHz()     CPU_CLOCK_I8080
#define cpu_get_processor() PROC_I8080
typedef void (*CPUFUN)();
extern CPUFUN cpu_opcodes[256];
#define CPU_EXEC(opcode) (cpu_opcodes[opcode])();

// -----------------------------------------------------------------------------
// Motherboard

// ----------------------------
// Mega
#if defined(__AVR_ATmega2560__)
#define THIS_CPU "Mega 2560"
#define MEMSIZE (2048)          // Mega2650 has 8k SRAM, Max, 6K: #define MEMSIZE (4096+2048)
//
#define USE_THROTTLE 0          // Set for slower Mega CPU.
#define MAX_TIMERS 9

// ----------------------------
// Due
#elif defined(__SAM3X8E__)
#define THIS_CPU "Due"
#define MEMSIZE 0x10000         // 64K
//
#define USE_THROTTLE 1          // Set for faster Due CPU.
#define MAX_TIMERS 13

#else
#define THIS_CPU "Other:Nano|Uno"
#error requires Arduino Mega2560 or Arduino Due
#define MEMSIZE (64)            // For Nano or Uno test: Global variables use 1935 bytes (23%) of dynamic memory
// Test on Nano or Uno.
//  Sketch uses 39902 bytes (123%) of program storage space. Maximum is 32256 bytes.
//  text section exceeds available space in board
//  Global variables use 1933 bytes (94%) of dynamic memory, leaving 115 bytes for local variables. Maximum is 2048 bytes.
//  Sketch too big; see http://www.arduino.cc/en/Guide/Troubleshooting#size for tips on reducing it.
//  Error compiling for board Arduino Uno.
#endif

// ---------------------------------
// Enable throttling of CPU speed.

#define CF_THROTTLE     0x01
#define CF_SERIAL_INPUT 0x08

#define NUM_SERIAL_DEVICES 4

extern uint32_t config_flags, config_flags2;
inline bool config_serial_input_enabled()     {
  return (config_flags & CF_SERIAL_INPUT) != 0;
}

// -----------------------------------------------------------------------------
// Memory, declare memory and memory read and write functions

extern byte Mem[MEMSIZE];

#define MWRITE(a,v) { Mem[a]=v; }

#define MREAD(a)    (Mem[a])
// WARNING: arguments to MEM_READ and MEM_WRITE macros should not have side effects
// (e.g. MEM_READ(addr++)) => any side effects will be executed multiple times!

// -----------------------------------------------------------------------------
// CPU Registers

void cpucore_i8080_print_registers();

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

void init_regS();

// -----------------------------------------------------------------------------
// Status bits and bytes

// Sync statusByteB bit values with Processor.ini for shifting out to 595's.
// Sync other status pins to match in Processor.ini.

extern const byte MEMR_ON;
extern const byte INP_ON;
extern const byte M1_ON;
extern const byte OUT_ON;
extern const byte HLTA_ON;
extern const byte STACK_ON;
extern const byte WO_ON;
extern const byte INT_ON;

inline void host_set_addr_leds(uint16_t v) {
  statusByteA = (v & 0xff); // lb where 0xff remove hb, leaving LB
  statusByteC = (v / 256);  // hb
}
#define host_set_data_leds(v)  statusByteL=(v)
#define host_read_data_leds()  statusByteL
#define host_read_addr_leds(v) (statusByteA | (statusByteC * 256))

#define host_read_status_led_WAIT()   status_wait
#define host_set_status_led_WAIT()  { digitalWrite(40, HIGH); status_wait = true; }
#define host_clr_status_led_WAIT()  { digitalWrite(40, LOW);  status_wait = false; }

#define host_read_status_led_HLDA()   status_hlda
#define host_set_status_led_HLDA()  { digitalWrite(41, HIGH); status_inte = true; }
#define host_clr_status_led_HLDA()  { digitalWrite(41, LOW);  status_inte = false; }

#define host_read_status_led_INTE()   status_inte
#define host_set_status_led_INTE()  { digitalWrite(38, HIGH); status_inte = true; }
#define host_clr_status_led_INTE()  { digitalWrite(38, LOW);  status_inte = false; }

#define host_set_status_led_MEMR()    statusByteB |=  MEMR_ON
#define host_set_status_led_INP()     statusByteB |=  INP_ON
#define host_set_status_led_M1()      statusByteB |=  M1_ON
#define host_set_status_led_OUT()     statusByteB |=  OUT_ON
#define host_set_status_led_HLTA()    statusByteB |=  HLTA_ON
#define host_set_status_led_STACK()   statusByteB |=  STACK_ON
#define host_set_status_led_WO()      statusByteB &= ~WO_ON       // Set turn WO off, reverse logic.
#define host_set_status_led_INT()     statusByteB |=  INT_ON

#define host_clr_status_led_MEMR()    statusByteB &= ~MEMR_ON
#define host_clr_status_led_INP()     statusByteB &= ~INP_ON
#define host_clr_status_led_M1()      statusByteB &= ~M1_ON
#define host_clr_status_led_OUT()     statusByteB &= ~OUT_ON
#define host_clr_status_led_HLTA()    statusByteB &= ~HLTA_ON
#define host_clr_status_led_STACK()   statusByteB &= ~STACK_ON
#define host_clr_status_led_WO()      statusByteB |=  WO_ON       // Set turn WO on, reverse logic.
#define host_clr_status_led_INT()     statusByteB &= ~INT_ON

#define host_set_status_leds_READMEM()       statusByteB |= MEMR_ON | WO_ON
#define host_set_status_leds_READMEM_M1()    statusByteB |= MEMR_ON | M1_ON | WO_ON
#define host_set_status_leds_READMEM_STACK() statusByteB |= MEMR_ON | STACK_ON | WO_ON
#define host_set_status_leds_WRITEMEM()      statusByteB &= ~(MEMR_ON | WO_ON)

inline byte host_mega_read_switches(byte highlow) {
  byte b = 0;
  return b;
}
#define host_read_sense_switches() host_mega_read_switches(true)
#define host_read_addr_switches() (host_mega_read_switches(true) * 256 | host_mega_read_switches(false))
#define host_check_interrupts() { if( Serial.available() ) serial_receive_host_data(0, Serial.read()); }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// numsys.h

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
// -----------------------------------------------------------------------------
// timer.h

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
// eof
