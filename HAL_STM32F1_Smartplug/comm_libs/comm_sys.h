#pragma once

#include "comm_def.h"

void delay_1ms();

void delay_ms(uint32_t millis);

void sys_tick_set(uint32_t tick_value);

void sys_tick_inc();

// millisecond tick
uint32_t sys_tick_count();

uint32_t sys_tick_diif(uint32_t test_tick, uint32_t prev_tick);

uint32_t sys_uptime();
