#include <stdio.h>
#include <stdatomic.h>
#include "pico/stdlib.h"

static atomic_uint timer_events;

bool timer_callback(struct repeating_timer *t)
{
    atomic_fetch_add_explicit(
        &timer_events,
        1,
        memory_order_relaxed
    );

    return true;
}

int main(void)
{
    stdio_init_all();

    atomic_init(&timer_events, 0);

    struct repeating_timer timer;

    add_repeating_timer_ms(
        -1,
        timer_callback,
        NULL,
        &timer
    );

    while (true) {
        unsigned events = atomic_exchange_explicit(
            &timer_events,
            0,
            memory_order_relaxed
        );

        if (events > 0) {
        printf("Events: %u\n", events);
        }

        sleep_ms(100);
    }
}
