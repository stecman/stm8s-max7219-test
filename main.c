#include "stm8s.h"

#include "ascii.h"
#include "generic/delay.h"

static uint8_t digits[6];

inline void spi_send_blocking(uint8_t data)
{
    // Load data into TX register
    SPI->DR = data;

    // Wait for TX buffer empty flag
    // while ((SPI->SR & SPI_SR_TXE) == 0);
    while (SPI_GetFlagStatus(SPI_FLAG_TXE) != SET);
}

static void max7219_cmd(uint8_t address, uint8_t data)
{
    // Chip select (active low)
    GPIOC->ODR &= ~(1<<4);

    spi_send_blocking(address);
    spi_send_blocking(data);

    // Wait for busy flag to clear
    // while ((SPI->SR & SPI_SR_BSY) != 0);
    while (SPI_GetFlagStatus(SPI_FLAG_BSY) != RESET);

    // Disable chip select
    GPIOC->ODR |= (1<<4);
    _delay_us(10);
}

static void max7219_write_digits(uint8_t* data, uint8_t digits)
{
    // Write value for all digits
    for (uint8_t i = 0; i < digits; ++i) {
        max7219_cmd(i + 1, data[i]);
    }
}

int main(void)
{
    // Configure the clock for maximum speed on the 16MHz HSI oscillator
    // At startup the clock output is divided by 8
    CLK->CKDIVR = 0x0;

    SPI_Init( SPI_FIRSTBIT_MSB,
              SPI_BAUDRATEPRESCALER_32,
              SPI_MODE_MASTER,
              SPI_CLOCKPOLARITY_LOW,
              SPI_CLOCKPHASE_1EDGE,
              SPI_DATADIRECTION_1LINE_TX,
              SPI_NSS_SOFT, 0 );

    SPI_Cmd(ENABLE);

    // Chip select on C4
    GPIOC->DDR |= (1<<4);
    GPIOC->CR1 |= (1<<4);
    GPIOC->CR2 |= (1<<4);
    GPIOC->ODR |= (1<<4);

    // Wait for power to stablise
    _delay_ms(10);

    GPIOC->ODR &= ~(1<<4);

    // Disable binary decode mode
    max7219_cmd(0x09, 0x00);

    // Set brightness
    max7219_cmd(0x0A, 0x0F);

    // Set scan mode to 6 digits
    max7219_cmd(0x0B, 0x06);

    // Enable display
    max7219_cmd(0x0C, 1);

    // Disable test mode
    max7219_cmd(0x0F, 0);

    while (1) {
        uint32_t ticks = 0;

        // Enable binary decode mode
        max7219_cmd(0x09, 0xFF);

        // max7219_write_digits(digits, 6);

        // _delay_ms(1000);


        while (1) {
            uint32_t ticksCopy = ticks;

            digits[0] = 0;
            digits[1] = 0;
            digits[2] = 0;
            digits[3] = 0;
            digits[4] = 0;
            digits[5] = 0;

            while (ticksCopy >= 100000) {
                digits[0]++;
                ticksCopy -= 100000;
            }

            while (ticksCopy >= 10000) {
                digits[1]++;
                ticksCopy -= 10000;
            }

            while (ticksCopy >= 1000) {
                digits[2]++;
                ticksCopy -= 1000;
            }

            while (ticksCopy >= 100) {
                digits[3]++;
                ticksCopy -= 100;
            }

            while (ticksCopy >= 10) {
                digits[4]++;
                ticksCopy -= 10;
            }

            while (ticksCopy >= 1) {
                digits[5]++;
                ticksCopy -= 1;
            }

            _delay_ms(100);
            max7219_write_digits(digits, 6);

            ticks++;
        }

        // // Disable binary decode mode
        // max7219_cmd(0x09, 0x00);

        // digits[0] = 0x0;
        // digits[1] = 0b00111101;
        // digits[2] = 0b00011101;
        // digits[3] = 0b00011101;
        // digits[4] = 0b00010101;
        // digits[5] = 0b00010001;

        // // Write value for all digits
        // max7219_write_digits(digits, 6);

        // for (uint8_t i = 0, on = 0; i < 7; ++i) {
        //     on = !on;
        //     max7219_cmd(0x0C, on);
        //     _delay_ms(200);
        // }

        // _delay_ms(500);

        // // Toggle test mode
        // for (uint8_t u = 0; u < 2; ++u) {
        //     uint8_t byte = 0xC0;

        //     while ( byte > 1) {

        //         for (uint8_t i = 0; i < 6; ++i) {
        //             max7219_cmd(i + 1, byte & ~0x1);
        //         }

        //         byte >>= 1;
        //         _delay_ms(80);
        //     }
        // }
    }
}
