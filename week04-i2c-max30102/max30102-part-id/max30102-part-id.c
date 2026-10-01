#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define SDA_PIN 4
#define SCL_PIN 5

#define MAX30102_ADDR 0x57
#define PART_ID_REG   0xFF

int main(void)
{
    // Initialize USB serial
    stdio_init_all();
    sleep_ms(2000);

    // Initialize I2C0 at 100 kHz
    i2c_init(i2c0, 100000);

    // Configure GP4 as SDA and GP5 as SCL
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);

    // Enable internal pull-ups
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    // Register we want to read
    uint8_t reg = PART_ID_REG;
    uint8_t value = 0;

    // Write the register address.
    // nostop = true so the next read generates a repeated START.
    int written = i2c_write_timeout_us(
        i2c0,
        MAX30102_ADDR,
        &reg,
        1,
        true,
        10000
    );

    printf("Written = %d\n", written);

    // Read one byte from the selected register.
    // nostop = false because we're finished after this read.
    int read = i2c_read_timeout_us(
        i2c0,
        MAX30102_ADDR,
        &value,
        1,
        false,
        10000
    );

    printf("Read = %d\n", read);
    printf("PART_ID = 0x%02X\n", value);

    while (true) {
        sleep_ms(1000);
    }
}