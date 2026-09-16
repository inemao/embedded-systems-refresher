#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "ring_buffer.h"
#include <stdatomic.h>
#include <inttypes.h>

static RingBuffer sample_buffer;
static atomic_uint overflow_count;

bool sample_timer_callback(struct repeating_timer *t)
{
    static uint32_t sequence = 0;

    Sample sample;

    sample.sequence = sequence;
    sample.value = adc_read();
    sequence += 1;

    if(!ring_buffer_push(&sample_buffer, sample)){
        atomic_fetch_add_explicit(
            &overflow_count,
            1,
            memory_order_relaxed
        );
    }

    return true;
}

int main()
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
    
    absolute_time_t last_report_time = get_absolute_time();

    while (true) {
        Sample sample;
        int64_t elapsed_us =
            absolute_time_diff_us(
            last_report_time,
            get_absolute_time()
            );
        if (elapsed_us >= 1000000){
            unsigned int overflows = atomic_exchange_explicit(
                &overflow_count,
                0,
                memory_order_relaxed
                );
            printf("O,%u\n",overflows);
            last_report_time = get_absolute_time();
        }
        if (ring_buffer_pop(&sample_buffer, &sample)) {
            printf("S,%" PRIu32 ",%u\n",
                sample.sequence,
                sample.value);

        }
    }
}
