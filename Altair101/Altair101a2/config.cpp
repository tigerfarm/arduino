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

#include "Altair101a.h"
#include "config.h"
#include "mem.h"
// #include "cpucore.h"
// #include "numsys.h"

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
  return (v >> ((uint32_t) i)) & ((1ul<<n)-1);
}

inline uint32_t set_bits(uint32_t v, byte i, byte n, uint32_t nv)
{
  uint32_t mask = ((1ul<<n)-1) << i;
  return (v & ~mask) | ((nv << i) & mask);
}

static uint32_t toggle_bits(uint32_t v, byte i, byte n, byte min = 0x00, byte max = 0xff)
{
  byte b = get_bits(v, i, n) + 1;
  return set_bits(v, i, n, b>max ? min : (b<min ? min : b));
}

#if USE_THROTTLE>0
int config_throttle()
{
  if( config_flags & CF_THROTTLE )
    {
      int i = get_bits(config_flags, 12, 5);
      if( i==0 )
        return -1; // auto
      else
        return i;  // manual
    }
  else
    return 0; // off
}
#endif

// --------------------------------------------------------------------------------

static void set_cursor(byte row, byte col)
{
  Serial.print(F("\033["));
  Serial.print(row);
  Serial.print(';');
  Serial.print(col);
  Serial.print(F("H\033[K"));
}

static void print_cpu()
{
  if( config_use_z80() )
    Serial.print(F("Zilog Z80"));
  else
    Serial.print(F("Intel 8080"));
}

static void print_mem_size(uint32_t s, byte row=0, byte col=0)
{
  if( row!=0 || col!=0 ) set_cursor(row, col);

  if( (s&0x3FF)==0 )
    {
      Serial.print(s/1024); 
      Serial.print(F(" KB"));
    }
  else
    {
      Serial.print(s); 
      Serial.print(F(" bytes"));
    }
}

static void print_flag(uint32_t data, uint32_t value, byte row=0, byte col=0)
{
  if( row!=0 || col!=0 ) set_cursor(row, col);
  Serial.print((data&value)!=0 ? F("yes") : F("no"));
}


static void print_flag(uint32_t value, byte row=0, byte col=0)
{
  print_flag(config_flags, value, row, col);
}


static void print_flag2(uint32_t value, byte row=0, byte col=0)
{
  print_flag(config_flags2, value, row, col);
}


static void print_vi_flag()
{
  Serial.print((config_flags&CF_HAVE_VI)!=0 ? F("Use Vector Interrupt board") : F("Interrupts connected directly to CPU"));
}


static void print_throttle(byte row = 0, byte col = 0)
{
  if( row!=0 || col!=0 ) set_cursor(row, col);

  int i = config_throttle();
  if     ( i<0  ) Serial.print(F("auto adjust"));
  else if( i==0 ) Serial.print(F("off"));
  else            Serial.print(i);
}

// --------------------------------------------------------------------------------

static bool save_config(byte fileno)
{
  return false;
}

static bool load_config(byte fileno)
{
  return false;
}

// --------------------------------------------------------------------------------
void config_defaults(bool apply)
{
  byte i, j;
  // default settings:
  // - SERIAL_DEBUG, SERIAL_INPUT, SERIAL_PANEL enabled if in STANDALONE mode, otherwise disabled
  // - Profiling disabled
  // - Throttling enabled (on Due)

  config_current = 0;
  config_flags = 0;
#if STANDALONE>0
  config_flags |= CF_SERIAL_DEBUG;
  config_flags |= CF_SERIAL_INPUT;
  config_flags |= CF_SERIAL_PANEL;
#endif
  config_flags |= CF_THROTTLE;

  config_flags2  = 0;              // Dazzler not enabled
  config_flags2 |= 0 << 3;         // VDM-1 not enabled
  config_flags2 |= B00110110 << 6; // VDM-1 DIP: 1=off, 2=on, 3=on, 4=off, 5=on, 6=on
  config_flags2 |= (0xCC00 >> 10) << 12; // VDM-1 base address : CC00

  new_config_serial_settings  = 0;
  new_config_serial_settings |= (0 << 8); // USB Programming port is primary interface


  uint32_t s = 0;
  for(byte dev=0; dev<NUM_SERIAL_DEVICES; dev++)
    config_serial_device_settings[dev] = s;

  // maximum amount of RAM supported by host
  config_mem_size = MEMSIZE; 

}

byte config_get_current()
{
  return config_current;
}

void config_setup(int n)
{

}
