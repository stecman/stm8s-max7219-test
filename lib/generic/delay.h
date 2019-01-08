#pragma once

// This includes the typedefs normally found in stdint.h
#include <stm8s.h>

/**
 * Create a delay in microseconds using 8-bit timer 4 (TIM4)
 */
void _delay_us(uint16_t microseconds);

void _delay_ms(uint16_t milliseconds);