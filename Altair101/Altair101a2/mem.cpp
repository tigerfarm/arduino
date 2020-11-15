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
#include "mem.h"
#include "host.h"
#include "numsys.h"
#include "config.h"


word mem_ram_limit = 0xFFFF, mem_protected_limit = 0xFFFF;
byte mem_protected_flags[32];

byte Mem[MEMSIZE];


byte MEM_READ_STEP(uint16_t a)
{
  if( altair_isreset() )
    {
      byte v = MREAD(a);
      host_set_status_leds_READMEM();
      altair_set_outputs(a, v);
      altair_wait_step();
      v = host_read_data_leds(); // CPU reads whatever is on the data bus at this point
      host_clr_status_led_MEMR();
      return v;
    }
  else
    return 0x00;
}


void MEM_WRITE_STEP(uint16_t a, byte v)
{ 
  if( altair_isreset() )
    {
      MWRITE(a, v);
      host_set_status_leds_WRITEMEM();
#if SHOW_MWRITE_OUTPUT>0
      altair_set_outputs(a, v);
#else
      altair_set_outputs(a, 0xff);
#endif
      altair_wait_step();
      host_clr_status_led_WO();
    }
}


static bool mem_is_rom(uint16_t a)
{
  for(int i=0; i<mem_get_num_roms(); i++)
    {
      uint16_t pa, pl, pe;
      mem_get_rom_info(i, NULL, &pa, &pl);
      pa &= ~0xFF;
      pe  = ((pa+pl-1)|0xFF);
      if( pa>a )
        return false;
      else if( a>=pa && a<=pe )
        return true;
    }

  return false;
}


void mem_print_layout()
{
  for(uint32_t i=0; i<0x10000; i+=0x100)
    {
      if( (i&0x3FFF) == 0 )
        {
          Serial.println();
          numsys_print_word(i);
          Serial.print(F(": "));
        }

      if( MEM_IS_WRITABLE(i) ) 
        Serial.print('.');
      else if( i>=MEMSIZE )
        Serial.print('#');
      else if( mem_is_rom(i) )
        Serial.print('P');
      else if( i>mem_ram_limit )
        Serial.print('-');
      else
        Serial.print('U');
    }

  Serial.println();
}


static void mem_protect_calc_limit()
{
  mem_protected_limit = mem_ram_limit;
  for(byte i=0; i<32; i++)
    if( mem_protected_flags[i] )
      for(byte j=0; j<8; j++)
        if( mem_protected_flags[i] & (1<<j) )
          {
            mem_protected_limit = min(mem_ram_limit, uint16_t(2048*i + 256*j));
            return;
          }
}

static void mem_protect_flag_set(uint16_t a, bool set)
{
  if( set )
    mem_protected_flags[a>>11] |=  (1<<((a>>8)&0x07));
  else
    mem_protected_flags[a>>11] &= ~(1<<((a>>8)&0x07));
}


static void mem_protect_flags_set(uint16_t start, uint16_t length, bool v)
{
  for(uint32_t p = start; p < (uint32_t) start + length; p += 0x100 )
    mem_protect_flag_set(p, v);
  mem_protect_flag_set(start+length-1, v);
}



void mem_protect(uint16_t a)
{
  mem_protect_flag_set(a, true);
  mem_protect_calc_limit();
}


void mem_unprotect(uint16_t a)
{
  if( !mem_is_rom(a) )
    {
      mem_protect_flag_set(a, false);
      mem_protect_calc_limit();
    }
}


bool mem_is_protected(uint16_t a)
{
  return !MEM_IS_WRITABLE(a) && a<=mem_ram_limit && !mem_is_rom(a);
}


bool mem_is_writable(uint16_t from, uint16_t to)
{
  for(uint32_t p = (from & ~0xff); p <= (uint32_t) to; p += 0x100 ) 
    if( !MEM_IS_WRITABLE(p) )
      return false;

  return true;
}



static void randomize(uint32_t from, uint32_t to)
{
  // note that if from/to are not on 4-byte boundaries
  // then a few bytes remain unchanged
  from = (from&3)==0 ? from/4 : from/4+1;
  to   = to/4;
  for(word i=from; i<to; i++)((uint32_t *) Mem)[i] = host_get_random();
}


static void mem_ram_init_section(uint16_t from, uint16_t to, bool clear)
{
  if( from>mem_ram_limit )
    {
      // "from" is before the RAM limit and "to" is after
      // => initialize with 0xFF
      memset(Mem+from, 0xFF, to-from+1);
    }
  else if( to<=mem_ram_limit )
    {
      // "to" is before the RAM limit
      // => initialize RAM with either 0 or random
      if( clear )
        memset(Mem+from, 0x00, to-from+1);
      else
        randomize(from, to);
    }
  else
    {
      // "from" is before the RAM limit and "to" is after
      // => initialize up to the limit with 0 or random...
      if( clear )
        memset(Mem+from, 0x00, mem_ram_limit-from+1);
      else
        randomize(from, mem_ram_limit);

      // ...and memory after the limit with 0xFF
      memset(Mem+mem_ram_limit+1, 0xFF, to-mem_ram_limit);
    }
}


void mem_ram_init(uint16_t from, uint16_t to, bool force_clear)
{
  byte i;
  uint32_t a = from;
  uint16_t pa, pl;

  // initialize RAM before and between ROMs
  for(i=0; i<mem_get_num_roms() && a<=to; i++)
    {
      mem_get_rom_info(i, NULL, &pa, &pl);
      if( (uint32_t) pa+pl>a )
        {
          if( pa>a ) mem_ram_init_section(a, min(pa-1, to), force_clear || config_clear_memory());
          a = pa+pl;
        }
    }

  // initialize RAM after last ROM
  if( a<=to ) mem_ram_init_section(a, to, force_clear || config_clear_memory());
}


void mem_set_ram_limit_usr(uint16_t a)
{
  uint16_t prev_limit = mem_ram_limit;
  mem_ram_limit = min(a, MEMSIZE-1);

  if( prev_limit < mem_ram_limit )
    {
      mem_ram_init(prev_limit+1, mem_ram_limit);

      // un-protect RAM below the new mem_ram_limit
      for(uint32_t p = prev_limit+1; p < ((uint32_t) mem_ram_limit)+1; p += 0x100 )
        mem_protect_flag_set(p, false);

      // restore protection for ROMs
      for(byte i=0; i<mem_get_num_roms(); i++)
        {
          uint16_t pa, pl;
          mem_get_rom_info(i, NULL, &pa, &pl);
          if( pa>mem_ram_limit ) 
            break;
          else if( pa+pl>prev_limit )
            mem_protect_flags_set(pa, pl, true);
        }
    }
  else if( mem_ram_limit < prev_limit )
    {
      mem_ram_init(mem_ram_limit+1, prev_limit);
      
      // write-protect RAM above the new mem_ram_limit
      // it would be easier to have an additional condition (a<=mem_ram_limit) in the
      // MEM_IS_WRITABLE macro but doing so would slow emulation as writing to memory
      // is one of the most common operations.
      for(uint32_t p = mem_ram_limit+1; p < ((uint32_t) prev_limit)+1; p += 0x100 )
        mem_protect_flag_set(p, true);
    }
  
  mem_protect_calc_limit();
}


uint16_t mem_get_ram_limit_usr()
{
  return mem_ram_limit;
}


void mem_setup()
{
  memset(mem_protected_flags, 0, 32);
  mem_ram_limit = MEMSIZE-1;
  for(uint32_t p = MEMSIZE; p < 0x10000; p += 0x100 )
    mem_protect_flag_set(p, true);
  mem_protect_calc_limit();
}


// -------------------- ROM handling
// #if MAX_NUM_ROMS==0
bool mem_add_rom(uint16_t start, uint16_t length, const char *name, uint16_t flags, uint32_t config_file_offset) { return false; }
bool mem_remove_rom(byte i, bool clear) { return false; }
byte mem_get_num_roms(bool includeTemp) { return 0; }
void mem_set_rom_flags(byte i, uint16_t flags) {}
bool mem_get_rom_info(byte i, char *name, uint16_t *start, uint16_t *length, uint16_t *flags) { return false; }
uint16_t mem_get_rom_autostart_address() { return 0xFFFF; }
void mem_clear_roms() {}
void mem_reset_roms() {}

// #else
// Else nothing...
