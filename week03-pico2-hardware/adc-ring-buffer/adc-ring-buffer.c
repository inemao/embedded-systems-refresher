#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "ring_buffer.h"
#include <stdatomic.h>

static atomic_uint overflow_count;

static RingBuffer sample_buffer;

bool sample_timer_callback(struct repeating_timer *t)
{
    uint16_t sample = adc_read();

    if (!ring_buffer_push(&sample_buffer, sample)) {
        atomic_fetch_add_explicit(
            &overflow_count,
            1,
            memory_order_relaxed
        );
    }

    return true;
}

int main(void)
{
    stdio_init_all();

    ring_buffer_init(&sample_buffer);

    atomic_init(&overflow_count, 0);

    adc_init();
    adc_gpio_init(26);
    adc_select_input(0);

    struct repeating_timer timer;

    add_repeating_timer_ms(
        -1,
        sample_timer_callback,
        NULL,
        &timer
    );

    absolute_time_t last_report = get_absolute_time();

    bool high_water_active = false;
    unsigned high_water_events = 0;

    bool pause_done = false;
    size_t max_count = 0;
    size_t count_after_pause = 0;

    unsigned samples_processed = 0;

    while (true) {

    if (!pause_done &&
    absolute_time_diff_us(last_report, get_absolute_time()) >= 500000) {

    sleep_ms(14);

    count_after_pause = ring_buffer_count(&sample_buffer);

    pause_done = true;
    }

    size_t count = ring_buffer_count(&sample_buffer);

    if (count > max_count) {
    max_count = count;
    }

    if (count >= HIGH_WATER_MARK && !high_water_active) {
    high_water_active = true;
    high_water_events++;
    }

    if (count <= CLEAR_WATER_MARK && high_water_active) {
    high_water_active = false;
    }

    uint16_t sample;

    if (ring_buffer_pop(&sample_buffer, &sample)) {
    samples_processed++;
    }

    absolute_time_t now = get_absolute_time();

    if (absolute_time_diff_us(last_report, now) >= 1000000) {
        unsigned overflows = atomic_exchange_explicit(
            &overflow_count,
            0,
            memory_order_relaxed
        );

        printf(
        "Processed: %u, Overflows: %u, High-water events: %u, "
        "Max count: %u, Count after pause: %u\n",
        samples_processed,
        overflows,
        high_water_events,
        (unsigned)max_count,
        (unsigned)count_after_pause
        );

        samples_processed = 0;
        last_report = now;
    }

    tight_loop_contents();
    }
}
