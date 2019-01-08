#include "delay.h"

void _delay_us(uint16_t microseconds) {
    TIM4->PSCR = TIM4_PRESCALER_1; // Set prescaler

    // Set count to approximately 1uS (clock/microseconds in 1 second)
    // The -1 adjusts for other runtime costs of this function
    TIM4->ARR = ((16000000L)/1000000) - 1;


    TIM4->CR1 = TIM4_CR1_CEN; // Enable counter

    for (; microseconds > 1; --microseconds) {
        while ((TIM4->SR1 & TIM4_SR1_UIF) == 0);

        // Clear overflow flag
        TIM4->SR1 &= ~TIM4_SR1_UIF;
    }
}

void _delay_ms(uint16_t milliseconds)
{
    while (milliseconds)
    {
        _delay_us(1000);
        milliseconds--;
    }
}
