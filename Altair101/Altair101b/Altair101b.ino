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

#include "Altair8800.h"
#include "config.h"
#include "cpucore.h"
#include "host.h"
#include "mem.h"
#include "serial.h"
#include "profile.h"
#include "numsys.h"
#include "timer.h"
#include "io.h"
// #include "breakpoint.h"
// #include "disassembler.h"
// #include "filesys.h"
// #include "drive.h"
// #include "tdrive.h"
// #include "cdrive.h"
// #include "hdsk.h"
// #include "printer.h"
// #include "prog.h"
// #include "dazzler.h"
// #include "vdm1.h"

#define BIT(n) (1<<(n))
#define b2s numsys_byte2string

uint16_t cswitch = 0;
uint16_t dswitch = 0;

static uint16_t p_regPC = 0xFFFF;

volatile uint32_t altair_interrupts_buf = 0;
volatile uint32_t altair_interrupts     = 0;
volatile byte     altair_vi_level_cur   = 0;
volatile byte     altair_vi_level       = 0;
volatile bool     altair_interrupts_enabled = false;
static   bool     altair_rtc_available  = false;
static   bool     altair_rtc_running    = false;

word status_wait = false;
bool have_ps2    = false;

void print_panel_serial(bool force = false);
void print_dbg_info();
void read_inputs_panel();
void read_inputs_serial();
void process_inputs();
void empty_input_buffer();
void rtc_setup();

#if USE_THROTTLE>0
uint32_t  throttle_micros = 0;
uint16_t  throttle_delay  = 0;

#define THROTTLE_TIMER_PERIOD 25000
void update_throttle()
{
  uint32_t now   = micros();
  uint32_t ratio = now == throttle_micros ? 0xffffffff : (2000 * THROTTLE_TIMER_PERIOD) / (now - throttle_micros);
  uint32_t t     = cpu_clock_KHz();

  if ( ratio > t + 10 )
    throttle_delay += (uint16_t) HOST_PERFORMANCE_FACTOR;
  else if ( ratio < t && throttle_delay > 0 )
    throttle_delay -= (uint16_t) HOST_PERFORMANCE_FACTOR;

  throttle_micros = now;
}
#endif


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
  altair_interrupt_disable();

  // wait while RESET switch is held
  while ( host_read_function_switch_debounced(SW_RESET) );

  // if in single-step mode we need to set a flag so we know
  // to exit out of the currently executing instruction
  if ( host_read_status_led_WAIT() ) cswitch |= BIT(SW_RESET);
}


byte get_device(byte switches)
{
  byte dev;
  if ( (switches & 0x80) == 0 )
    dev = serial_last_active_primary_device();
  else
    dev = (switches & 0x60) >> 5;

  return dev;
}


// -----------------------------------------------------------------------------
void read_inputs()
{
  cswitch = 0;
  read_inputs_panel();
  read_inputs_serial();
  print_panel_serial();
  process_inputs();
}

// -----------------------------------------------------------------------------
void process_inputs()
{
  if ( cswitch & BIT(SW_DEPOSIT) )
  {
    MWRITE(regPC, dswitch & 0xff);
    altair_set_outputs(regPC, MREAD(regPC));
  }
  else if ( cswitch & BIT(SW_DEPNEXT) )
  {
    regPC++;
    MWRITE(regPC, dswitch & 0xff);
    altair_set_outputs(regPC, MREAD(regPC));
  }

  if ( cswitch & BIT(SW_EXAMINE) )
  {
    regPC = dswitch;
    p_regPC = ~regPC;
    altair_set_outputs(regPC, MREAD(regPC));
  }
  else if ( cswitch & BIT(SW_EXNEXT) )
  {
    regPC = regPC + 1;
    altair_set_outputs(regPC, MREAD(regPC));
  }

  if ( cswitch & BIT(SW_RESET) )
  {
    altair_wait_reset();
  }

  if ( cswitch & BIT(SW_CLR) )
  {
    serial_close_files();
  }

  if ( cswitch & BIT(SW_RUN) )
  {
    if ( config_serial_debug_enabled() && config_serial_input_enabled() )
      Serial.print(F("\r\n\n--- RUNNING (press ESC twice to stop) ---\r\n\n"));
    host_clr_status_led_WAIT();
    host_clr_status_led_PROT();
  }
  if ( cswitch & (BIT(SW_PROTECT)) )
  {
    mem_protect(regPC);
    if ( mem_is_protected(regPC) ) host_set_status_led_PROT();
  }
  if ( cswitch & (BIT(SW_UNPROTECT)) )
  {
    mem_unprotect(regPC);
    if ( !mem_is_protected(regPC) ) host_clr_status_led_PROT();
  }
}

// -----------------------------------------------------------------------------
void altair_wait_step()
{
  cswitch &= BIT(SW_RESET); // clear everything but RESET status
  while ( host_read_status_led_WAIT() && (cswitch & (BIT(SW_STEP) | BIT(SW_SLOW) | BIT(SW_RESET))) == 0 )
  {
    read_inputs();
    delay(10);
  }

  if ( cswitch & BIT(SW_SLOW) ) delay(500);
}

// -----------------------------------------------------------------------------
void read_inputs_panel()
{
  // we react on positive edges on the function switches...
  cswitch = host_read_function_switches_edge();
  // ...except for the SLOW switch which is active as long as it is held down
  if ( host_read_function_switch_debounced(SW_SLOW) ) cswitch |= BIT(SW_SLOW);
#if STANDALONE==0
  // address switches on Mega are connected to analog inputs which are free
  // floating and therefore random when not connected
  dswitch = host_read_addr_switches();
#endif
}


byte read_device()
{
  byte dev = 0xff;

#if USE_SECOND_2SIO>0
  Serial.print(F("(s=SIO,a=ACR,1=2SIO-1,2=2SIO-2,3=2nd 2SIO-1,4=2nd 2SIO-2,SPACE=last,ESC=abort): "));
#else
  Serial.print(F("(s=SIO,a=ACR,1=2SIO-1,2=2SIO-2,SPACE=last,ESC=abort): "));
#endif

  int c = -1;
#if USE_SECOND_2SIO>0
  while ( c != 's' && c != 'a' && c != '1' && c != '2' && c != '3' && c != '4' && c != ' ' && c != 27 )
#else
  while ( c != 's' && c != 'a' && c != '1' && c != '2' && c != ' ' && c != 27 )
#endif
    c = serial_read();

  if ( c != 27 )
  {
    switch (c)
    {
      case 's': dev = CSM_SIO;   break;
      case 'a': dev = CSM_ACR;   break;
      case '1': dev = CSM_2SIO1; break;
      case '2': dev = CSM_2SIO2; break;
#if USE_SECOND_2SIO>0
      case '3': dev = CSM_2SIO3; break;
      case '4': dev = CSM_2SIO4; break;
#endif
      case ' ': dev = 0x80;      break;
    }

    Serial.print((char) c);
  }

  return dev;
}


void altair_dump_intel_hex(uint16_t start, uint16_t end)
{
  uint16_t a = start;
  while ( a <= end )
  {
    byte cs = 0;
    Serial.write(':');
    byte n = min(16, end - a + 1);
    numsys_print_byte_hex(n); cs -= n; // record length
    numsys_print_byte_hex(a >> 8); cs -= a >> 8; // address high byte
    numsys_print_byte_hex(a & 0xff); cs -= a & 0xff; // address low byte
    numsys_print_byte_hex(0); // record type (0=data record)
    for (uint16_t i = 0; i < n; i++)
    {
      byte b = MREAD(a + i);
      numsys_print_byte_hex(b);
      cs -= b;
    }
    numsys_print_byte_hex(cs);
    Serial.println();
    a += n;
    if ( a == 0 ) break;
  }
  Serial.println(F(":0000000000"));
}


bool altair_read_intel_hex(uint16_t *start, uint16_t *end)
{
  word aa = 0xffff;
  if ( start != NULL ) *start = 0xFFFF;
  if ( end != NULL   ) *end   = 0x0000;

  while ( 1 )
  {
    // expect line to start with a colon
    int c = -1;
    while ( c < 0 || c == 10 || c == 13 || c == ' ' || c == '\t' ) c = serial_read();
    if ( c != ':' ) return false;

    // initialize checksum
    byte cs = 0;

    // first byte is record length
    byte n = numsys_read_hex_byte(); cs -= n;

    // next two bytes are address
    byte d = numsys_read_hex_byte(); cs -= d;
    word a = numsys_read_hex_byte(); cs -= a;
    a = a + 256 * d;

    // next byte is record type
    byte r = numsys_read_hex_byte(); cs -= r;

    switch ( r )
    {
      case 0:
        {
          // data record

          if ( a != aa && n > 0 )
          {
            if ( aa != 0xffff ) {
              numsys_print_word(aa - 1);
              Serial.println();
            }
            numsys_print_word(a);
            aa = a;
          }
          aa += n;
          Serial.print('.');

          if ( n > 0 && start != NULL && a < *start ) *start = a;
          for (byte i = 0; i < n; i++)
          {
            d = numsys_read_hex_byte();
            MWRITE(a, d);
            cs -= d;
            a++;
          }
          if ( n > 0 && end != NULL && a - 1 > *end ) *end = a - 1;

          break;
        }

      default:
        {
          // unhandled record => skip
          for (byte i = 0; i < n; i++) cs += numsys_read_hex_byte();
          break;
        }
    }

    // test checksum
    byte csb = numsys_read_hex_byte();
    if ( cs != csb )
      return false; // checksum error

    // empty record means we're done
    if ( n == 0 )
    {
      numsys_print_word(aa - 1);
      Serial.println();
      return true;
    }
  }
}


void read_data()
{
  uint16_t addr, len;
  byte b;
  Serial.print(F("\r\n\nStart address: "));
  if ( numsys_read_word(&addr) )
  {
    Serial.print(F("\r\nNumber of bytes: "));
    if ( numsys_read_word(&len) )
    {
      Serial.print(F("\r\nData: "));
      while ( len > 0 )
      {
        Serial.write(' ');
        if ( !numsys_read_byte(&b) ) return;
        MWRITE(addr, (byte) b);
        ++addr;
        --len;
      }
    }
  }

  Serial.println();
}

void empty_input_buffer()
{
  while ( true )
  {
    if ( serial_read() < 0 ) {
      delay(15);
      if ( serial_read() < 0 ) {
        delay(150);
        if ( serial_read() < 0 ) break;
      }
    }
  }
}

// -----------------------------------------------------------------------------
void read_inputs_serial()
{
  if ( !config_serial_input_enabled() )
    return;

  int data = serial_read();
  if ( data < 0 )
    return;
#if STANDALONE>0
  else if ( data >= '0' && data <= '9' )
    dswitch = dswitch ^ (1 << (data - '0'));
  else if ( data >= 'a' && data <= 'f' )
    dswitch = dswitch ^ (1 << (data - 'a' + 10));
  else if ( data == '/' )
  {
    Serial.print(F("\r\nSet Addr switches to: "));
    numsys_read_word(&dswitch);
    Serial.println('\n');
  }
#endif
  else if ( data == 'X' )
    cswitch |= BIT(SW_EXAMINE);
  else if ( data == 'x' )
    cswitch |= BIT(SW_EXNEXT);
  else if ( data == 'P' )
    cswitch |= BIT(SW_DEPOSIT);
  else if ( data == 'p' )
    cswitch |= BIT(SW_DEPNEXT);
  else if ( data == 'r' )
    cswitch |= BIT(SW_RUN);
  else if ( data == 'o' || data == 27 )
    cswitch |= BIT(SW_STOP);
  else if ( data == 't' )
    cswitch |= BIT(SW_STEP);
  else if ( data == 'R' )
    cswitch |= BIT(SW_RESET);
  else if ( data == '!' )
    cswitch |= BIT(SW_RESET) | BIT(SW_STOP);
  else if ( data == 'U' )
    cswitch |= BIT(SW_AUX1UP);
  else if ( data == 'u' )
    cswitch |= BIT(SW_AUX1DOWN);
  else if ( data == '>' )
  {
    Serial.print(F("\r\nRun from address: "));
    if ( numsys_read_word(&regPC) )
    {
      regPC--;
      p_regPC = ~regPC;
      if ( config_serial_debug_enabled() )
        Serial.print(F("\r\n\n--- RUNNING (press ESC twice to stop) ---\r\n\n"));
      Serial.println();
      host_clr_status_led_WAIT();
      host_clr_status_led_PROT();
    }
    else
      Serial.println();
  }
  else if ( data == 'n' )
  {
    numsys_toggle();
    p_regPC = ~regPC;
    print_dbg_info();
  }
}

// -----------------------------------------------------------------------------
void print_panel_serial(bool force)
{
  byte dbus;
  static uint16_t p_dswitch = 0, p_cswitch = 0, p_abus = 0xffff, p_dbus = 0xffff, p_status = 0xffff;
  uint16_t status, abus;

  if ( !config_serial_panel_enabled() )
    return;

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
void print_dbg_info()
{
  if ( config_serial_debug_enabled() && host_read_status_led_WAIT() && regPC != p_regPC )
    cpu_print_registers();
}


// -----------------------------------------------------------------------------
void reset(bool resetPC)
{
  host_clr_status_led_INT();
  host_set_status_led_WO();
  host_clr_status_led_STACK();
  host_clr_status_led_HLTA();
  host_clr_status_led_OUT();
  host_clr_status_led_M1();
  host_clr_status_led_INP();
  host_clr_status_led_MEMR();
  host_clr_status_led_PROT();
  serial_update_hlda_led();
  altair_interrupt_disable();

  if ( resetPC )
  {
    uint16_t a = mem_get_rom_autostart_address();
    regPC      = (a == 0xffff) ? 0x0000 : a;
    p_regPC    = ~regPC;
  }

  serial_reset();

#if STANDALONE>0
  if ( cswitch & BIT(SW_STOP) )
#else
  if ( host_read_function_switch(SW_STOP) )
#endif
  {
    have_ps2 = false;

    // close all open files
    serial_close_files();
  }

  // remove ROMS that were temporarily installed by the Simulator
  // and restore auto-disable ROMS
  mem_reset_roms();

  altair_interrupts     = 0;
  altair_interrupts_buf = 0;
  // tdrive_reset();
  // cdrive_reset();
}


// -----------------------------------------------------------------------------
void switch_interrupt_handler()
{
  if ( altair_interrupts & INT_SW_STOP )
  {
    altair_interrupts &= ~INT_SW_STOP;
    host_clr_status_led_MEMR();
    host_clr_status_led_WO();
    host_set_status_led_WAIT();
    if ( !config_serial_debug_enabled() )  {}
    else if ( config_serial_panel_enabled() )
      // Stacy: Serial.print(F("\033[2J\033[14B\r\n------ STOP ------\r\n\n"));
      Serial.print(F("\r\n------ STOP ------\r\n\n"));
    else
      Serial.print(F("\r\n\n------ STOP ------\r\n\n"));
    p_regPC = ~regPC;
  }
  else if ( altair_interrupts & INT_SW_RESET )
  {
    altair_wait_reset();
    reset(true);
  }
  else
  {
    cswitch = 0;
    if ( altair_interrupts & INT_SW_CLR )      {
      cswitch |= BIT(SW_CLR);
      altair_interrupts &= ~INT_SW_CLR;
    }
    if ( altair_interrupts & INT_SW_AUX2UP )   {
      cswitch |= BIT(SW_AUX2UP);
      altair_interrupts &= ~INT_SW_AUX2UP;
    }
    if ( altair_interrupts & INT_SW_AUX2DOWN ) {
      cswitch |= BIT(SW_AUX2DOWN);
      altair_interrupts &= ~INT_SW_AUX2DOWN;
    }
#if STANDALONE==0
    dswitch = host_read_addr_switches();
#endif
    process_inputs();
  }
}



void altair_rtc_interrupt()
{
  altair_interrupt(INT_RTC);
}

// -----------------------------------------------------------------------------
void rtc_setup()
{
  float rate = config_rtc_rate();
  altair_rtc_available = rate > 0.0;

  if ( !altair_rtc_available )
  {
    timer_stop(TIMER_RTC);
    altair_rtc_running = false;
    //printf("RTC disabled\n");
  }
  else
  {
    timer_setup(TIMER_RTC, (uint32_t) (1000000.0 / rate), altair_rtc_interrupt);
    if ( altair_rtc_running ) timer_start(TIMER_RTC, 0, true);
    //printf("RTC enabled: %f %i\n", rate, (int) (1000000.0/rate));
  }
}


void altair_vi_check_interrupt()
{
  byte level = 0xff;

  if     ( altair_interrupts_buf & config_interrupt_vi_mask[0] ) level = 0;
  else if ( altair_interrupts_buf & config_interrupt_vi_mask[1] ) level = 1;
  else if ( altair_interrupts_buf & config_interrupt_vi_mask[2] ) level = 2;
  else if ( altair_interrupts_buf & config_interrupt_vi_mask[3] ) level = 3;
  else if ( altair_interrupts_buf & config_interrupt_vi_mask[4] ) level = 4;
  else if ( altair_interrupts_buf & config_interrupt_vi_mask[5] ) level = 5;
  else if ( altair_interrupts_buf & config_interrupt_vi_mask[6] ) level = 6;
  else if ( altair_interrupts_buf & config_interrupt_vi_mask[7] ) level = 7;

  if ( level < altair_vi_level_cur )
  {
    altair_vi_level    = level;
    altair_interrupts |= INT_VECTOR;
  }
}


void altair_vi_out_control(byte port, byte data)
{
  // set current interrupt level for vector interrupts
  altair_vi_level_cur = (data & 8) == 0 ? 0xff : (7 - (data & 7));

  // reset interrupt line from RTC
  if ( data & 0x10 )
    altair_interrupt(INT_RTC, false);

  // enable/disable real-time clock
  if ( ((data & 0x40) && !altair_rtc_running && altair_rtc_available) )
  {
    altair_rtc_running = true;
    timer_start(TIMER_RTC, 0, true);
  }
  else if ( !(data & 0x40) && altair_rtc_running )
  {
    altair_rtc_running = false;
    timer_stop(TIMER_RTC);
  }

  if ( altair_interrupts_enabled )
    altair_vi_check_interrupt();
}


void altair_vi_register_ports()
{
  io_register_port_out(0xfe, config_have_vi() ? altair_vi_out_control : NULL);
}


void altair_interrupt_enable()
{
  host_set_status_led_INTE();
  altair_interrupts_enabled = true;

  if ( config_have_vi() )
    altair_vi_check_interrupt();
  else
    altair_interrupts = (altair_interrupts & ~INT_DEVICE) | (altair_interrupts_buf & config_interrupt_mask);
}


bool altair_interrupt_enabled()
{
  return altair_interrupts_enabled;
}


void altair_interrupt_disable()
{
  host_clr_status_led_INTE();
  altair_interrupts_enabled = false;
  altair_interrupts &= ~INT_DEVICE;
}


void altair_interrupt(uint32_t i, bool set)
{
  if ( i & INT_DEVICE )
  {
    if ( set )
      altair_interrupts_buf |=  (i & INT_DEVICE);
    else
      altair_interrupts_buf &= ~(i & INT_DEVICE);

    if ( !altair_interrupts_enabled )
      altair_interrupts &= ~INT_DEVICE;
    else if ( config_have_vi() )
      altair_vi_check_interrupt();
    else
      altair_interrupts = (altair_interrupts & ~INT_DEVICE) | (altair_interrupts_buf & config_interrupt_mask);
  }

  if ( i & INT_SWITCH )
    altair_interrupts |= (i & INT_SWITCH);
}


bool altair_interrupt_active(uint32_t i)
{
  return (altair_interrupts_buf & i) != 0;
}


static byte altair_interrupt_handler()
{
  byte opcode = 0xff;

  host_set_status_led_M1();
  host_set_status_led_INT();
  host_clr_status_led_MEMR();

  // Determine the opcode to put on the data bus (if VI enabled).
  // Must do this before calling altair_interrupt_disable, otherwise
  // the INT_VECTOR flag is cleared already
  if ( altair_interrupts & INT_VECTOR )
  {
    opcode = 0307 | (altair_vi_level * 8);

    // the VI interrupt output line gets cleared as soon as the
    // CPU acknowledges the interrupt
    altair_interrupts &= ~INT_VECTOR;
  }

  // disable interrupts now
  altair_interrupt_disable();

  if ( host_read_status_led_WAIT() )
  {
    if ( config_serial_debug_enabled() ) {
      Serial.print(F("\r\nInterrupt! opcode="));
      numsys_print_byte(opcode);
      Serial.println();
    }
    altair_set_outputs(regPC, opcode);
    altair_wait_step();
  }
  else
    host_set_data_leds(opcode);

  host_clr_status_led_HLTA();
  host_clr_status_led_INT();
  host_clr_status_led_M1();

  return opcode;
}

// -----------------------------------------------------------------------------
bool altair_isreset()
{
  return (cswitch & BIT(SW_RESET)) == 0;
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


// -----------------------------------------------------------------------------
byte altair_read_sense_switches(byte port)
{
  byte data;

  // read SENSE switches
#if STANDALONE>0
  data = dswitch / 256;
#else
  static unsigned long debounceTimeout = 0;
  static byte debounceVal = 0;
  if ( millis() > debounceTimeout )
  {
    data = host_read_sense_switches();
    debounceVal = data;
    debounceTimeout = millis() + 20;
  }
  else
    data = debounceVal;
#endif

  return data;
}

// -----------------------------------------------------------------------------
void altair_out(byte port, byte data)
{
  host_set_addr_leds(port | port * 256);
  host_set_data_leds(data);
  host_set_status_led_OUT();
  host_set_status_led_WO();
  io_out(port, data);
  if ( host_read_status_led_WAIT() )
  {
    altair_set_outputs(port | port * 256, 0xff);
    altair_wait_step();
  }

  host_clr_status_led_OUT();
}

// -----------------------------------------------------------------------------
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
      data = io_inp(port);
      altair_set_outputs(port | port * 256, data);
      host_clr_status_led_INP();
      read_inputs();
      // advance simulation time (so timers can expire)
      TIMER_ADD_CYCLES(50);
    }
    if ( cswitch & BIT(SW_SLOW) ) delay(500);
  }
  else
  {
    host_set_status_led_INP();
    data = io_inp(port);
    host_set_data_leds(data);
    host_clr_status_led_INP();
  }
  return data;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void setup() {
  cswitch = 0;
  dswitch = 0;
  Serial.begin(115200);

  timer_setup();
  mem_setup();
  io_setup();
  host_setup();
  if ( host_read_function_switch(SW_RESET) )
    config_setup(-1);
  else if ( host_read_function_switch(SW_DEPOSIT) )
    config_setup(host_read_addr_switches());
  else
    config_setup(0);
  cpu_setup();
  serial_setup();
  profile_setup();
  rtc_setup();

  altair_vi_register_ports();
  io_register_port_inp(0xff, altair_read_sense_switches);

  // set up serial connection on the host
  for (byte i = 0 ; i < HOST_NUM_SERIAL_PORTS; i++)
    host_serial_setup(i, config_host_serial_baud_rate(i), config_host_serial_config(i),
                      config_host_serial_primary() == i);

  // emulator extra: holding down CLR at powerup will keep all registers
  // and memory content initialized with 0. Otherwise (as would be normal
  // with the Altair), everything is random.
  mem_ram_init(0, MEMSIZE - 1);

  config_clear_memory();  // Stacy, confirm sets to memory bytes to zero.
  /*
    if ( !host_read_function_switch(SW_CLR) && !config_clear_memory() )
    {
    regPC = host_get_random();
    regSP = host_get_random();
    regA  = host_get_random();
    regS  = host_get_random() & (PS_CARRY | PS_PARITY | PS_HALFCARRY | PS_ZERO | PS_SIGN);
    regB  = host_get_random();
    regC  = host_get_random();
    regD  = host_get_random();
    regE  = host_get_random();
    regH  = host_get_random();
    regL  = host_get_random();
    }
  */

  host_set_status_led_WAIT();
  reset(false);

  uint16_t a = mem_get_rom_autostart_address();
  if ( a != 0xFFFF )
  {
    regPC = a;
    host_clr_status_led_WAIT();
  }
}


// --------------------------------------------------------------------------------
void loop()
{
  byte opcode;

  // --------------------------------------------------------------------------------
  // if we are NOT in WAIT mode then enter the main simulation loop
  if ( !host_read_status_led_WAIT() )
  {
    // clear all switch-related interrupts before starting loop
    altair_interrupts &= ~INT_SWITCH;

    // enable/disable profiling
    profile_enable(config_profiling_enabled());

#if USE_THROTTLE>0
    if ( config_throttle() < 0 )
    {
      timer_setup(TIMER_THROTTLE, 0, update_throttle);
      timer_start(TIMER_THROTTLE, THROTTLE_TIMER_PERIOD, true);
      throttle_delay  = (uint16_t) (10.0 * HOST_PERFORMANCE_FACTOR);
      throttle_micros = micros();
    }
    else
      throttle_delay = (uint16_t) (config_throttle() * HOST_PERFORMANCE_FACTOR);
#endif

    while ( true )
    {
      // put PC on address bus LEDs
      host_set_addr_leds(regPC);

      // check for events that can't be handled by real interrupts
      // on the host (e.g. serial input on Mega)
      host_check_interrupts();

      if ( altair_interrupts )
      {
        // interrupt detected
        if ( altair_interrupts & INT_SWITCH )
        {
          // switch-related interrupt (simulation handling)
          p_regPC = ~regPC;
          switch_interrupt_handler();
          if ( host_read_status_led_WAIT() )
            break;    // exit simulation loop
          else
            continue; // start over
        }
        else
        {
          // ALTAIR interrupt
          opcode = altair_interrupt_handler();
        }
      }
      else
      {
        // no interrupt => read opcode, put it on data bus LEDs and advance PC
#if USE_REAL_MREAD_TIMING>0
        host_set_status_led_M1();
        opcode = MEM_READ(regPC);
#else
        host_set_status_leds_READMEM_M1();
        host_set_addr_leds(regPC);
        opcode = MREAD(regPC);
        host_set_data_leds(opcode);
#endif
        regPC++;
        host_clr_status_led_M1();
      }

      // take a CPU step
      PROFILE_COUNT_OPCODE(opcode);
      CPU_EXEC(opcode);

      // check for breakpoint hit
      // breakpoint_check(regPC);

#if USE_THROTTLE>0
      // delay execution according to current delay
      // (need the NOP, otherwise the compiler will optimize the loop away)
      for (uint32_t i = 0; i < throttle_delay; i++) asm("NOP");
#endif
    }

#if USE_THROTTLE>0
    timer_stop(TIMER_THROTTLE);
#endif

    // flush any characters stuck in the serial buffer
    // (so we don't accidentally execute commands after stopping)
    if ( config_serial_input_enabled() ) empty_input_buffer();
  }

  // --------------------------------------------------------------------------------
  // Wait mode.

  if ( cswitch & BIT(SW_RESET) )
  {
    reset(true);
    read_inputs();
  }

  print_dbg_info();
  p_regPC = regPC;

  host_set_status_led_M1();

  // check for events that can't be handled by real interrupts
  // on the host (e.g. serial input on Mega)
  host_check_interrupts();

  if ( altair_interrupts & INT_DEVICE )
  {
    opcode = altair_interrupt_handler();
  }
  else
  {
    opcode = MEM_READ(regPC);
    regPC++;
  }

  host_clr_status_led_M1();
  if ( !(cswitch & BIT(SW_RESET)) )
  {
    PROFILE_COUNT_OPCODE(opcode);
    CPU_EXEC(opcode);

    // if the PC has not changed (e.g. jump to the same address) then modify p_regPC
    // to force debugger display (otherwise there would be no feedback)
    if ( regPC == p_regPC ) p_regPC = ~regPC;

#if STANDALONE>0
    // handle STOP interrupts generated by HLT opcodes in standalone mode
    if ( altair_interrupts & INT_SW_STOP ) switch_interrupt_handler();
#endif
  }

  // --------------------------------------------------------------------------------
}

// --------------------------------------------------------------------------------