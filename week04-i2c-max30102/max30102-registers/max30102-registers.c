#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define SDA_PIN 4
#define SCL_PIN 5

#define MAX30102_ADDR 0x57
#define PART_ID_REG   0xFF

#define REG_SPO2_CONFIG 0x0A
#define REG_MODE_CONFIG 0x09

#define REG_LED1_PA 0x0C
#define REG_LED2_PA 0x0D

int max30102_write_register(uint8_t reg, uint8_t value)
{
    uint8_t data[2] = {reg, value};

    int results = i2c_write_timeout_us(
        i2c0,
        MAX30102_ADDR,
        data,
        2,
        false,
        10000
    );

    return results;
}

int max30102_read_register(uint8_t reg, uint8_t *value)
{
    int written = i2c_write_timeout_us(
        i2c0,
        MAX30102_ADDR,
        &reg,
        1,
        true,
        10000
    );

    if (written != 1) {
        return -1;
    }

    int read = i2c_read_timeout_us(
        i2c0,
        MAX30102_ADDR,
        value,
        1,
        false,
        10000
    );

    if (read != 1) {
        return -2;
    }

    return 1;
}

int main()
{
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

    int write_result = max30102_write_register(
    REG_SPO2_CONFIG,
    0x27
    );

    uint8_t value = 0;

    int read_result = max30102_read_register(
        REG_SPO2_CONFIG,
        &value
    );

    printf("Write result = %d\n", write_result);
    printf("Read result = %d\n", read_result);
    printf("SPO2_CONFIG = 0x%02X\n", value);

    int mode_write_result = max30102_write_register(
    REG_MODE_CONFIG,
    0x03
    );

    value = 0;

    int mode_read_result = max30102_read_register(
        REG_MODE_CONFIG,
        &value
    );

    printf("Write result = %d\n", mode_write_result);
    printf("Read result = %d\n", mode_read_result);
    printf("MODE_CONFIG = 0x%02X\n", value);

    int led_red_write_result = max30102_write_register(
    REG_LED1_PA,
    0x32
    );

    value = 0;

    int led_red_read_result = max30102_read_register(
        REG_LED1_PA,
        &value
    );

    printf("Write result = %d\n", led_red_write_result);
    printf("Read result = %d\n", led_red_read_result);
    printf("LED_Red_CONFIG = 0x%02X\n", value);

    int led_IR_write_result = max30102_write_register(
    REG_LED2_PA,
    0x32
    );

    value = 0;

    int led_IR_read_result = max30102_read_register(
        REG_LED2_PA,
        &value
    );

    printf("Write result = %d\n", led_IR_write_result);
    printf("Read result = %d\n", led_IR_read_result);
    printf("LED_IR_CONFIG = 0x%02X\n", value);

    while (true) {
        sleep_ms(1000);
    }
}
