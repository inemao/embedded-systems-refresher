#include "status.h"

static atomic_uchar status_register;
static atomic_size_t overflow_count;

void overflow_count_init(void)
{
    atomic_init(&overflow_count, 0);
}

void overflow_count_increment(void)
{
    atomic_fetch_add_explicit(
        &overflow_count,
        1,
        memory_order_relaxed
    );
}

size_t overflow_count_take(void)
{
    return atomic_exchange_explicit(
        &overflow_count,
        0,
        memory_order_relaxed
    );
}

void status_init(void)
{
    atomic_init(&status_register, 0);
    overflow_count_init();
}

void status_set(uint8_t mask)
{
    atomic_fetch_or_explicit(
        &status_register,
        mask,
        memory_order_relaxed
    );
}

int status_is_set(uint8_t mask)
{
    uint8_t value = atomic_load_explicit(
        &status_register,
        memory_order_relaxed
    );

    return (value & mask) != 0;
}

void status_clear(uint8_t mask)
{
    atomic_fetch_and_explicit(
        &status_register,
        (uint8_t)~mask,
        memory_order_relaxed
    );
}

int status_test_and_clear(uint8_t mask)
{
    uint8_t old_value = atomic_fetch_and_explicit(
        &status_register,
        (uint8_t)~mask,
        memory_order_relaxed
    );

    return (old_value & mask) != 0;
}



