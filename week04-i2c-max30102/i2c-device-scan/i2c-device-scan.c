#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define SDA_PIN 4
#define SCL_PIN 5

#define MAX30102_ADDR 0x57
#define PART_ID_REG   0xFF


int main(){
    
    
    stdio_init_all();
    sleep_ms(2000);

    i2c_init(i2c0, 100000);

    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);

    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    printf("Scanning I2C bus...\n");
    for (int address = 0x08; address <= 0x77; address++) {
   
        uint8_t data;

        int result = i2c_read_timeout_us(
            i2c0,
            address,
            &data,
            1,
            false,
            1000
        );

        if (result == 1) {
            printf("Found device at 0x%02X\n", address);
        }
    }
    printf("Scan complete.\n");
    return 0; 
}