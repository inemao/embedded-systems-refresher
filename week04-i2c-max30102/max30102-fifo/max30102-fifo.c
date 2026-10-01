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

    int fifo_wr_ptr_write_result = max30102_write_register(REG_FIFO_WR_PTR, 0);

    value = 0;

     int fifo_wr_ptr_read_result = max30102_read_register(
        REG_FIFO_WR_PTR,
        &value
    );

    printf("Write result = %d\n", fifo_wr_ptr_write_result);
    printf("Read result = %d\n", fifo_wr_ptr_read_result);
    printf("FIFO_WR_PTR = 0x%02X\n", value);

    
    int ovf_counter_write_result = max30102_write_register(REG_OVF_COUNTER, 0);

    value = 0;

    int ovf_counter_read_result = max30102_read_register(
        REG_OVF_COUNTER,
        &value
    );

    printf("Write result = %d\n", ovf_counter_write_result);
    printf("Read result = %d\n", ovf_counter_read_result);
    printf("OVF_COUNTER = 0x%02X\n", value);
    
    int fifo_rd_ptr_write_result = max30102_write_register(REG_FIFO_RD_PTR, 0);

    value = 0;

    int fifo_rd_ptr_read_result = max30102_read_register(
        REG_FIFO_RD_PTR,
        &value
    );

    printf("Write result = %d\n", fifo_rd_ptr_write_result);
    printf("Read result = %d\n", fifo_rd_ptr_read_result);
    printf("FIFO_RD_PTR = 0x%02X\n", value);


    int fifo_config_write_result = max30102_write_register(REG_FIFO_CONFIG, 0x00);

    value = 0;

    int fifo_config_read_result = max30102_read_register(
        REG_FIFO_CONFIG,
        &value
    );

    printf("Write result = %d\n", fifo_config_write_result);
    printf("Read result = %d\n", fifo_config_read_result);
    printf("FIFO_CONFIG = 0x%02X\n", value);
    
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

    sleep_ms(100);

    uint8_t write_ptr = 0;
    uint8_t read_ptr = 0;
    uint8_t overflow = 0;

    fifo_wr_ptr_read_result = max30102_read_register(
        REG_FIFO_WR_PTR,
        &write_ptr
    );

    printf("FIFO_WR_PTR = 0x%02X\n", write_ptr);

    fifo_rd_ptr_read_result = max30102_read_register(
        REG_FIFO_RD_PTR,
        &read_ptr
    );

    printf("FIFO_RD_PTR = 0x%02X\n", read_ptr);

    ovf_counter_read_result = max30102_read_register(
        REG_OVF_COUNTER,
        &overflow
    );

    printf("OVF_COUNTER = 0x%02X\n", overflow);
   
   /*uint32_t red = 0;
    uint32_t ir = 0;

    int sample_result = max30102_read_fifo_sample(&red, &ir);

    printf("FIFO read result = %d\n", sample_result);
    printf("Red sample = %lu\n", (unsigned long)red);
    printf("IR sample = %lu\n", (unsigned long)ir);

    uint8_t read_ptr_after = 0;

    max30102_read_register(
        REG_FIFO_RD_PTR,
        &read_ptr_after
    );

    printf("FIFO_RD_PTR after sample = 0x%02X\n", read_ptr_after);*/


    uint8_t unread = (write_ptr - read_ptr) & 0x1F;

    printf("Unread samples = %u\n", unread);

    while (unread > 0) {
        uint32_t red = 0;
        uint32_t ir = 0;

        int sample_result = max30102_read_fifo_sample(&red, &ir);

        if (sample_result != 6) {
            printf("FIFO read failed: %d\n", sample_result);
            break;
        }

        printf("Red sample = %lu, IR sample = %lu\n",
           (unsigned long)red,
           (unsigned long)ir);

        unread--;
    }

    while (true) {
        sleep_ms(1000);
    }
}
