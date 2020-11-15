// -----------------------------------------------------------------------------
// Altair 8800 Simulator
// Copyright (C) 2017 David Hansel
// -----------------------------------------------------------------------------

#ifndef HOST_H
#define HOST_H

#include <Arduino.h>

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
