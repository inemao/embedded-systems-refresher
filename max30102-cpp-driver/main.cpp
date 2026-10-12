#include <cstdio>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "MAX30102.hpp"
#include "PicoI2C.hpp"

#define SDA_PIN 4
#define SCL_PIN 5

int main() {
    stdio_init_all();
    sleep_ms(2000);

    i2c_init(i2c0, 100000);

    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);

    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    PicoI2C pico_i2c(i2c0);
    MAX30102 sensor(pico_i2c, 0x57);

    MAX30102::Config config;

    config.adc_range = MAX30102::AdcRange::RANGE_4096;
    config.sample_rate = MAX30102::SampleRate::SPS_100;
    config.pulse_width = MAX30102::PulseWidth::US_411;
    config.red_led_current = 0x32;
    config.ir_led_current = 0x32;

   if (sensor.initialize(config)) {
        printf("MAX30102 initialized successfully\n");
    } else {
        printf("MAX30102 initialization failed\n");
    }


    while (true) {
        int available = sensor.available();

        if (available < 0) {
            printf("FIFO read error\n");
        } else if (available == 0) {
        } else {
            while (available > 0) {
                MAX30102::Sample sample;

                if (!sensor.read_sample(sample)) {
                    printf("Sample read error\n");
                    break;
                }

                printf("Red: %lu, IR: %lu\n",
                    (unsigned long)sample.red,
                    (unsigned long)sample.ir);

                available--;
            }
        }
    }
}