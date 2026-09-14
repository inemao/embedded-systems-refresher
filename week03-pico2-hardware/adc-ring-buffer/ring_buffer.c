#include "ring_buffer.h"

void ring_buffer_init(RingBuffer *rb)
{
    atomic_init(&rb->head, 0);
    atomic_init(&rb->tail, 0);
}

bool ring_buffer_push(RingBuffer *rb, uint16_t value)
{
    size_t head = atomic_load_explicit(
        &rb->head,
        memory_order_relaxed
    );

    size_t next_head =
        (head + 1) & (BUFFER_SIZE - 1);

    size_t tail = atomic_load_explicit(
        &rb->tail,
        memory_order_acquire
    );

    if (next_head == tail) {
        return false;
    }

    rb->data[head] = value;

    atomic_store_explicit(
        &rb->head,
        next_head,
        memory_order_release
    );

    return true;
}

bool ring_buffer_pop(RingBuffer *rb, uint16_t *value)
{
    size_t tail = atomic_load_explicit(
        &rb->tail,
        memory_order_relaxed
    );

    size_t head = atomic_load_explicit(
        &rb->head,
        memory_order_acquire
    );

    if (tail == head) {
        return false;
    }

    *value = rb->data[tail];

    size_t next_tail =
        (tail + 1) & (BUFFER_SIZE - 1);

    atomic_store_explicit(
        &rb->tail,
        next_tail,
        memory_order_release
    );

    return true;
}

size_t ring_buffer_count(const RingBuffer *rb)
{
    size_t head = atomic_load_explicit(
        &rb->head,
        memory_order_acquire
    );

    size_t tail = atomic_load_explicit(
        &rb->tail,
        memory_order_acquire
    );

    return (head - tail) & (BUFFER_SIZE - 1);
}