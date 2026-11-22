#include <cstdio>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "MAX30102.hpp"
#include "PicoI2C.hpp"
#include "hardware/gpio.h"
#include "hardware/sync.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define SDA_PIN 4
#define SCL_PIN 5

constexpr uint SENSOR_INT_PIN = 6;

TaskHandle_t sensor_task_handle = nullptr;
QueueHandle_t ppg_sample_queue = nullptr;

void sensor_gpio_isr(uint gpio, uint32_t events) {
    if (gpio == SENSOR_INT_PIN && (events & GPIO_IRQ_EDGE_FALL)) {

        BaseType_t higher_priority_task_woken = pdFALSE;

        if (sensor_task_handle != nullptr) {
            vTaskNotifyGiveFromISR(
                sensor_task_handle,
                &higher_priority_task_woken
            );

            portYIELD_FROM_ISR(higher_priority_task_woken);
        }
    }
}

extern "C" void vApplicationMallocFailedHook(void)
{
    while (true)
    {
        // Halt here if FreeRTOS runs out of heap memory.
    }
}

extern "C" void vApplicationStackOverflowHook(
    TaskHandle_t xTask,
    char* pcTaskName
)
{
    (void)xTask;
    (void)pcTaskName;

    taskDISABLE_INTERRUPTS();

    while (true)
    {
        // Halt here if a FreeRTOS task overflows its stack.
    }
}

void heartbeat_task(void* parameters)
{
    (void)parameters;

    while (true)
    {
        printf("FreeRTOS heartbeat\n");

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void ppg_processing_task(void* parameters) {
    (void)parameters;

    MAX30102::Sample sample;
    uint32_t processed_count = 0;

    while (true) {
        if (xQueueReceive(
                ppg_sample_queue,
                &sample,
                portMAX_DELAY) == pdPASS) {

            processed_count++;

            if (processed_count % 100 == 0) {
                printf("Processed PPG samples: %lu\n",
                    (unsigned long)processed_count);
            }
        }
    }
}

void sensor_acquisition_task(void* parameters)
{
    MAX30102* sensor = static_cast<MAX30102*>(parameters);

    bool more_samples_pending = false;

    while (true) {
        if (!more_samples_pending && gpio_get(SENSOR_INT_PIN) != 0) {
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        }

        if (!sensor->clear_interrupt_status()) {
            printf("Interrupt status read error\n");
            continue;
        }
        
        constexpr int MAX_SAMPLES_PER_PASS = 16;

        int samples_processed = 0;

        while (samples_processed < MAX_SAMPLES_PER_PASS) {
            int remaining = sensor->available();

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

            if (!sensor->read_sample(sample)) {
                printf("Sample read error\n");
                more_samples_pending = false;
                break;
            }

            if (xQueueSend(ppg_sample_queue, &sample, 0) != pdPASS) {
                static uint32_t queue_overflow_count = 0;
                queue_overflow_count++;

                if (queue_overflow_count % 100 == 0) {
                    printf("PPG queue overflow count: %lu\n",
                        (unsigned long)queue_overflow_count);
                }
            }

            static uint32_t sample_count = 0;

            sample_count++;

            if (sample_count % 100 == 0) {
            printf("Red: %lu, IR: %lu\n",
                    (unsigned long)sample.red,
                    (unsigned long)sample.ir);
            }

            samples_processed++;
        }

        // Check whether unread samples remain after this pass.
        if (samples_processed == MAX_SAMPLES_PER_PASS) {
            int remaining = sensor->available();

            if (remaining >= 0) {
                more_samples_pending = (remaining > 0);
            } else {
                printf("FIFO occupancy read error\n");
                more_samples_pending = false;
            }
        }
    }
}

int main() {
    stdio_init_all();
    sleep_ms(2000);
    BaseType_t result = xTaskCreate(
    heartbeat_task,
    "Heartbeat",
    512,
    nullptr,
    1,
    nullptr
    );

    if (result != pdPASS) {
        printf("Failed to create heartbeat task\n");
        while (true) {
            tight_loop_contents();
        }
    }

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

    irq_set_priority(IO_IRQ_BANK0, 0xA0);

    ppg_sample_queue = xQueueCreate(
        32,
        sizeof(MAX30102::Sample)
    );

    if (ppg_sample_queue == nullptr) {
        printf("Failed to create PPG sample queue\n");
        while (true) {
            tight_loop_contents();
        }
    }

    BaseType_t sensor_task_result = xTaskCreate(
        sensor_acquisition_task,
        "PPG_Acquisition",
        1024,
        &sensor,
        2,
        &sensor_task_handle
    );

    if (sensor_task_result != pdPASS) {
        printf("Failed to create sensor acquisition task\n");

        while (true) {
            tight_loop_contents();
        }
    }

    BaseType_t processing_task_result = xTaskCreate(
        ppg_processing_task,
        "PPG_Processing",
        512,
        nullptr,
        1,
        nullptr
    );

    if (processing_task_result != pdPASS) {
        printf("Failed to create PPG processing task\n");

        while (true) {
            tight_loop_contents();
        }
    }

    gpio_set_irq_enabled_with_callback(
        SENSOR_INT_PIN,
        GPIO_IRQ_EDGE_FALL,
        true,
        &sensor_gpio_isr
    );

    printf("Starting FreeRTOS scheduler...\n");

    vTaskStartScheduler();

    // Execution should never reach this point.
    printf("ERROR: FreeRTOS scheduler returned\n");

    while (true) {
        tight_loop_contents();
    }
}