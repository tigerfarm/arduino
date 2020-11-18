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

// Note: To improve performance, the MEMR LED handling is a bit lazy while a program is
// running. Memory reads are by far the most common bus action and any tiny
// bit of time that can be cut here makes a significant performance difference.
// Setting USE_REAL_MREAD_TIMING to 1 will improve the accuracy of MREAD at the
// cost of performace. Leaving this at 0 has virtually no visible consequences
// apart from a slight difference in brightness of the MEMR LED while running.
// Setting it to 1 significantly reduces performance.
// Most users should keep this at 0

// If SHOW_MWRITE_OUTPUT enabled, the D0-7 LEDs will show values being output to the data bus
// during memory write operations). This is different from the original
// Altair behavior where the D0-7 LEDs were all on for write operations
// because the LEDs are wired to the DIN bus lines which are floating during
// CPU write). Additionally, enabling this makes sure that the "WO" LED will
// go out (negative logic) AFTER the address and data buses have been set to
// the proper values. It also changes timing of the "WO" LED similar to
// the USE_REAL_MREAD_TIMING setting.
// Enable this if you want to connect external hardware that needs to see
// data during memory write instructions or if you want to see the data
// during writes and do not care that this behavior does not match the original.

byte MEM_READ_STEP(uint16_t a);
void MEM_WRITE_STEP(uint16_t a, byte v);
#define USE_REAL_MREAD_TIMING 0
#if USE_REAL_MREAD_TIMING>0
inline byte MEM_READ(uint16_t a) {
  byte res;
  if ( host_read_status_led_WAIT() )
    res = MEM_READ_STEP(a);
  else
  {
    host_set_addr_leds(a);
    host_set_status_leds_READMEM();
    res = host_set_data_leds(MREAD(a));
    host_clr_status_led_MEMR();
  }
  return res;
}
#else
#define MEM_READ(a) ( host_read_status_led_WAIT() ? MEM_READ_STEP(a) : (host_set_status_leds_READMEM(),  host_set_addr_leds(a), host_set_data_leds(MREAD(a)) ))
#endif

// ----------------------------------------
#define MWRITE(a,v) { Mem[a]=v; }

#define SHOW_MWRITE_OUTPUT 0
#if SHOW_MWRITE_OUTPUT>0
inline void MEM_WRITE(uint16_t a, byte v) {
  if ( host_read_status_led_WAIT() )
    MEM_WRITE_STEP(a, v );
  else
  {
    host_set_addr_leds(a);
    host_set_data_leds(v);
    host_set_status_leds_WRITEMEM();
    MWRITE(a, v);
    host_clr_status_led_WO();
  }
}
#else
#define MEM_WRITE(a, v) if( host_read_status_led_WAIT() ) MEM_WRITE_STEP(a, v); else { host_set_status_leds_WRITEMEM(); host_set_addr_leds(a); host_set_data_leds(0xff); MWRITE(a, v); }
#endif

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
extern union unionHL{
  struct {
    byte L, H;
  };
  uint16_t HL;
} regHL;
extern union unionPC{
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
// -----------------------------------------------------------------------------
// host_mega.h included here rather than another file.
// -----------------------------------------------------------------------------
inline void host_set_addr_leds(uint16_t v) {
  PORTA = (v & 0xff);
  PORTC = (v / 256);
}
#define host_read_addr_leds(v) (PORTA | (PORTC * 256))
#define host_set_data_leds(v)  PORTL=(v)
#define host_read_data_leds()  PORTL

#define host_set_status_led_INT()     PORTB |=  0x01
#define host_set_status_led_WO()      PORTB &= ~0x02
#define host_set_status_led_STACK()   PORTB |=  0x04
#define host_set_status_led_HLTA()    PORTB |=  0x08
#define host_set_status_led_M1()      PORTB |=  0x20
#define host_set_status_led_MEMR()    PORTB |=  0x80
#define host_set_status_led_INTE()    digitalWrite(38, HIGH);
#define host_set_status_led_PROT()    digitalWrite(39, HIGH)
#define host_set_status_led_WAIT()  { digitalWrite(40, HIGH); status_wait = true; }
#define host_set_status_led_HLDA()    digitalWrite(41, HIGH)

#define host_clr_status_led_INT()     PORTB &= ~0x01
#define host_clr_status_led_WO()      PORTB |=  0x02
#define host_clr_status_led_STACK()   PORTB &= ~0x04
#define host_clr_status_led_HLTA()    PORTB &= ~0x08
#define host_clr_status_led_M1()      PORTB &= ~0x20
#define host_clr_status_led_MEMR()    PORTB &= ~0x80
#define host_clr_status_led_INTE()    digitalWrite(38, LOW);
#define host_clr_status_led_PROT()    digitalWrite(39, LOW)
#define host_clr_status_led_WAIT()  { digitalWrite(40, LOW); status_wait = false; }
#define host_clr_status_led_HLDA()    digitalWrite(41, LOW)

#define host_read_status_led_WAIT() status_wait
#define host_read_status_led_M1()   PORTB&0x20
#define host_read_status_led_INTE() PORTD&0x80
#define host_read_status_led_HLTA() PORTB&0x08

#define host_set_status_leds_READMEM()       PORTB |=  0x82
#define host_set_status_leds_READMEM_M1()    PORTB |=  0xA2;
#define host_set_status_leds_READMEM_STACK() PORTB |=  0x86;
#define host_set_status_leds_WRITEMEM()      PORTB &= ~0x82

#define host_set_status_led_INP()     PORTB |=  0x40
#define host_clr_status_led_INP()     PORTB &= ~0x40
#define host_set_status_led_OUT()     PORTB |=  0x10
#define host_clr_status_led_OUT()     PORTB &= ~0x10

uint16_t host_read_status_leds();
inline byte host_mega_read_switches(byte highlow) {
  byte b = 0;
  ADCSRB = highlow ? 0x08 : 0x00; // MUX5
  ADMUX = 0x40; ADCSRA = 0xD4; while ( ADCSRA & 0x40 ); if ( ADCH != 0 ) b |= 0x01;
  ADMUX = 0x41; ADCSRA = 0xD4; while ( ADCSRA & 0x40 ); if ( ADCH != 0 ) b |= 0x02;
  ADMUX = 0x42; ADCSRA = 0xD4; while ( ADCSRA & 0x40 ); if ( ADCH != 0 ) b |= 0x04;
  ADMUX = 0x43; ADCSRA = 0xD4; while ( ADCSRA & 0x40 ); if ( ADCH != 0 ) b |= 0x08;
  ADMUX = 0x44; ADCSRA = 0xD4; while ( ADCSRA & 0x40 ); if ( ADCH != 0 ) b |= 0x10;
  ADMUX = 0x45; ADCSRA = 0xD4; while ( ADCSRA & 0x40 ); if ( ADCH != 0 ) b |= 0x20;
  ADMUX = 0x46; ADCSRA = 0xD4; while ( ADCSRA & 0x40 ); if ( ADCH != 0 ) b |= 0x40;
  ADMUX = 0x47; ADCSRA = 0xD4; while ( ADCSRA & 0x40 ); if ( ADCH != 0 ) b |= 0x80;
  return b;
}
#define host_read_sense_switches() host_mega_read_switches(true)
#define host_read_addr_switches() (host_mega_read_switches(true) * 256 | host_mega_read_switches(false))
bool host_read_function_switch(byte inputNum);
#define host_check_interrupts() { if( Serial.available() ) serial_receive_host_data(0, Serial.read()); }

// -----------------------------------------------------------------------------
#elif defined(__SAM3X8E__)
#define THIS_CPU "Due"
// -----------------------------------------------------------------------------
// host_due.h included here rather than another file.
// -----------------------------------------------------------------------------
inline byte host_read_sense_switches() {
  // SW8...15  => PIOA, bits 12-15,17-20 (negative logic)
  word w = ~REG_PIOA_PDSR;
  return ((w & 0xF000) / (1<<12)) | ((w & 0x1E0000) / (1<<13));
}
uint16_t host_read_addr_switches();

#define host_set_status_led_INT()     REG_PIOB_SODR = 1<<25
#define host_set_status_led_WO()      REG_PIOC_CODR = 1<<28
#define host_set_status_led_STACK()   REG_PIOC_SODR = 1<<26
#define host_set_status_led_HLTA()    REG_PIOC_SODR = 1<<25
#define host_set_status_led_M1()      REG_PIOC_SODR = 1<<23
#define host_set_status_led_MEMR()    REG_PIOC_SODR = 1<<21
#define host_set_status_led_INTE()    REG_PIOD_SODR = 1<<8;
#define host_set_status_led_PROT()    REG_PIOB_SODR = 1<<27
#define host_set_status_led_WAIT()  { REG_PIOC_SODR = 1<<29; status_wait = true; }
#define host_set_status_led_HLDA()    REG_PIOB_SODR = 1<<26

#define host_clr_status_led_INT()     REG_PIOB_CODR = 1<<25
#define host_clr_status_led_WO()      REG_PIOC_SODR = 1<<28
#define host_clr_status_led_STACK()   REG_PIOC_CODR = 1<<26
#define host_clr_status_led_HLTA()    REG_PIOC_CODR = 1<<25
#define host_clr_status_led_M1()      REG_PIOC_CODR = 1<<23
#define host_clr_status_led_MEMR()    REG_PIOC_CODR = 1<<21
#define host_clr_status_led_INTE()    REG_PIOD_CODR = 1<<8;
#define host_clr_status_led_PROT()    REG_PIOB_CODR = 1<<27
#define host_clr_status_led_WAIT()  { REG_PIOC_CODR = 1<<29; status_wait = false; }
#define host_clr_status_led_HLDA()    REG_PIOB_CODR = 1<<26

#define host_read_status_led_WAIT()   status_wait
#define host_read_status_led_M1()     (REG_PIOC_PDSR & (1<<23))
#define host_read_status_led_HLTA()   (REG_PIOC_PDSR & (1<<25))
#define host_read_status_led_INTE()   status_inte
#define host_set_status_led_INP()     REG_PIOC_SODR = 1<<22;
#define host_clr_status_led_INP()     REG_PIOC_CODR = 1<<22;
#define host_set_status_led_OUT()     REG_PIOC_SODR = 1<<24
#define host_clr_status_led_OUT()     REG_PIOC_CODR = 1<<24
#define host_set_status_leds_READMEM()        REG_PIOC_SODR = 0x10200000
#define host_set_status_leds_READMEM_M1()     REG_PIOC_SODR = 0x10A00000
#define host_set_status_leds_READMEM_STACK()  REG_PIOC_SODR = 0x14200000
#define host_set_status_leds_WRITEMEM()       REG_PIOC_CODR = 0x10200000
uint16_t host_read_status_leds();

// ----------------------------------------------------- address bus
inline void host_set_addr_leds(uint16_t v) {
  // A0..7  => 34, 35, ..., 41 (PIOC, bits 2-9)
  // A8..15 => 51, 50, ..., 44 (PIOC, bits 12-19)
  REG_PIOC_ODSR = (v & 0x00ff) * 4 + (v & 0xff00) * 16;
}
uint16_t host_read_addr_leds();
// ---------------------------------------------------- data bus
#define host_set_data_leds(v) REG_PIOD_ODSR = v
byte host_read_data_leds();
// ---------------------------------------------------- interrupts
// On the Due we are using real interrupts so nothing needs o be done here
#define host_check_interrupts() while(0)
void host_serial_interrupts_pause();
void host_serial_interrupts_resume();

// -----------------------------------------------------------------------------
#else
#define THIS_CPU "Other:Nano|Uno"
// #error requires Arduino Mega2560, Arduino Due
// Test on Nano or Uno.
//  Sketch uses 39902 bytes (123%) of program storage space. Maximum is 32256 bytes.
//  text section exceeds available space in board
//  Global variables use 1933 bytes (94%) of dynamic memory, leaving 115 bytes for local variables. Maximum is 2048 bytes.
//  Sketch too big; see http://www.arduino.cc/en/Guide/Troubleshooting#size for tips on reducing it.
//  Error compiling for board Arduino Uno.
// -----------------------------------------------------------------------------
inline void host_set_addr_leds(uint16_t v) {
  // PORTA = (v & 0xff);
  // PORTC = (v / 256);
}
uint16_t host_read_addr_leds();
uint16_t host_set_addr_leds();
#define host_set_data_leds(v)  0
#define host_read_data_leds()  0

#define host_set_status_led_INT()     0
#define host_set_status_led_WO()      0
#define host_set_status_led_STACK()   0
#define host_set_status_led_HLTA()    0
#define host_set_status_led_M1()      0
#define host_set_status_led_MEMR()    0
#define host_set_status_led_INTE()    0
#define host_set_status_led_PROT()    0
#define host_set_status_led_WAIT()  { status_wait = true; }
#define host_set_status_led_HLDA()    0

#define host_clr_status_led_INT()     0
#define host_clr_status_led_WO()      0
#define host_clr_status_led_STACK()   0
#define host_clr_status_led_HLTA()    0
#define host_clr_status_led_M1()      0
#define host_clr_status_led_MEMR()    0
#define host_clr_status_led_INTE()    0
#define host_clr_status_led_PROT()    0
#define host_clr_status_led_WAIT()  { status_wait = false; }
#define host_clr_status_led_HLDA()    0

#define host_read_status_led_WAIT() status_wait
#define host_read_status_led_M1()   0
#define host_read_status_led_INTE() 0
#define host_read_status_led_HLTA() 0

#define host_set_status_leds_READMEM()       0
#define host_set_status_leds_READMEM_M1()    0
#define host_set_status_leds_READMEM_STACK() 0
#define host_set_status_leds_WRITEMEM()      0

#define host_set_status_led_INP()     0
#define host_clr_status_led_INP()     0
#define host_set_status_led_OUT()     0
#define host_clr_status_led_OUT()     0

uint16_t host_read_status_leds();
inline byte host_mega_read_switches(byte highlow) {
  byte b = 0;
  ADCSRB = highlow ? 0x08 : 0x00; // MUX5
  ADMUX = 0x40; ADCSRA = 0xD4; while ( ADCSRA & 0x40 ); if ( ADCH != 0 ) b |= 0x01;
  ADMUX = 0x41; ADCSRA = 0xD4; while ( ADCSRA & 0x40 ); if ( ADCH != 0 ) b |= 0x02;
  ADMUX = 0x42; ADCSRA = 0xD4; while ( ADCSRA & 0x40 ); if ( ADCH != 0 ) b |= 0x04;
  ADMUX = 0x43; ADCSRA = 0xD4; while ( ADCSRA & 0x40 ); if ( ADCH != 0 ) b |= 0x08;
  ADMUX = 0x44; ADCSRA = 0xD4; while ( ADCSRA & 0x40 ); if ( ADCH != 0 ) b |= 0x10;
  ADMUX = 0x45; ADCSRA = 0xD4; while ( ADCSRA & 0x40 ); if ( ADCH != 0 ) b |= 0x20;
  ADMUX = 0x46; ADCSRA = 0xD4; while ( ADCSRA & 0x40 ); if ( ADCH != 0 ) b |= 0x40;
  ADMUX = 0x47; ADCSRA = 0xD4; while ( ADCSRA & 0x40 ); if ( ADCH != 0 ) b |= 0x80;
  return b;
}
#define host_read_sense_switches() host_mega_read_switches(true)
#define host_read_addr_switches() (host_mega_read_switches(true) * 256 | host_mega_read_switches(false))
bool host_read_function_switch(byte inputNum);
#define host_check_interrupts() { if( Serial.available() ) serial_receive_host_data(0, Serial.read()); }

// -----------------------------------------------------------------------------
// handling front panel switches
bool     host_read_function_switch(byte i);
bool     host_read_function_switch_debounced(byte i);
bool     host_read_function_switch_edge(byte i);
uint16_t host_read_function_switches_edge();
void     host_reset_function_switch_state();

// at a minimum the host must provide persistent storage memory to which
// we can write and read, implementing our own mini-filesystem
bool host_storage_init(bool write);
void host_storage_close();
void host_storage_write(const void *data, uint32_t addr, uint32_t len);
void host_storage_read(void *data,  uint32_t addr, uint32_t len);
void host_storage_move(uint32_t to, uint32_t from, uint32_t len);
void host_storage_invalidate();

// miscellaneous
void host_copy_flash_to_ram(void *dst, const void *src, uint32_t len);
uint32_t host_get_random();
void host_system_info();
void host_setup();

#endif  // CPU: Nano

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
