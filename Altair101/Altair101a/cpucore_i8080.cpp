// -----------------------------------------------------------------------------
// Altair 8800 Simulator
// Copyright (C) 2017 David Hansel
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
// -----------------------------------------------------------------------------

#include <Arduino.h>
#include "cpucore_i8080.h"

word status_wait = 1;
//
// For Processor.ino
// Replace PORTX with statusByteX.
byte statusByteA = B00000000;        // By default, all are OFF.
byte statusByteB = B00000000;
byte statusByteC = B00000000;
byte statusByteD = B00000000;
byte statusByteG = B00000000;
byte statusByteL = B00000000;
byte dataByte    = B00000000;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Altair8800.ino

void altair_interrupt(uint32_t i, bool set = true);
bool altair_interrupt_active(uint32_t i);
void altair_interrupt_enable();
void altair_interrupt_disable();
bool altair_interrupt_enabled();

void altair_interrupt_disable() {
  // host_clr_status_led_INTE();
  // altair_interrupts_enabled = false;
  // altair_interrupts &= ~INT_DEVICE;
}

void altair_interrupt(uint32_t i, bool set) {}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// mem.cpp

byte Mem[MEMSIZE];

// -----------------------------------------
// Read

// dave
byte MEM_READ(uint16_t a) {
  byte returnByte;
  if (status_wait) {
    returnByte = MEM_READ_THIS(a);
    Serial.print(F("+ returnByte="));
    Serial.println(returnByte);
    singleStepWait();
  } else {
    returnByte = MEM_READ_THIS(a);
  }
  return returnByte;
}

inline uint16_t MEM_READ_WORD(uint16_t addr) {
  if ( host_read_status_led_WAIT() ) {
    // dave
    // Since wait state, single step through memory reads.
    byte l, h;
    //
    l = MREAD(addr);
    host_set_status_leds_READMEM();
    host_set_addr_leds(addr);
    host_set_data_leds(l);
    singleStepWait();
    //
    addr++;
    h = MREAD(addr);
    host_set_addr_leds(addr);
    host_set_data_leds(h);
    singleStepWait();
    return l | (h * 256);
  }
  else {
    byte l, h;
    l = MEM_READ_STEP(addr);
    addr++;
    h = MEM_READ_STEP(addr);
    return l | (h * 256);
  }
}

byte MEM_READ_STEP(uint16_t a) {
  byte v = MREAD(a);
  host_set_status_leds_READMEM();
  altair_set_outputs(a, v);
  v = host_read_data_leds(); // CPU reads whatever is on the data bus at this point
  host_clr_status_led_MEMR();
  return v;
}

// -----------------------------------------
// Write

inline void MEM_WRITE_WORD(uint16_t addr, uint16_t v)
{
  if ( host_read_status_led_WAIT() ) {
    MEM_WRITE_STEP(addr, v & 255);
    addr++;
    MEM_WRITE_STEP(addr, v / 256);
  }
  else {
    byte b;
#if SHOW_MWRITE_OUTPUT>0
    b = v & 255;
    MEM_WRITE(addr, b);
    for (uint8_t i = 0; i < 5; i++) asm("NOP");
    b = v / 256;
    addr++;
    MEM_WRITE(addr, b);
#else
    host_set_status_leds_WRITEMEM();
    host_set_data_leds(0xff);
    host_set_addr_leds(addr);
    b = v & 255;
    MWRITE(addr, b);
    for (uint8_t i = 0; i < 5; i++) asm("NOP");
    addr++;
    host_set_addr_leds(addr);
    b = v / 256;
    MWRITE(addr, b);
#endif
  }
}

// dave
void MEM_WRITE(uint16_t a, uint16_t v) {
  if ( host_read_status_led_WAIT() ) {
    host_set_addr_leds(a);
    host_set_data_leds(v);
    host_set_status_leds_WRITEMEM();
    MWRITE(a, v);
    singleStepWait();
  } else {
    MWRITE(a, v);
  }
}

void MEM_WRITE_STEP(uint16_t a, byte v) {
  MWRITE(a, v);
  host_set_status_leds_WRITEMEM();
#if SHOW_MWRITE_OUTPUT>0
  altair_set_outputs(a, v);
#else
  altair_set_outputs(a, 0xff);
#endif
  singleStepWait();
  host_clr_status_led_WO();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// From cpucore_i8080.cpp

void pushStackSlow(byte valueH, byte valueL) {
  host_set_status_led_STACK();
  regSP--;
  MEM_WRITE_STEP(regSP, valueH);
  regSP--;
  MEM_WRITE_STEP(regSP, valueL);
  host_clr_status_led_STACK();
}

void popStackSlow(byte *valueH, byte *valueL) {
  host_set_status_led_STACK();
  *valueL = MEM_READ_STEP(regSP);
  regSP++;
  *valueH = MEM_READ_STEP(regSP);
  host_clr_status_led_STACK();
}


#if SHOW_MWRITE_OUTPUT>0

#define pushStack(valueH, valueL)             \
  if( !host_read_status_led_WAIT() )          \
  {                                           \
    host_set_status_led_STACK();              \
    regSP--;                                  \
    MEM_WRITE(regSP, valueH);                 \
    regSP--;                                  \
    MEM_WRITE(regSP, valueL);                 \
    host_clr_status_led_STACK();              \
  }                                           \
  else pushStackSlow(valueH, valueL);

#else

#define pushStack(valueH, valueL)             \
  if( !host_read_status_led_WAIT() )          \
  {                                           \
    host_set_status_led_STACK();              \
    host_set_status_leds_WRITEMEM();          \
    regSP--;                                  \
    host_set_addr_leds(regSP);                \
    MWRITE(regSP, valueH);                    \
    regSP--;                                  \
    host_set_addr_leds(regSP);                \
    MWRITE(regSP, valueL);                    \
    host_clr_status_led_STACK();              \
  }                                           \
  else pushStackSlow(valueH, valueL);

#endif

#if USE_REAL_MREAD_TIMING>0

#define popStack(valueH, valueL)              \
  if( !host_read_status_led_WAIT() )          \
  {                                           \
    host_set_status_led_STACK();              \
    valueL = MEM_READ(regSP);                 \
    regSP++;                                  \
    valueH = MEM_READ(regSP);                 \
    regSP++;                                  \
    host_clr_status_led_STACK();              \
  }                                           \
  else popStackSlow(&valueH, &valueL);

#else

#define popStack(valueH, valueL)                   \
  if( !host_read_status_led_WAIT() )               \
  {                                                \
    host_set_status_leds_READMEM_STACK();          \
    host_set_addr_leds(regSP);                     \
    valueL = MREAD(regSP);                         \
    regSP++;                                       \
    host_set_addr_leds(regSP);                     \
    valueH = host_set_data_leds(MREAD(regSP));     \
    regSP++;                                       \
    host_clr_status_led_STACK();                   \
  }                                                \
  else popStackSlow(&valueH, &valueL);

#endif


inline void pushStackWord(uint16_t v) {
  host_set_status_led_STACK();
  regSP -= 2;
  MEM_WRITE_WORD(regSP, v);
  host_clr_status_led_STACK();
}

inline uint16_t popStackWord() {
  uint16_t v;
  host_set_status_led_STACK();
  v = MEM_READ_WORD(regSP);
  regSP += 2;
  host_clr_status_led_STACK();
  return v;
}

#define pushPC() pushStack(regPCU.H, regPCU.L) //pushStackWord(regPC)
#define popPC()  regPC = popStackWord()

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// config.cpp

#define CONFIG_FILE_VERSION 10

// current configuration number
static byte config_current = 0;
uint32_t config_flags;
uint32_t config_flags2;
uint32_t config_serial_settings, new_config_serial_settings;

// cofig_serial_settings2:
// xxxxxxxB BPPSBBPP SBBPPSBB PPSBBPPS
// for all 5 host interfaces:
// BB = number of bits (0=5, 1=6, 2=7, 3=8)
// PP = parity         (0=none, 1=even, 2=odd)
// S  = stop bits      (0=1, 1=2)
uint32_t config_serial_settings2, new_config_serial_settings2;

// config_serial_device_settings[0-5]
// xxxxxxxx xxxxMMMR TT77UUVV CNNNBBBB
// BBBB = baud rate for serial playback (see baud rates above)
// NNN  = NULs to send after a carriage return when playing back examples
// C    = trap CLOAD/CSAVE in extended BASIC (for CSM_ACR device only)
// MMM  = map device to host interface (000=NONE, 001=first, 010=second, 011=third, 100=fourth, 101=fifth, 111=primary)
// UU   = only uppercase for inputs (00=off, 01=on, 10=autodetect)
// 77   = use 7 bit for serial outputs (00=off [use 8 bit], 01=on, 10=autodetect)
// TT   = translate backspace to (00=off, 01=underscore, 10=autodetect, 11=delete)
// R    = force realtime operation (use baud rate even if not using interrupts)
// VV   = 88-SIO board version (0=rev0, 1=rev1, 2=Cromemco)
uint32_t config_serial_device_settings[NUM_SERIAL_DEVICES];

// map emulated device (SIO/2SIO etc.) to host serial port number
byte config_serial_sim_to_host[NUM_SERIAL_DEVICES];

// masks defining which interrupts (INT_*) are at which vector interrupt levels
uint32_t config_interrupt_vi_mask[8];

// mask defining whch interrupts (INT_*) are connected if VI board is not installed
uint32_t config_interrupt_mask;

// amount of RAM installed
uint32_t config_mem_size;

// --------------------------------------------------------------------------------

static bool config_read_string(char *buf, byte bufsize)
{
  return true;
}

inline uint32_t get_bits(uint32_t v, byte i, byte n)
{
  return (v >> ((uint32_t) i)) & ((1ul << n) - 1);
}

inline uint32_t set_bits(uint32_t v, byte i, byte n, uint32_t nv)
{
  uint32_t mask = ((1ul << n) - 1) << i;
  return (v & ~mask) | ((nv << i) & mask);
}

static uint32_t toggle_bits(uint32_t v, byte i, byte n, byte min = 0x00, byte max = 0xff)
{
  byte b = get_bits(v, i, n) + 1;
  return set_bits(v, i, n, b > max ? min : (b < min ? min : b));
}

#if USE_THROTTLE>0
int config_throttle()
{
  if ( config_flags & CF_THROTTLE )
  {
    int i = get_bits(config_flags, 12, 5);
    if ( i == 0 )
      return -1; // auto
    else
      return i;  // manual
  }
  else
    return 0; // off
}
#endif

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// cpucore.cpp

// i8080 registers implementation
union unionAF regAF;
union unionBC regBC;
union unionDE regDE;
union unionHL regHL;
union unionPC regPCU;
uint16_t regSP;

#ifdef __AVR_ATmega2560__
#ifdef __SAM3X8E__
#define nothing 13
uint16_t host_read_addr_leds() {
  // A0..7  => PIOC, bits 2-9
  // A8..15 => PIOC, bits 12-19
  word w = REG_PIOC_PDSR;
  return ((w & 0x000ff000) >> 4) | ((w & 0x000003fc) >> 2);
}
#endif
#else
uint16_t host_read_addr_leds() {
  return (0);
}
#endif

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// numsys.cpp

static byte numsys = NUMSYS_HEX;

void numsys_print_byte(byte b) {
  switch ( numsys )
  {
    case NUMSYS_HEX: numsys_print_byte_hex(b); break;
    case NUMSYS_OCT: numsys_print_byte_oct(b); break;
    default: numsys_print_byte_dec(b); break;
  }
}

void numsys_print_word(uint16_t w) {
  if ( numsys == NUMSYS_HEX )
  {
    numsys_print_byte(w >> 8);
    numsys_print_byte(w & 0xff);
  }
  else if ( numsys == NUMSYS_OCT )
  {
    Serial.print((w >> 15) & 007);
    Serial.print((w >> 12) & 007);
    Serial.print((w >> 9) & 007);
    Serial.print((w >> 6) & 007);
    Serial.print((w >> 3) & 007);
    Serial.print( w      & 007);
  }
  else
  {
    if ( w < 10 )    Serial.print(' ');
    if ( w < 100 )   Serial.print(' ');
    if ( w < 1000 )  Serial.print(' ');
    if ( w < 10000 ) Serial.print(' ');
    Serial.print(w);
  }
}


void numsys_print_mem(uint16_t addr, byte num, bool printBrackets)
{
  byte i;
  if ( printBrackets ) Serial.print('[');
  for (i = 0; i < num; i++)
  {
    numsys_print_byte(MREAD(addr + i));
    if (i + 1 < num) Serial.print(' ');
  }
  if ( printBrackets ) Serial.print(']');
}

void numsys_print_byte_hex(byte b)
{
  if ( b < 16 ) Serial.print('0');
  Serial.print(b, HEX);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// timer.cpp

#ifdef __AVR_ATmega2560__
#define MAX_TIMERS 9
#else
#define MAX_TIMERS 13
#endif


#define DEBUG 0

uint32_t timer_cycle_counter        = 0;
uint32_t timer_cycle_counter_offset = 0;
uint32_t timer_next_expire_cycles   = 0xffffffff;
byte     timer_next_expire_tid      = 0xff;


struct TimerData {
  TimerFnTp timer_fn;
  bool      running;
  bool      recurring;
  uint32_t  cycles_period;
  uint32_t  cycles_count;
};

struct TimerData timer_data[MAX_TIMERS];
byte timer_queue[MAX_TIMERS];
byte timer_queue_len = 0;

#if DEBUG>1
static void print_queue()
{
  printf("TIMER QUEUE: [");
  for (byte i = 0; i < timer_queue_len; i++)
    printf("%i=%i/%i ", timer_queue[i], timer_data[timer_queue[i]].cycles_count, timer_data[timer_queue[i]].cycles_period);
  if ( timer_queue_len > 0 )
    printf("] / next timer %i in %u\n", timer_next_expire_tid, timer_next_expire_cycles);
  else
    printf("]\n");
}
#else
#define print_queue() while(0)
#endif

static void print_queue2()
{
  printf("TIMER QUEUE: [");
  for (byte i = 0; i < timer_queue_len; i++)
    printf("%i=%i/%i ", timer_queue[i], timer_data[timer_queue[i]].cycles_count, timer_data[timer_queue[i]].cycles_period);
  if ( timer_queue_len > 0 )
    printf("] / next timer %i in %u\n", timer_next_expire_tid, timer_next_expire_cycles);
  else
    printf("]\n");
}


static void timer_reset_counter()
{
#if DEBUG>1
  printf("timer_reset_counter()\n");
#endif
  for (byte i = 0; i < timer_queue_len; i++)
  {
    if ( timer_data[timer_queue[i]].cycles_count > timer_cycle_counter )
      timer_data[timer_queue[i]].cycles_count -= timer_cycle_counter;
    else
      timer_data[timer_queue[i]].cycles_count = 0;
  }

  timer_cycle_counter_offset += timer_cycle_counter;
  timer_next_expire_cycles   -= timer_cycle_counter;
  timer_cycle_counter = 0;

  print_queue();
}


static void timer_queue_get_next()
{
#if DEBUG>1
  printf("timer_queue_get_next()\n");
#endif
  if ( timer_queue_len < 2 )
  {
    timer_next_expire_tid    = 0xff;
    timer_next_expire_cycles = 0xffffffff;
    timer_queue_len          = 0;
    timer_cycle_counter_offset += timer_cycle_counter;
    timer_cycle_counter = 0;
  }
  else
  {
    // remove first element from queue
    memmove(timer_queue, timer_queue + 1, timer_queue_len - 1);
    timer_queue_len--;

    // reset cycle counter
    for (byte i = 0; i < timer_queue_len; i++)
    {
      if ( timer_data[timer_queue[i]].cycles_count > timer_cycle_counter )
        timer_data[timer_queue[i]].cycles_count -= timer_cycle_counter;
      else
        timer_data[timer_queue[i]].cycles_count = 0;
    }

    timer_cycle_counter_offset += timer_cycle_counter;
    timer_cycle_counter = 0;

    // set next expiration
    timer_next_expire_tid    = timer_queue[0];
    timer_next_expire_cycles = timer_data[timer_next_expire_tid].cycles_count;
  }

  print_queue();
}


static void timer_queue_add(byte tid)
{
#if DEBUG>1
  printf("timer_queue_add()\n");
#endif

  timer_reset_counter();
  uint32_t cycles_count = timer_data[tid].cycles_count;

  byte i;
  for (i = 0; i < timer_queue_len; i++)
    if ( timer_data[timer_queue[i]].cycles_count > cycles_count )
      break;

  memmove(timer_queue + i + 1, timer_queue + i, timer_queue_len - i);
  timer_queue[i] = tid;
  timer_queue_len++;

  if ( i == 0 )
  {
    timer_next_expire_tid    = tid;
    timer_next_expire_cycles = timer_data[tid].cycles_count;
  }

  print_queue();
}


static void timer_queue_remove(byte tid)
{
#if DEBUG>1
  printf("timer_queue_remove()\n");
#endif

  byte i;
  for (i = 0; i < timer_queue_len; i++)
    if ( timer_queue[i] == tid )
    {
      memmove(timer_queue + i, timer_queue + i + 1, timer_queue_len - i - 1);
      timer_queue_len--;

      if ( timer_queue_len == 0 )
      {
        timer_next_expire_tid    = 0xff;
        timer_next_expire_cycles = 0xffffffff;
      }
      else if ( i == 0 )
      {
        timer_next_expire_tid    = timer_queue[0];
        timer_next_expire_cycles = timer_data[timer_next_expire_tid].cycles_count;
      }

      print_queue();
      return;
    }
}


void timer_check()
{
  byte tid = timer_next_expire_tid;
  bool show = false;

  while ( tid < 0xff )
  {
#if DEBUG>0
    printf("%u: timer %i expired\n", timer_get_cycles(), tid);
    print_queue();
#endif
    if ( !timer_data[tid].recurring )
      timer_data[tid].running = false;

    timer_queue_get_next();

    if ( timer_data[tid].recurring )
    {
#if DEBUG>0
      printf("re-scheduling timer %i\n", tid);
#endif
      timer_data[tid].cycles_count = timer_data[tid].cycles_period;
      timer_queue_add(tid);
    }

#if DEBUG>1
    printf("calling timer %i function\n", tid);
#endif
    (timer_data[tid].timer_fn)();
#if DEBUG>1
    printf("returned from timer %i function\n", tid);
#endif

    // check if more timers expired
    if ( timer_queue_len > 0 && timer_data[timer_queue[0]].cycles_count == 0 )
      tid = timer_queue[0];
    else
      tid = 0xff;
  }
}


void timer_setup(byte tid, uint32_t microseconds, TimerFnTp timer_fn)
{
  if ( tid < MAX_TIMERS )
  {
    bool running = timer_data[tid].running;
    if ( running ) timer_stop(tid);
    timer_data[tid].timer_fn      = timer_fn;
    timer_data[tid].cycles_period = microseconds * 2;
    if ( running ) timer_start(tid);
  }
}


void timer_start(byte tid, uint32_t microseconds, bool recurring)
{
  if ( tid < MAX_TIMERS )
  {
    if ( microseconds > 0 ) timer_data[tid].cycles_period = microseconds * 2;

#if DEBUG>0
    printf("%u: starting timer %i: %i microseconds\n", timer_get_cycles(), tid, timer_data[tid].cycles_period / 2);
#endif
    if ( timer_data[tid].running ) timer_queue_remove(tid);

    timer_data[tid].recurring    = recurring;
    timer_data[tid].cycles_count = timer_data[tid].cycles_period;
    timer_data[tid].running      = true;

    timer_queue_add(tid);
  }
}


void timer_stop(byte tid)
{
  if ( tid < MAX_TIMERS && timer_data[tid].running )
  {
#if DEBUG>0
    printf("%u: stopping timer %i\n", timer_get_cycles(), tid);
#endif
    timer_queue_remove(tid);
    timer_data[tid].running = false;
  }
}

uint32_t timer_get_period(byte tid)
{
  return tid < MAX_TIMERS ? timer_data[tid].cycles_period / 2 : 0;
}

bool timer_running(byte tid)
{
  return tid < MAX_TIMERS ? timer_data[tid].running : false;
}


void timer_setup()
{
  timer_queue_len  = 0;
  timer_cycle_counter = 0;
  timer_cycle_counter_offset = 0;
  for (byte tid = 0; tid < MAX_TIMERS; tid++)
  {
    timer_data[tid].running  = false;
    timer_data[tid].timer_fn = NULL;
  }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// cpucore_i8080.cpp

#define setCarryBit(v)     if(v) regS |= PS_CARRY;     else regS &= ~PS_CARRY
#define setHalfCarryBit(v) if(v) regS |= PS_HALFCARRY; else regS &= ~PS_HALFCARRY

static const byte halfCarryTableAdd[] = { 0, 0, 1, 0, 1, 0, 1, 1 };
static const byte halfCarryTableSub[] = { 1, 0, 0, 0, 1, 1, 1, 0 };
#define setHalfCarryBitAdd(opd1, opd2, res) setHalfCarryBit(halfCarryTableAdd[((((opd1) & 0x08) / 2) | (((opd2) & 0x08) / 4) | (((res) & 0x08) / 8)) & 0x07])
#define setHalfCarryBitSub(opd1, opd2, res) setHalfCarryBit(halfCarryTableSub[((((opd1) & 0x08) / 2) | (((opd2) & 0x08) / 4) | (((res) & 0x08) / 8)) & 0x07])

static const byte parity_table[256] =
{ 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
  0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
  0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
  1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
  0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
  1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
  1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
  0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1
};


// -----------------------------------------------------------------------------
inline void setStatusBits(byte value) {
  byte b;
  b = regS & ~(PS_ZERO | PS_SIGN | PS_PARITY);
  if ( parity_table[value] ) b |= PS_PARITY;
  if ( value == 0 ) b |= PS_ZERO;
  b |= (value & PS_SIGN);
  regS = b;
}


#define CPU_ADC(REG) \
  static void cpu_ADC ## REG () \
  { \
    uint16_t w = regA + reg ## REG; \
    if(regS & PS_CARRY) w++; \
    setHalfCarryBitAdd(regA, reg ## REG, w); \
    setCarryBit(w&0x100); \
    setStatusBits((byte) w); \
    regA = (byte) w; \
    TIMER_ADD_CYCLES(4); \
  }

CPU_ADC(B);
CPU_ADC(C);
CPU_ADC(D);
CPU_ADC(E);
CPU_ADC(H);
CPU_ADC(L);
CPU_ADC(A);


#define CPU_ADD(REG) \
  static void cpu_ADD ## REG () \
  { \
    uint16_t w = regA + reg ## REG; \
    setCarryBit(w&0x100); \
    setHalfCarryBitAdd(regA, reg ## REG, w); \
    setStatusBits((byte) w); \
    regA = (byte) w; \
    TIMER_ADD_CYCLES(4); \
  }

CPU_ADD(B);
CPU_ADD(C);
CPU_ADD(D);
CPU_ADD(E);
CPU_ADD(H);
CPU_ADD(L);
CPU_ADD(A);


#define CPU_SBB(REG) \
  static void cpu_SBB ## REG () \
  { \
    uint16_t w = regA - reg ## REG; \
    if(regS & PS_CARRY) w--; \
    setHalfCarryBitSub(regA, reg ## REG, w); \
    setCarryBit(w&0x100); \
    setStatusBits((byte) w); \
    regA = (byte) w; \
    TIMER_ADD_CYCLES(4); \
  }

CPU_SBB(B);
CPU_SBB(C);
CPU_SBB(D);
CPU_SBB(E);
CPU_SBB(H);
CPU_SBB(L);
CPU_SBB(A);


#define CPU_SUB(REG) \
  static void cpu_SUB ## REG () \
  { \
    uint16_t w = regA - reg ## REG; \
    setCarryBit(w&0x100); \
    setHalfCarryBitSub(regA, reg ## REG, w); \
    setStatusBits((byte) w); \
    regA = (byte) w; \
    TIMER_ADD_CYCLES(4); \
  }

CPU_SUB(B);
CPU_SUB(C);
CPU_SUB(D);
CPU_SUB(E);
CPU_SUB(H);
CPU_SUB(L);
CPU_SUB(A);


#define CPU_ANA(REG) \
  static void cpu_ANA ## REG () \
  { \
    setHalfCarryBit(regA&0x08 | reg ## REG & 0x08); \
    regA &= reg ## REG; \
    setCarryBit(0); \
    setStatusBits(regA); \
    TIMER_ADD_CYCLES(4); \
  }

CPU_ANA(B);
CPU_ANA(C);
CPU_ANA(D);
CPU_ANA(E);
CPU_ANA(H);
CPU_ANA(L);
CPU_ANA(A);


#define CPU_XRA(REG) \
  static void cpu_XRA ## REG () \
  { \
    regA ^= reg ## REG; \
    setCarryBit(0); \
    setHalfCarryBit(0); \
    setStatusBits(regA); \
    TIMER_ADD_CYCLES(4); \
  }

CPU_XRA(B);
CPU_XRA(C);
CPU_XRA(D);
CPU_XRA(E);
CPU_XRA(H);
CPU_XRA(L);
CPU_XRA(A);


#define CPU_ORA(REG) \
  static void cpu_ORA ## REG () \
  { \
    regA |= reg ## REG; \
    setCarryBit(0); \
    setHalfCarryBit(0); \
    setStatusBits(regA); \
    TIMER_ADD_CYCLES(4); \
  }

CPU_ORA(B);
CPU_ORA(C);
CPU_ORA(D);
CPU_ORA(E);
CPU_ORA(H);
CPU_ORA(L);
CPU_ORA(A);


static void cpu_ADCM()
{
  byte opd2  = MEM_READ(regHL.HL);
  uint16_t w = regA + opd2;
  if (regS & PS_CARRY) w++;
  setHalfCarryBitAdd(regA, opd2, w);
  setCarryBit(w & 0x100);
  setStatusBits((byte) w);
  regA = (byte) w;
  TIMER_ADD_CYCLES(7);
}

static void cpu_ADDM()
{
  byte opd2 = MEM_READ(regHL.HL);
  uint16_t w    = regA + opd2;
  setCarryBit(w & 0x100);
  setHalfCarryBitAdd(regA, opd2, w);
  setStatusBits((byte) w);
  regA = (byte) w;
  TIMER_ADD_CYCLES(7);
}

static void cpu_SBBM()
{
  byte opd2 = MEM_READ(regHL.HL);
  uint16_t w    = regA - opd2;
  if (regS & PS_CARRY) w--;
  setHalfCarryBitSub(regA, opd2, w);
  setCarryBit(w & 0x100);
  setStatusBits((byte) w);
  regA = (byte) w;
  TIMER_ADD_CYCLES(7);
}

static void cpu_SUBM()
{
  byte opd2 = MEM_READ(regHL.HL);
  uint16_t w    = regA - opd2;
  setCarryBit(w & 0x100);
  setHalfCarryBitSub(regA, opd2, w);
  setStatusBits((byte) w);
  regA = (byte) w;
  TIMER_ADD_CYCLES(7);
}

static void cpu_ANAM()
{
  byte opd2 = MEM_READ(regHL.HL);
  setHalfCarryBit(regA & 0x08 | opd2 & 0x08);
  regA &= opd2;
  setCarryBit(0);
  setStatusBits(regA);
  TIMER_ADD_CYCLES(7);
}

static void cpu_XRAM()
{
  regA ^= MEM_READ(regHL.HL);
  setCarryBit(0);
  setHalfCarryBit(0);
  setStatusBits(regA);
  TIMER_ADD_CYCLES(7);
}

static void cpu_ORAM()
{
  regA |= MEM_READ(regHL.HL);
  setCarryBit(0);
  setHalfCarryBit(0);
  setStatusBits(regA);
  TIMER_ADD_CYCLES(7);
}

static void cpu_CALL()
{
  regPC += 2;
  pushPC();
  regPC = MEM_READ_WORD(regPC - 2);
  TIMER_ADD_CYCLES(17);
}

#define CPU_CMP(REG) \
  static void cpu_CMP ## REG () \
  { \
    uint16_t w = regA - reg ## REG; \
    setCarryBit(w&0x100); \
    setHalfCarryBitSub(regA, reg ## REG, w); \
    setStatusBits((byte) w); \
    TIMER_ADD_CYCLES(4); \
  }

CPU_CMP(B);
CPU_CMP(C);
CPU_CMP(D);
CPU_CMP(E);
CPU_CMP(H);
CPU_CMP(L);
CPU_CMP(A);


static void cpu_CMPM()
{
  byte opd2 = MEM_READ(regHL.HL);
  uint16_t w    = regA - opd2;
  setCarryBit(w & 0x100);
  setHalfCarryBitSub(regA, opd2, w);
  setStatusBits((byte) w);
  TIMER_ADD_CYCLES(7);
}

#define CPU_DCR(REG) \
  static void cpu_DCR ## REG () \
  { \
    byte res = reg ## REG - 1; \
    setHalfCarryBit((res & 0x0f)!=0x0f); \
    setStatusBits(res); \
    reg ## REG = res; \
    TIMER_ADD_CYCLES(5); \
  }

CPU_DCR(B);
CPU_DCR(C);
CPU_DCR(D);
CPU_DCR(E);
CPU_DCR(H);
CPU_DCR(L);
CPU_DCR(A);

static void cpu_DCRM()
{
  byte res  = MEM_READ(regHL.HL) - 1;
  setHalfCarryBit((res & 0x0f) != 0x0f);
  setStatusBits(res);
  MEM_WRITE(regHL.HL, res);
  TIMER_ADD_CYCLES(10);
}

static void cpu_ADI()
{
  byte opd2 = MEM_READ(regPC);
  uint16_t w    = regA + opd2;
  setCarryBit(w & 0x100);
  setHalfCarryBitAdd(regA, opd2, w);
  setStatusBits((byte) w);
  regA = (byte) w;
  regPC++;
  TIMER_ADD_CYCLES(7);
}

static void cpu_ACI()
{
  byte opd2 = MEM_READ(regPC);
  uint16_t w    = regA + opd2;
  if (regS & PS_CARRY) w++;
  setHalfCarryBitAdd(regA, opd2, w);
  setCarryBit(w & 0x100);
  setStatusBits((byte) w);
  regA = (byte) w;
  regPC++;
  TIMER_ADD_CYCLES(7);
}

static void cpu_SUI()
{
  byte opd2 = MEM_READ(regPC);
  uint16_t w    = regA - opd2;
  setCarryBit(w & 0x100);
  setHalfCarryBitSub(regA, opd2, w);
  setStatusBits((byte) w);
  regA = (byte) w;
  regPC++;
  TIMER_ADD_CYCLES(7);
}

static void cpu_SBI()
{
  byte opd2 = MEM_READ(regPC);
  uint16_t w    = regA - opd2;
  if (regS & PS_CARRY) w--;
  setHalfCarryBitSub(regA, opd2, w);
  setCarryBit(w & 0x100);
  setStatusBits((byte) w);
  regA = (byte) w;
  regPC++;
  TIMER_ADD_CYCLES(7);
}

static void cpu_ANI()
{
  byte opd2 = MEM_READ(regPC);
  setHalfCarryBit(regA & 0x08 | opd2 & 0x08);
  regA &= opd2;
  setCarryBit(0);
  setStatusBits(regA);
  regPC++;
  TIMER_ADD_CYCLES(7);
}

static void cpu_XRI()
{
  regA ^= MEM_READ(regPC);
  setCarryBit(0);
  setHalfCarryBit(0);
  setStatusBits(regA);
  regPC++;
  TIMER_ADD_CYCLES(7);
}

static void cpu_ORI()
{
  regA |= MEM_READ(regPC);
  setCarryBit(0);
  setHalfCarryBit(0);
  setStatusBits(regA);
  regPC++;
  TIMER_ADD_CYCLES(7);
}

static void cpu_CPI()
{
  byte opd2  = MEM_READ(regPC);
  uint16_t w = regA - opd2;
  setCarryBit(w & 0x100);
  setHalfCarryBitSub(regA, opd2, w);
  setStatusBits((byte) w);
  regPC++;
  TIMER_ADD_CYCLES(7);
}

static void cpu_CMA()
{
  regA = ~regA;
  TIMER_ADD_CYCLES(4);
}

static void cpu_CMC()
{
  regS ^= PS_CARRY;
  TIMER_ADD_CYCLES(4);
}

static void cpu_DAA()
{
  byte b   = regA;
  byte adj = 0;
  if ( (regS & PS_HALFCARRY) || (b & 0x0f) > 0x09 ) adj = 0x06;
  if ( (regS & PS_CARRY)     || (b & 0xf0) > 0x90 || ((b & 0xf0) == 0x90 && (b & 0x0f) > 9) )
  {
    adj  |= 0x60;
    regS |= PS_CARRY;
  }

  regA = b + adj;
  setHalfCarryBitAdd(b, adj, regA);
  setStatusBits(regA);
  TIMER_ADD_CYCLES(4);
}



#define CPU_DAD(REG)            \
  static void cpu_DAD ## REG()         \
  {                             \
    uint16_t w = reg##REG.REG;  \
    regHL.HL += w;              \
    setCarryBit(regHL.HL < w);  \
    TIMER_ADD_CYCLES(10);        \
  }

CPU_DAD(BC);
CPU_DAD(DE);
CPU_DAD(HL);


static void cpu_DADS()
{
  regHL.HL += regSP;
  setCarryBit(regHL.HL < regSP);
  TIMER_ADD_CYCLES(10);
}


#define CPU_DCX(REG) \
  static void cpu_DCX ## REG () \
  {                      \
    host_set_addr_leds(--reg##REG.REG); \
    TIMER_ADD_CYCLES(5);  \
  }

CPU_DCX(BC);
CPU_DCX(DE);
CPU_DCX(HL);

static void cpu_DCXSP()
{
  regSP--;
  TIMER_ADD_CYCLES(5);
}

static void cpu_DI()
{
  // altair_interrupt_disable();
  TIMER_ADD_CYCLES(4);
}

static void cpu_EI()
{
  // altair_interrupt_enable();
  TIMER_ADD_CYCLES(4);
}

static void cpu_HLT()
{
  altair_hlt();
  TIMER_ADD_CYCLES(7);
}

#define CPU_INR(REG) \
  static void cpu_INR ## REG() \
  { \
    byte res = reg ## REG + 1; \
    setHalfCarryBit((res&0x0f)==0); \
    setStatusBits(res); \
    reg ## REG = res; \
    TIMER_ADD_CYCLES(5); \
  }

CPU_INR(B);
CPU_INR(C);
CPU_INR(D);
CPU_INR(E);
CPU_INR(H);
CPU_INR(L);
CPU_INR(A);


static void cpu_INRM()
{
  byte res = MEM_READ(regHL.HL) + 1;
  setHalfCarryBit((res & 0x0f) == 0);
  setStatusBits(res);
  MEM_WRITE(regHL.HL, res);
  TIMER_ADD_CYCLES(10);
}

#define CPU_INX(REG) \
  static void cpu_INX ## REG () \
  { \
    host_set_addr_leds(++reg##REG.REG); \
    TIMER_ADD_CYCLES(5); \
  }

CPU_INX(BC);
CPU_INX(DE);
CPU_INX(HL);


static void cpu_INXSP()
{
  regSP++;
  TIMER_ADD_CYCLES(5);
}

static void cpu_LDA() {
  uint16_t addr = MEM_READ_WORD(regPC);
  regA = MEM_READ(addr);
  regPC += 2;
  TIMER_ADD_CYCLES(13);
}

#define CPU_LDX(REG) \
  static void cpu_LDX ## REG() \
  { \
    regA = MEM_READ(reg##REG.REG); \
    TIMER_ADD_CYCLES(7); \
  }

CPU_LDX(BC);
CPU_LDX(DE);

static void cpu_LHLD()
{
  uint16_t addr = MEM_READ_WORD(regPC);
  regL = MEM_READ(addr);
  regH = MEM_READ(addr + 1);
  regPC += 2;
  TIMER_ADD_CYCLES(16);
}

static void cpu_LXIS()
{
  regSP = MEM_READ_WORD(regPC);
  regPC += 2;
  TIMER_ADD_CYCLES(10);
}

#define CPU_LXI(REGH,REGL) \
  static void cpu_LXI ## REGH ## REGL() \
  { \
    reg ## REGL = MEM_READ(regPC); \
    reg ## REGH = MEM_READ(regPC+1); \
    regPC += 2; \
    TIMER_ADD_CYCLES(10); \
  }

CPU_LXI(B, C);
CPU_LXI(D, E);
CPU_LXI(H, L);


#define CPU_MVRR(REGTO,REGFROM)                 \
  static void cpu_MV ## REGTO ## REGFROM ()            \
  {                                             \
    reg ## REGTO = reg ## REGFROM;              \
    TIMER_ADD_CYCLES(5);                         \
  }

#define CPU_MVMR(REGFROM)                       \
  static void cpu_MVM ## REGFROM()                     \
  {                                             \
    MEM_WRITE(regHL.HL, reg ## REGFROM);        \
    TIMER_ADD_CYCLES(7);                         \
  }

#define CPU_MVRM(REGTO)                         \
  static void cpu_MV ## REGTO ## M()                   \
  {                                             \
    reg ## REGTO = MEM_READ(regHL.HL);          \
    TIMER_ADD_CYCLES(7);                         \
  }

#define CPU_MVRI(REGTO)                         \
  static void cpu_MV ## REGTO ## I()                   \
  {                                             \
    reg ## REGTO = MEM_READ(regPC);             \
    regPC++;                                    \
    TIMER_ADD_CYCLES(7);                         \
  }

CPU_MVRR(B, B);
CPU_MVRR(B, C);
CPU_MVRR(B, D);
CPU_MVRR(B, E);
CPU_MVRR(B, H);
CPU_MVRR(B, L);
CPU_MVRR(B, A);
CPU_MVRM(B);
CPU_MVMR(B);
CPU_MVRI(B);

CPU_MVRR(C, B);
CPU_MVRR(C, C);
CPU_MVRR(C, D);
CPU_MVRR(C, E);
CPU_MVRR(C, H);
CPU_MVRR(C, L);
CPU_MVRR(C, A);
CPU_MVRM(C);
CPU_MVMR(C);
CPU_MVRI(C);

CPU_MVRR(D, B);
CPU_MVRR(D, C);
CPU_MVRR(D, D);
CPU_MVRR(D, E);
CPU_MVRR(D, H);
CPU_MVRR(D, L);
CPU_MVRR(D, A);
CPU_MVRM(D);
CPU_MVMR(D);
CPU_MVRI(D);

CPU_MVRR(E, B);
CPU_MVRR(E, C);
CPU_MVRR(E, D);
CPU_MVRR(E, E);
CPU_MVRR(E, H);
CPU_MVRR(E, L);
CPU_MVRR(E, A);
CPU_MVRM(E);
CPU_MVMR(E);
CPU_MVRI(E);

CPU_MVRR(H, B);
CPU_MVRR(H, C);
CPU_MVRR(H, D);
CPU_MVRR(H, E);
CPU_MVRR(H, H);
CPU_MVRR(H, L);
CPU_MVRR(H, A);
CPU_MVRM(H);
CPU_MVMR(H);
CPU_MVRI(H);

CPU_MVRR(L, B);
CPU_MVRR(L, C);
CPU_MVRR(L, D);
CPU_MVRR(L, E);
CPU_MVRR(L, H);
CPU_MVRR(L, L);
CPU_MVRR(L, A);
CPU_MVRM(L);
CPU_MVMR(L);
CPU_MVRI(L);

CPU_MVRR(A, B);
CPU_MVRR(A, C);
CPU_MVRR(A, D);
CPU_MVRR(A, E);
CPU_MVRR(A, H);
CPU_MVRR(A, L);
CPU_MVRR(A, A);
CPU_MVRM(A);
CPU_MVMR(A);
CPU_MVRI(A);


static void cpu_MVMI() {
  // MVI dst, M
  MEM_WRITE(regHL.HL, MEM_READ(regPC));
  regPC++;
  TIMER_ADD_CYCLES(10);
}


static void cpu_NOP()
{
  TIMER_ADD_CYCLES(4);
}

static void cpu_PCHL()
{
  regPC = regHL.HL;
  TIMER_ADD_CYCLES(5);
}


#define CPU_POP(REGH, REGL)  \
  static void cpu_POP ## REGH ## REGL() \
  { \
    popStack(reg ## REGH, reg ## REGL); \
    TIMER_ADD_CYCLES(10); \
  }

CPU_POP(B, C);
CPU_POP(D, E);
CPU_POP(H, L);
CPU_POP(A, S);


#define CPU_PSH(REGH, REGL) \
  static void cpu_PSH ## REGH ## REGL() \
  { \
    pushStack(reg ## REGH, reg ## REGL); \
    TIMER_ADD_CYCLES(11); \
  }

CPU_PSH(B, C);
CPU_PSH(D, E);
CPU_PSH(H, L);


static void cpu_PSHAS()
{
  pushStack(regA, (regS & 0xD5) | 0x02);
  TIMER_ADD_CYCLES(11);
}

static void cpu_RLC()
{
  byte b = regA & 128;
  regA   = (regA * 2) | (b ? 1 : 0) ;
  setCarryBit(b);
  TIMER_ADD_CYCLES(4);
}

static void cpu_RRC()
{
  byte b = regA & 1;
  regA   = (regA / 2) | (b ? 128 : 0) ;
  setCarryBit(b);
  TIMER_ADD_CYCLES(4);
}

static void cpu_RAL()
{
  byte b = regA & 128;
  regA   = (regA * 2) | ((regS & PS_CARRY) ? 1 : 0) ;
  setCarryBit(b);
  TIMER_ADD_CYCLES(4);
}

static void cpu_RAR()
{
  byte b = regA & 1;
  regA   = (regA / 2) | ((regS & PS_CARRY) ? 128 : 0) ;
  setCarryBit(b);
  TIMER_ADD_CYCLES(4);
}

static void cpu_RET()
{
  popPC();
  TIMER_ADD_CYCLES(10);
}

#define CPU_RST(N) \
  static void cpu_RST ## N() \
  { \
    pushPC(); \
    regPC = 0x00 ## N; \
    TIMER_ADD_CYCLES(11); \
  }

CPU_RST(00);
CPU_RST(08);
CPU_RST(10);
CPU_RST(18);
CPU_RST(20);
CPU_RST(28);
CPU_RST(30);
CPU_RST(38);

static void cpu_RNZ()
{
  if ( !(regS & PS_ZERO) )
  {
    popPC();
    TIMER_ADD_CYCLES(11);
  }
  else
    TIMER_ADD_CYCLES(5);
}

static void cpu_RZ()
{
  if ( (regS & PS_ZERO) )
  {
    popPC();
    TIMER_ADD_CYCLES(11);
  }
  else
    TIMER_ADD_CYCLES(5);
}

static void cpu_RNC()
{
  if ( !(regS & PS_CARRY) )
  {
    popPC();
    TIMER_ADD_CYCLES(11);
  }
  else
    TIMER_ADD_CYCLES(5);
}

static void cpu_RC()
{
  if ( (regS & PS_CARRY) )
  {
    popPC();
    TIMER_ADD_CYCLES(11);
  }
  else
    TIMER_ADD_CYCLES(5);
}

static void cpu_RPO()
{
  if ( !(regS & PS_PARITY) )
  {
    popPC();
    TIMER_ADD_CYCLES(11);
  }
  else
    TIMER_ADD_CYCLES(5);
}

static void cpu_RPE()
{
  if ( (regS & PS_PARITY) )
  {
    popPC();
    TIMER_ADD_CYCLES(11);
  }
  else
    TIMER_ADD_CYCLES(5);
}

static void cpu_RP()
{
  if ( !(regS & PS_SIGN) )
  {
    popPC();
    TIMER_ADD_CYCLES(11);
  }
  else
    TIMER_ADD_CYCLES(5);
}

static void cpu_RM()
{
  if ( (regS & PS_SIGN) )
  {
    popPC();
    TIMER_ADD_CYCLES(11);
  }
  else
    TIMER_ADD_CYCLES(5);
}

static void cpu_JMP()
{
  regPC = MEM_READ_WORD(regPC);
  TIMER_ADD_CYCLES(10);
}

static void cpu_JNZ()
{
  uint16_t addr = MEM_READ_WORD(regPC);
  if ( !(regS & PS_ZERO) ) regPC = addr; else regPC += 2;
  TIMER_ADD_CYCLES(10);
}

static void cpu_JZ()
{
  uint16_t addr = MEM_READ_WORD(regPC);
  if ( (regS & PS_ZERO) ) regPC = addr; else regPC += 2;
  TIMER_ADD_CYCLES(10);
}

static void cpu_JNC()
{
  uint16_t addr = MEM_READ_WORD(regPC);
  if ( !(regS & PS_CARRY) ) regPC = addr; else regPC += 2;
  TIMER_ADD_CYCLES(10);
}

static void cpu_JC()
{
  uint16_t addr = MEM_READ_WORD(regPC);
  if ( (regS & PS_CARRY) ) regPC = addr; else regPC += 2;
  TIMER_ADD_CYCLES(10);
}

static void cpu_JPO()
{
  uint16_t addr = MEM_READ_WORD(regPC);
  if ( !(regS & PS_PARITY) ) regPC = addr; else regPC += 2;
  TIMER_ADD_CYCLES(10);
}

static void cpu_JPE()
{
  uint16_t addr = MEM_READ_WORD(regPC);
  if ( (regS & PS_PARITY) ) regPC = addr; else regPC += 2;
  TIMER_ADD_CYCLES(10);
}

static void cpu_JP()
{
  uint16_t addr = MEM_READ_WORD(regPC);
  if ( !(regS & PS_SIGN) ) regPC = addr; else regPC += 2;
  TIMER_ADD_CYCLES(10);
}

static void cpu_JM()
{
  uint16_t addr = MEM_READ_WORD(regPC);
  if ( (regS & PS_SIGN) ) regPC = addr; else regPC += 2;
  TIMER_ADD_CYCLES(10);
}

static void cpu_CNZ()
{
  uint16_t addr = MEM_READ_WORD(regPC);
  regPC += 2;
  if ( !(regS & PS_ZERO) )
  {
    pushPC();
    regPC = addr;
    TIMER_ADD_CYCLES(17);
  }
  else
  {
    TIMER_ADD_CYCLES(11);
  }
}

static void cpu_CZ()
{
  uint16_t addr = MEM_READ_WORD(regPC);
  regPC += 2;
  if ( (regS & PS_ZERO) )
  {
    pushPC();
    regPC = addr;
    TIMER_ADD_CYCLES(17);
  }
  else
  {
    TIMER_ADD_CYCLES(11);
  }
}

static void cpu_CNC()
{
  uint16_t addr = MEM_READ_WORD(regPC);
  regPC += 2;
  if ( !(regS & PS_CARRY) )
  {
    pushPC();
    regPC = addr;
    TIMER_ADD_CYCLES(17);
  }
  else
  {
    TIMER_ADD_CYCLES(11);
  }
}

static void cpu_CC()
{
  uint16_t addr = MEM_READ_WORD(regPC);
  regPC += 2;
  if ( (regS & PS_CARRY) )
  {
    pushPC();
    regPC = addr;
    TIMER_ADD_CYCLES(17);
  }
  else
  {
    TIMER_ADD_CYCLES(11);
  }
}

static void cpu_CPO()
{
  uint16_t addr = MEM_READ_WORD(regPC);
  regPC += 2;
  if ( !(regS & PS_PARITY) )
  {
    pushPC();
    regPC = addr;
    TIMER_ADD_CYCLES(17);
  }
  else
  {
    TIMER_ADD_CYCLES(11);
  }
}

static void cpu_CPE()
{
  uint16_t addr = MEM_READ_WORD(regPC);
  regPC += 2;
  if ( (regS & PS_PARITY) )
  {
    pushPC();
    regPC = addr;
    TIMER_ADD_CYCLES(17);
  }
  else
  {
    TIMER_ADD_CYCLES(11);
  }
}

static void cpu_CP()
{
  uint16_t addr = MEM_READ_WORD(regPC);
  regPC += 2;
  if ( !(regS & PS_SIGN) )
  {
    pushPC();
    regPC = addr;
    TIMER_ADD_CYCLES(17);
  }
  else
  {
    TIMER_ADD_CYCLES(11);
  }
}

static void cpu_CM()
{
  uint16_t addr = MEM_READ_WORD(regPC);
  regPC += 2;
  if ( (regS & PS_SIGN) )
  {
    pushPC();
    regPC = addr;
    TIMER_ADD_CYCLES(17);
  }
  else
  {
    TIMER_ADD_CYCLES(11);
  }
}

static void cpu_SHLD()
{
  uint16_t addr = MEM_READ_WORD(regPC);
  MEM_WRITE(addr,   regL);
  MEM_WRITE(addr + 1u, regH);
  regPC += 2;
  TIMER_ADD_CYCLES(16);
}

static void cpu_SPHL()
{
  regSP = regHL.HL;
  TIMER_ADD_CYCLES(5);
}

static void cpu_STA() {
  uint16_t addr = MEM_READ_WORD(regPC);
  MEM_WRITE(addr, regA);
  regPC += 2;
  TIMER_ADD_CYCLES(13);
}

#define CPU_STX(REG) \
  static void cpu_STX ## REG() \
  { \
    MEM_WRITE(reg##REG.REG, regA); \
    TIMER_ADD_CYCLES(7); \
  }

CPU_STX(BC);
CPU_STX(DE);


static void cpu_STC()
{
  regS |= PS_CARRY;
  TIMER_ADD_CYCLES(4);
}

static void cpu_XTHL()
{
  byte b;
  b = MEM_READ(regSP + 1u); MEM_WRITE(regSP + 1u, regH); regH = b;
  b = MEM_READ(regSP);    MEM_WRITE(regSP,    regL); regL = b;
  TIMER_ADD_CYCLES(18);
}

static void cpu_XCHG()
{
  byte b;
  b = regD; regD = regH; regH = b;
  b = regE; regE = regL; regL = b;
  TIMER_ADD_CYCLES(5);
}

static void cpu_OUT() {
  altair_out(MEM_READ(regPC), regA);
  TIMER_ADD_CYCLES(10);
  regPC++;
}

static void cpu_IN() {
  regA = altair_in(MEM_READ(regPC));
  TIMER_ADD_CYCLES(10);
  regPC++;
}

static void cpu_print_status_register(byte s) {
  if ( s & PS_SIGN )      Serial.print('S'); else Serial.print('.');
  if ( s & PS_ZERO )      Serial.print('Z'); else Serial.print('.');
  Serial.print('.');
  if ( s & PS_HALFCARRY ) Serial.print('A'); else Serial.print('.');
  Serial.print('.');
  if ( s & PS_PARITY )    Serial.print('P'); else Serial.print('.');
  Serial.print('.');
  if ( s & PS_CARRY )     Serial.print('C'); else Serial.print('.');
}

void cpucore_i8080_print_registers() {
  Serial.print(F("++ PC   = ")); numsys_print_word(regPC);
  Serial.print(F(" = ")); numsys_print_mem(regPC, 3, true);
  Serial.println();
  Serial.print(F("++ SP   = ")); numsys_print_word(regSP);
  Serial.print(F(" = ")); numsys_print_mem(regSP, 8, true);
  Serial.println();
  Serial.print(F("++ regS = "));   numsys_print_byte(regS);
  Serial.print(F(" = ")); cpu_print_status_register(regS);
  Serial.println();
  // ---
  Serial.print(F("+ regA: "));
  printData(regA);
  Serial.println("");
  // ---
  Serial.print(F("+ regB: "));
  printData(regB);
  Serial.print(F("  regC: "));
  printData(regC);
  Serial.println("");
  // ---
  Serial.print(F("+ regD: "));
  printData(regD);
  Serial.print(F("  regE: "));
  printData(regE);
  Serial.println("");
  // ---
  Serial.print(F("+ regH: "));
  printData(regH);
  Serial.print(F("  regL: "));
  printData(regL);
  // ---
  Serial.println();
}

CPUFUN cpucore_i8080_opcodes[256] = {
  cpu_NOP,   cpu_LXIBC, cpu_STXBC, cpu_INXBC, cpu_INRB,  cpu_DCRB,  cpu_MVBI,  cpu_RLC,		// 000-007 (0x00-0x07)
  cpu_NOP,   cpu_DADBC, cpu_LDXBC, cpu_DCXBC, cpu_INRC,  cpu_DCRC,  cpu_MVCI,  cpu_RRC,		// 010-017 (0x08-0x0F)
  cpu_NOP,   cpu_LXIDE, cpu_STXDE, cpu_INXDE, cpu_INRD,  cpu_DCRD,  cpu_MVDI,  cpu_RAL,		// 020-027 (0x10-0x17)
  cpu_NOP,   cpu_DADDE, cpu_LDXDE, cpu_DCXDE, cpu_INRE,  cpu_DCRE,  cpu_MVEI,  cpu_RAR,		// 030-037 (0x18-0x1F)
  cpu_NOP,   cpu_LXIHL, cpu_SHLD,  cpu_INXHL, cpu_INRH,  cpu_DCRH,  cpu_MVHI,  cpu_DAA,		// 040-047 (0x20-0x27)
  cpu_NOP,   cpu_DADHL, cpu_LHLD,  cpu_DCXHL, cpu_INRL,  cpu_DCRL,  cpu_MVLI,  cpu_CMA,		// 050-057 (0x28-0x2F)
  cpu_NOP,   cpu_LXIS,  cpu_STA,   cpu_INXSP, cpu_INRM,  cpu_DCRM,  cpu_MVMI,  cpu_STC,		// 060-067 (0x30-0x37)
  cpu_NOP,   cpu_DADS,  cpu_LDA,   cpu_DCXSP, cpu_INRA,  cpu_DCRA,  cpu_MVAI,  cpu_CMC,		// 070-077 (0x38-0x3F)

  cpu_MVBB,  cpu_MVBC,  cpu_MVBD,  cpu_MVBE,  cpu_MVBH,  cpu_MVBL,  cpu_MVBM,  cpu_MVBA,	// 100-107 (0x40-0x47)
  cpu_MVCB,  cpu_MVCC,  cpu_MVCD,  cpu_MVCE,  cpu_MVCH,  cpu_MVCL,  cpu_MVCM,  cpu_MVCA,       	// 110-117 (0x48-0x4F)
  cpu_MVDB,  cpu_MVDC,  cpu_MVDD,  cpu_MVDE,  cpu_MVDH,  cpu_MVDL,  cpu_MVDM,  cpu_MVDA,	// 120-127 (0x50-0x57)
  cpu_MVEB,  cpu_MVEC,  cpu_MVED,  cpu_MVEE,  cpu_MVEH,  cpu_MVEL,  cpu_MVEM,  cpu_MVEA,	// 130-137 (0x58-0x5F)
  cpu_MVHB,  cpu_MVHC,  cpu_MVHD,  cpu_MVHE,  cpu_MVHH,  cpu_MVHL,  cpu_MVHM,  cpu_MVHA,	// 140-147 (0x60-0x67)
  cpu_MVLB,  cpu_MVLC,  cpu_MVLD,  cpu_MVLE,  cpu_MVLH,  cpu_MVLL,  cpu_MVLM,  cpu_MVLA,	// 150-157 (0x68-0x6F)
  cpu_MVMB,  cpu_MVMC,  cpu_MVMD,  cpu_MVME,  cpu_MVMH,  cpu_MVML,  cpu_HLT,   cpu_MVMA,	// 160-167 (0x70-0x77)
  cpu_MVAB,  cpu_MVAC,  cpu_MVAD,  cpu_MVAE,  cpu_MVAH,  cpu_MVAL,  cpu_MVAM,  cpu_MVAA,	// 170-177 (0x78-0x7F)

  cpu_ADDB,  cpu_ADDC,  cpu_ADDD,  cpu_ADDE,  cpu_ADDH,  cpu_ADDL,  cpu_ADDM,  cpu_ADDA,	// 200-207 (0x80-0x87)
  cpu_ADCB,  cpu_ADCC,  cpu_ADCD,  cpu_ADCE,  cpu_ADCH,  cpu_ADCL,  cpu_ADCM,  cpu_ADCA,	// 210-217 (0x88-0x8F)
  cpu_SUBB,  cpu_SUBC,  cpu_SUBD,  cpu_SUBE,  cpu_SUBH,  cpu_SUBL,  cpu_SUBM,  cpu_SUBA,	// 220-227 (0x90-0x97)
  cpu_SBBB,  cpu_SBBC,  cpu_SBBD,  cpu_SBBE,  cpu_SBBH,  cpu_SBBL,  cpu_SBBM,  cpu_SBBA,	// 230-237 (0x98-0x9F)
  cpu_ANAB,  cpu_ANAC,  cpu_ANAD,  cpu_ANAE,  cpu_ANAH,  cpu_ANAL,  cpu_ANAM,  cpu_ANAA,	// 240-247 (0xA0-0xA7)
  cpu_XRAB,  cpu_XRAC,  cpu_XRAD,  cpu_XRAE,  cpu_XRAH,  cpu_XRAL,  cpu_XRAM,  cpu_XRAA,	// 250-257 (0xA8-0xAF)
  cpu_ORAB,  cpu_ORAC,  cpu_ORAD,  cpu_ORAE,  cpu_ORAH,  cpu_ORAL,  cpu_ORAM,  cpu_ORAA,        // 260-267 (0xB0-0xB7)
  cpu_CMPB,  cpu_CMPC,  cpu_CMPD,  cpu_CMPE,  cpu_CMPH,  cpu_CMPL,  cpu_CMPM,  cpu_CMPA,	// 270-277 (0xB8-0xBF)

  cpu_RNZ,   cpu_POPBC, cpu_JNZ,   cpu_JMP,   cpu_CNZ,   cpu_PSHBC, cpu_ADI,   cpu_RST00,	// 300-307 (0xC0-0xC7)
  cpu_RZ,    cpu_RET,   cpu_JZ,    cpu_JMP,   cpu_CZ,    cpu_CALL,  cpu_ACI,   cpu_RST08,	// 310-317 (0xC8-0xCF)
  cpu_RNC,   cpu_POPDE, cpu_JNC,   cpu_OUT,   cpu_CNC,   cpu_PSHDE, cpu_SUI,   cpu_RST10,	// 320-327 (0xD0-0xD7)
  cpu_RC,    cpu_RET,   cpu_JC,    cpu_IN,    cpu_CC,    cpu_CALL,  cpu_SBI,   cpu_RST18,	// 330-337 (0xD8-0xDF)
  cpu_RPO,   cpu_POPHL, cpu_JPO,   cpu_XTHL,  cpu_CPO,   cpu_PSHHL, cpu_ANI,   cpu_RST20,	// 340-347 (0xE0-0xE7)
  cpu_RPE,   cpu_PCHL,  cpu_JPE,   cpu_XCHG,  cpu_CPE,   cpu_CALL,  cpu_XRI,   cpu_RST28,	// 350-357 (0xE8-0xEF)
  cpu_RP,    cpu_POPAS, cpu_JP,    cpu_DI,    cpu_CP,    cpu_PSHAS, cpu_ORI,   cpu_RST30,	// 360-367 (0xF0-0xF7)
  cpu_RM,    cpu_SPHL,  cpu_JM,    cpu_EI,    cpu_CM,    cpu_CALL,  cpu_CPI,   cpu_RST38	// 370-377 (0xF8-0xFF)
};
