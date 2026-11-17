#include <cstdio>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "MAX30102.hpp"
#include "PicoI2C.hpp"
#include "hardware/gpio.h"
#include "hardware/sync.h"

#define SDA_PIN 4
#define SCL_PIN 5

constexpr uint SENSOR_INT_PIN = 6;

volatile bool ppg_interrupt_pending = false;

void sensor_gpio_isr(uint gpio, uint32_t events) {
    if (gpio == SENSOR_INT_PIN &&
        (events & GPIO_IRQ_EDGE_FALL)) {
        ppg_interrupt_pending = true;
    }
}

int main() {
    stdio_init_all();
    sleep_ms(2000);

    i2c_init(i2c0, 100000);

    gpio_init(SENSOR_INT_PIN);
    gpio_set_dir(SENSOR_INT_PIN, GPIO_IN);
    gpio_pull_up(SENSOR_INT_PIN);

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

   if (!sensor.initialize(config)) {
        printf("MAX30102 initialization failed\n");

        while (true) {
            sleep_ms(1000);
        }
    }

    printf("MAX30102 initialized successfully\n");

    if (!sensor.clear_interrupt_status()) {
        printf("Failed to clear interrupt status\n");
        return 1;
    }

    if (!sensor.enable_ppg_ready_interrupt()) {
        printf("Failed to enable PPG-ready interrupt\n");
        return 1;
    }

    gpio_set_irq_enabled_with_callback(
        SENSOR_INT_PIN,
        GPIO_IRQ_EDGE_FALL,
        true,
        &sensor_gpio_isr
    );

    if (gpio_get(SENSOR_INT_PIN) == 0) {
        ppg_interrupt_pending = true;
    }

    bool more_samples_pending = false;

    while (true) {
        uint32_t saved = save_and_disable_interrupts();

        bool pending = ppg_interrupt_pending;
        ppg_interrupt_pending = false;

        restore_interrupts(saved);

        if (!pending &&
            !more_samples_pending &&
            gpio_get(SENSOR_INT_PIN) != 0) {
                tight_loop_contents();
                continue;
        }

        if (!sensor.clear_interrupt_status()) {
            printf("Interrupt status read error\n");
            continue;
        }
        
        constexpr int MAX_SAMPLES_PER_PASS = 16;

        int samples_processed = 0;

        while (samples_processed < MAX_SAMPLES_PER_PASS) {
            int remaining = sensor.available();

            if (remaining < 0) {
                printf("FIFO occupancy read error\n");
                more_samples_pending = false;
                break;
            }

            if (remaining == 0) {
                more_samples_pending = false;
                break;
            }

            MAX30102::Sample sample;

            if (!sensor.read_sample(sample)) {
                printf("Sample read error\n");
                more_samples_pending = false;
                break;
            }

            printf("Red: %lu, IR: %lu\n",
                    (unsigned long)sample.red,
                    (unsigned long)sample.ir);

            samples_processed++;
        }

        // Check whether unread samples remain after this pass.
        if (samples_processed == MAX_SAMPLES_PER_PASS) {
            int remaining = sensor.available();

            if (remaining >= 0) {
                more_samples_pending = (remaining > 0);
            } else {
                printf("FIFO occupancy read error\n");
                more_samples_pending = false;
            }
        }
    }
}