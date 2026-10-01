#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define SDA_PIN 4
#define SCL_PIN 5

#define MAX30102_ADDR 0x57

#define REG_SPO2_CONFIG 0x0A
#define REG_MODE_CONFIG 0x09

#define REG_LED1_PA 0x0C
#define REG_LED2_PA 0x0D

#define REG_FIFO_WR_PTR   0x04
#define REG_OVF_COUNTER   0x05
#define REG_FIFO_RD_PTR   0x06
#define REG_FIFO_DATA     0x07
#define REG_FIFO_CONFIG   0x08


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

int max30102_read_fifo_sample(uint32_t *red, uint32_t *ir)
{
    uint8_t reg = REG_FIFO_DATA;
    uint8_t data[6];

    // select FIFO_DATA register

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
        data,
        6,
        false,
        10000
    );

    if (read != 6) {
        return -2;
    }
    
    // reconstruct red

    *red =
    ((uint32_t)data[0] << 16) |
    ((uint32_t)data[1] << 8) |
    (uint32_t)data[2];

    *red &= 0x3FFFF;

    // reconstruct IR

    *ir =
    ((uint32_t)data[3] << 16) |
    ((uint32_t)data[4] << 8) |
    (uint32_t)data[5];

    *ir &= 0x3FFFF;

    return 6;
    
}


int main()
{
    stdio_init_all();
    sleep_ms(2000);

    i2c_init(i2c0, 100000);

    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);

    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    // Configure optical acquisition
    max30102_write_register(REG_SPO2_CONFIG, 0x27);
    max30102_write_register(REG_LED1_PA, 0x32);
    max30102_write_register(REG_LED2_PA, 0x32);

    // Reset FIFO
    max30102_write_register(REG_FIFO_WR_PTR, 0);
    max30102_write_register(REG_OVF_COUNTER, 0);
    max30102_write_register(REG_FIFO_RD_PTR, 0);
    max30102_write_register(REG_FIFO_CONFIG, 0x00);

    // Enable SpO2 acquisition LAST
    max30102_write_register(REG_MODE_CONFIG, 0x03);

    uint32_t sequence = 0;

    while (true) {
        uint8_t overflow = 0;

        uint8_t write_ptr = 0;
        uint8_t read_ptr = 0;

        int overflow_result = max30102_read_register(
            REG_OVF_COUNTER,
            &overflow
        );


        // Read REG_FIFO_WR_PTR into write_ptr

        int write_result = max30102_read_register(
            REG_FIFO_WR_PTR,
            &write_ptr
        );

        // Read REG_FIFO_RD_PTR into read_ptr

        int read_result = max30102_read_register(
            REG_FIFO_RD_PTR,
            &read_ptr
        );

        if (overflow_result != 1 || write_result != 1 || read_result != 1) {
            continue;
        }

        if(overflow != 0){
            printf("O,%u\n",overflow);
            max30102_write_register(REG_OVF_COUNTER, 0);
        }

        // Calculate and empty the unread amount
        uint8_t unread = (write_ptr - read_ptr) & 0x1F;

        while (unread > 0) {
            uint32_t red = 0;
            uint32_t ir = 0;

            int result = max30102_read_fifo_sample(&red, &ir);

            if (result != 6) {
                break;
            }

            printf("S,%lu,%lu,%lu\n",
                (unsigned long) sequence,
                (unsigned long)red,
                (unsigned long)ir);

            sequence++;

            unread--;
        }       
    }
}

