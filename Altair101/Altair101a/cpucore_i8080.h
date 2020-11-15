// -----------------------------------------------------------------------------
// Altair 8800 Simulator
// Copyright (C) 2017 David Hansel
// -----------------------------------------------------------------------------

#include <Arduino.h>
#include "Altair101a.h"

#ifndef CPUCORE_I8080_H
#define CPUCORE_I8080_H

// extern CPUFUN cpucore_i8080_opcodes[256];
void cpucore_i8080_print_registers();

#endif

// -----------------------------------------------------------------------------
// host.h
// -----------------------------------------------------------------------------

#ifndef HOST_H
#define HOST_H

#if defined(__AVR_ATmega2560__)
#include "host_mega.h"
#elif defined(__SAM3X8E__)
#include "host_due.h"
#else
#error requires Arduino Mega2560, Arduino Due
#endif

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

#endif

// -----------------------------------------------------------------------------
// config.h
// -----------------------------------------------------------------------------

#ifndef CONFIG_H
#define CONFIG_H


// If this is set to 1 and the host provides a file system (i.e. an SD card is connected)
// then store configurations and other data directly on the hosts' file system,
// Otherwise those items are stored in a persistent memory block using our own mini-filesystem
#define USE_HOST_FILESYS 0


// Allowing breakpoints significantly reduces performance but is helpful
// for debugging.  Also uses 2*MAX_BREAKPOINTS+1 bytes of RAM
#define MAX_BREAKPOINTS 0


// Setting USE_PROFILING_DETAIL to 1 will (every 10 seconds) show a 
// list of which opcodes were executed how many times (if profiling is enabled).
// Reduces performance and uses 1k of RAM
#define USE_PROFILING_DETAIL 0


// Enables throttling of CPU speed. This only makes sense to enable
// on the Due since the Mega is too slow anyways and the throttling 
// checks would only reduce performance further.
// #define USE_THROTTLE 1
#define USE_THROTTLE 0        // Stacy, for standalone test.

// Maximum number of ROMs that can be added. 
// Uses 13+(15*MAX_NUM_ROMS) bytes of RAM for organizational data. The actual 
// ROM content is stored in the emulated RAM and therefore does not occupy any 
// additional memory on the host. Set to 0 to completely disable ROM support.
// #define MAX_NUM_ROMS 8
#define MAX_NUM_ROMS 0    // Stacy, for standalone test.

// Enables support for MITS disk drives (maximum 16). Each drive uses about
// 160 bytes of RAM. Set to 0 to completely disable drive support.
// #define NUM_DRIVES 4
#define NUM_DRIVES 0  // Stacy, for standalone test.

// Enables support for Cromemco disk drives (maximum 4).
// Set to 0 to disable Cromemco drive support.
#define NUM_CDRIVES 0


// Enables support for Tarbell disk drives (maximum 4).
// Set to 0 to completely disable Tarbell drive support.
#define NUM_TDRIVES 0


// Enables support for hard disk (88-HDSK). Hard disk support uses
// about 1100 bytes of RAM plus 56 bytes for each unit.
// Set to 0 to completely disable hard disk support
// #define NUM_HDSK_UNITS 1
#define NUM_HDSK_UNITS 0    // Stacy, for standalone test.

// Enables printer emulation which uses about 140 bytes of RAM.
// #define USE_PRINTER 1
#define USE_PRINTER 0    // Stacy, for standalone test.

// Enable two 88-2SIO devices (instead of one).
#define USE_SECOND_2SIO 0


// Enables support for Cromemco Dazzler. Note that to actually see the
// Dazzler picture you need to connect a client. See:
// https://www.hackster.io/david-hansel/dazzler-display-for-altair-simulator-3febc6
#define USE_DAZZLER 0


// Enables support for Processor Technology VDM-1. Note that to actually see the
// VDM-1 picture you need to connect to a client. See:
// https://github.com/dhansel/VDM1
#define USE_VDM1 0


// Enables support for the external I/O bus described in the documentation.
// Enabling this slightly changes the LED behavior during input/output operations
// to facilitate I/O through the Arduino pins connected to the D0-7 LEDs.
// See the "Interfacing external hardware" section in the documentation for details.
#define USE_IO_BUS 0


// To improve performance, the MEMR LED handling is a bit lazy while a program is
// running. Memory reads are by far the most common bus action and any tiny
// bit of time that can be cut here makes a significant performance difference.
// Setting USE_REAL_MREAD_TIMING to 1 will improve the accuracy of MREAD at the
// cost of performace. Leaving this at 0 has virtually no visible consequences
// apart from a slight difference in brightness of the MEMR LED while running.
// Setting it to 1 significantly reduces performance.
// Most users should keep this at 0
#define USE_REAL_MREAD_TIMING 0


// If enabled, the D0-7 LEDs will show values being output to the data bus
// during memory write operations). This is different from the original
// Altair behavior where the D0-7 LEDs were all on for write operations
// because the LEDs are wired to the DIN bus lines which are floating during
// CPU write). Additionally, enabling this makes sure that the "WO" LED will
// go out (negative logic) AFTER the address and data buses have been set to
// the proper values. It also changes timing of the "WO" LED similar to
// the USE_REAL_MREAD_TIMING setting above.
// Enable this if you want to connect external hardware that needs to see
// data during memory write instructions or if you want to see the data
// during writes and do not care that this behavior does not match the original.
#define SHOW_MWRITE_OUTPUT 0


// If enabled, Address switch state will be set by issuing the '/'
// serial command.  Actual switches will be ignored.
// Useful when operating while not connected to the front panel hardware.
// #define STANDALONE 0
#define STANDALONE 1    // Stacy set to 1 for standalone testing.

// ------------------------------------------------------------------------------

#define CF_THROTTLE     0x01
#define CF_PROFILE      0x02
#define CF_SERIAL_PANEL 0x04
#define CF_SERIAL_INPUT 0x08
#define CF_SERIAL_DEBUG 0x10
#define CF_CLEARMEM     0x20
#define CF_HAVE_VI      0x40
#define CF_DRIVE_RT     0x80
#define CF_PRINTER_RT   0x00020000
#define CF_HDSK_RT      0x00040000

#define CSM_SIO         0
#define CSM_ACR         1
#define CSM_2SIO1       2
#define CSM_2SIO2       3
#define CSM_2SIO3       4
#define CSM_2SIO4       5

#define CSF_OFF         0
#define CSF_ON          1
#define CSF_AUTO        2

#define CSFB_NONE       0
#define CSFB_UNDERSCORE 1
#define CSFB_DELETE     3
#define CSFB_AUTO       2

#define CP_NONE         0
#define CP_OKI          1
#define CP_C700         2
#define CP_GENERIC      3


#if USE_SECOND_2SIO>0
#define NUM_SERIAL_DEVICES 6
#else
#define NUM_SERIAL_DEVICES 4
#endif

extern uint32_t config_flags, config_flags2;
extern uint32_t config_serial_settings;
extern uint32_t config_interrupt_mask;
extern uint32_t config_interrupt_vi_mask[8];
extern byte     config_serial_sim_to_host[NUM_SERIAL_DEVICES];

void config_setup(int n = 0);
void config_edit();
void config_defaults(bool apply);
byte config_get_current();

#if USE_THROTTLE>0
int     config_throttle(); // 0=off, <0=auto delay, >0=manual delay
#else
#define config_throttle() 0
#endif

inline bool config_profiling_enabled()        { return (config_flags & CF_PROFILE)!=0; }
inline bool config_clear_memory()             { return (config_flags & CF_CLEARMEM)!=0; }
inline bool config_serial_panel_enabled()     { return (config_flags & CF_SERIAL_PANEL)!=0; }
inline bool config_serial_input_enabled()     { return (config_flags & CF_SERIAL_INPUT)!=0; }
inline bool config_serial_debug_enabled()     { return (config_flags & CF_SERIAL_DEBUG)!=0; }
inline bool config_have_vi()                  { return (config_flags & CF_HAVE_VI)!=0; }

float    config_rtc_rate();

uint32_t config_host_serial_baud_rate(byte iface);
uint32_t config_host_serial_config(byte iface);
byte     config_host_serial_primary();

#endif
// -----------------------------------------------------------------------------
// cpucore.h
// -----------------------------------------------------------------------------

#ifndef CPUCORE_H
#define CPUCORE_H

#define PS_CARRY       0x01
#define PS_PARITY      0x04
#define PS_HALFCARRY   0x10
#define PS_ZERO        0x40
#define PS_SIGN        0x80


// The emulater runs at 2MHz
#define CPU_CLOCK_I8080 2000

extern union unionAF
{
  struct { byte A, F; };
  uint16_t AF;
} regAF;

extern union unionBC
{
  struct { byte C, B; };
  uint16_t BC;
} regBC;

extern union unionDE
{
  struct { byte E, D; };
  uint16_t DE;
} regDE;

extern union unionHL
{
  struct { byte L, H; };
  uint16_t HL;
} regHL;

extern union unionPC
{
  struct { byte L, H; };
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

void cpu_setup();
void cpu_print_registers();

#endif

// -----------------------------------------------------------------------------
// Altair 8800 Simulator
// Copyright (C) 2017 David Hansel
// -----------------------------------------------------------------------------

#ifndef MEM_H
#define MEM_H

extern byte Mem[MEMSIZE];
void mem_print_layout();

#if MEMSIZE < 0x10000
// if we have less than 64k of RAM then always map ROM basic to 0xC000-0xFFFF
// Stacy: #define MREAD(a)    ((a)>=0xC000 ? prog_basic_read_16k(a) : ((a) < MEMSIZE ? Mem[a] : 0xFF))
#define MREAD(a)    ((a)>=0xC000 ? 0xC000 : ((a) < MEMSIZE ? Mem[a] : 0xFF))
#define MWRITE(a,v) {Mem[a]=v;}
#else
// If we have 64k of RAM then we just copy ROM basic to the upper 16k and write-protect
// that area.  Faster to check the address on writing than reading since there are far more
// reads than writes. Also we can skip memory bounds checking because addresses are 16 bit.
#define MREAD(a)    (Mem[a])

#if USE_DAZZLER>0
#include "dazzler.h"
#if USE_VDM1>0
#include "vdm1.h"
#define MWRITE(a,v) { dazzler_write_mem(a, v); vdm1_write_mem(a, v); Mem[a]=v; }
#else
#define MWRITE(a,v) { dazzler_write_mem(a, v); Mem[a]=v; }
#endif
#elif USE_VDM1>0
#include "vdm1.h"
#define MWRITE(a,v) { vdm1_write_mem(a, v); Mem[a]=v; }
#else
#define MWRITE(a,v) { Mem[a]=v; }
#endif

#endif

byte MEM_READ_STEP(uint16_t a);
void MEM_WRITE_STEP(uint16_t a, byte v);

// WARNING: arguments to MEM_READ and MEM_WRITE macros should not have side effects
// (e.g. MEM_READ(addr++)) => any side effects will be executed multiple times!

#if USE_REAL_MREAD_TIMING>0
inline byte MEM_READ(uint16_t a)
{
  byte res;
  if( host_read_status_led_WAIT() )
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

#if SHOW_MWRITE_OUTPUT>0
inline void MEM_WRITE(uint16_t a, byte v)
{
  if( host_read_status_led_WAIT() )
    MEM_WRITE_STEP(a,v );
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


// manage ROMs
#define MEM_ROM_FLAG_TEMP      0x01
#define MEM_ROM_FLAG_AUTOSTART 0x04
#define MEM_ROM_FLAG_DISABLED  0x08

bool mem_add_rom(uint16_t start, uint16_t length, const char *name = NULL, uint16_t flags = 0, uint32_t filepos = 0);
void mem_set_rom_filepos(byte i, uint32_t pos);
bool mem_remove_rom(byte i, bool clear = true);
byte mem_get_num_roms(bool includeTemp = true);
void mem_set_rom_flags(byte i, uint16_t flags);
bool mem_get_rom_info(byte i, char *name = NULL, uint16_t *start = NULL, uint16_t *length = NULL, uint16_t *flags = NULL);
void mem_disable_rom(byte i);
void mem_disable_rom(const char *name);
uint16_t mem_get_rom_autostart_address();
void mem_clear_roms();
void mem_reset_roms();
void mem_restore_roms();
byte mem_find_rom(const char *name);


// set the highest address to be treated as RAM (everything above is ROM)
void     mem_set_ram_limit_usr(uint16_t a);
uint16_t mem_get_ram_limit_usr();

void mem_ram_init(uint16_t from, uint16_t to, bool force_clear = false);

void mem_setup();

#endif

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

#ifdef __AVR_ATmega2560__
// on Arduino Mega do not use String as it requires heap space
// and RAM is in short supply. This function is used exclusively for
// logging via Serial.print, so returning the number directly will still
// work, it just will always be printed as decimal
#define  numsys_byte2string(i) i
#else
String   numsys_byte2string(byte b);
#endif

#endif

// -----------------------------------------------------------------------------
// timer.h
// -----------------------------------------------------------------------------

#ifndef TIMER_H
#define TIMER_H

// timers 0-4 must represent the four serial devices
#define TIMER_SIO      0
#define TIMER_ACR      1
#define TIMER_2SIO1    2
#define TIMER_2SIO2    3
#define TIMER_2SIO3    4
#define TIMER_2SIO4    5
#define TIMER_RTC      6
#define TIMER_PRINTER  7
#define TIMER_PROFILE  8
// timers below here are not needed for Arduino MEGA build
#define TIMER_THROTTLE 9
#define TIMER_DRIVE    10
#define TIMER_HDSK     11
#define TIMER_VDM1     12


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

#endif

// -----------------------------------------------------------------------------
