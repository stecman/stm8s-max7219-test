#include "stm8s.h"

#include "ascii.h"
#include "generic/delay.h"

static uint8_t digits[6];

inline void spi_send_blocking(uint8_t data)
{
    // Load data into TX register
    SPI->DR = data;

    // Wait for TX buffer empty flag
    while (SPI_GetFlagStatus(SPI_FLAG_TXE) != SET);
}

static void max7219_cmd(uint8_t address, uint8_t data)
{
    // Chip select (active low)
    GPIOC->ODR &= ~(1<<4);

    spi_send_blocking(address);
    spi_send_blocking(data);

    // Wait for busy flag to clear
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

inline void configureTickTimer()
{
    // Number of ticks per second with 16Mhz/1024 (+/- calibration)
    const uint16_t target = 15625 + 10;

    // Prescale to fit 1 second inside a 16-bit counter at 16MHz
    TIM2->PSCR = TIM2_PRESCALER_1024;

    // Load refresh delay into timer
    TIM2->ARRH = (target >> 8);
    TIM2->ARRL = (target & 0xFF);

    // Enable timer
    TIM2->EGR |= TIM2_EVENTSOURCE_UPDATE;
    TIM2->CR1 |= TIM2_CR1_CEN;
}

void loadNumber(uint8_t input, uint8_t* output)
{
    output[0] = input / 10;
    output[1] = input % 10;
}

struct WallTime {
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
};

static struct WallTime currentTime = {COMPILE_HOUR, COMPILE_MINUTE, COMPILE_SECOND};

static const uint8_t brightnessTable[] = {
    0x05, // 12 am
    0x05, // 1am
    0x05, // 2am
    0x05, // 3am
    0x06, // 4am
    0x07, // 5am
    0x08, // 6am
    0x09, // 7am
    0x0B, // 8am
    0x0D, // 9am
    0x0E, // 10am
    0x0E, // 11am
    0x0F, // 12pm
    0x0F, // 1pm
    0x0F, // 2pm
    0x0E, // 3pm
    0x0E, // 4pm
    0x0E, // 5pm
    0x0D, // 6pm
    0x0C, // 7pm
    0x0B, // 8pm
    0x09, // 9pm
    0x08, // 10pm
    0x06, // 11pm
};

int main(void)
{
    // Configure the clock for maximum speed on the 16MHz HSI oscillator
    // At startup the clock output is divided by 8
    CLK->CKDIVR = 0x0;

    // Clock calibrated to within a few milliseconds of 1s with the configured timer
    CLK->HSITRIMR = 0b111;

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
    _delay_ms(1);

    GPIOC->ODR &= ~(1<<4);

    // Disable binary decode mode
    max7219_cmd(0x09, 0x00);

    // Set brightness
    max7219_cmd(0x0A, 0x0A);

    // Set scan mode to 6 digits
    max7219_cmd(0x0B, 0x06);

    // Enable display
    max7219_cmd(0x0C, 1);

    // Disable test mode
    max7219_cmd(0x0F, 0);

    // Enable binary decode mode
    max7219_cmd(0x09, 0xFF);

    configureTickTimer();


    // Tick outputon D5
    GPIOD->DDR |= (1<<5);
    GPIOD->CR1 |= (1<<5);
    GPIOD->CR2 |= (1<<5);
    GPIOD->ODR |= (1<<5);

    while (1) {

        // If tick timer has overflowed, increment time
        if (TIM2->SR1 & TIM2_SR1_UIF) {
            TIM2->SR1 &= ~TIM2_SR1_UIF;

            GPIOD->ODR ^= (1<<5);

            ++currentTime.second;

            if (currentTime.second == 60) {
                currentTime.second = 0;
                ++currentTime.minute;
            }

            if (currentTime.minute == 60) {
                currentTime.minute = 0;
                ++currentTime.hour;
            }

            // Roll over at 24 hours
            if (currentTime.hour == 24) {
                currentTime.hour = 0;
            }

            loadNumber(currentTime.hour, digits);
            loadNumber(currentTime.minute, digits + 2);
            loadNumber(currentTime.second, digits + 4);

            max7219_write_digits(digits, 6);

            // Update brightness
            max7219_cmd(0x0A, brightnessTable[currentTime.hour]);
        }
    }
}
