#ifndef RING_BUFFER_H    // Include guard: checks if RING_BUFFER_H is not defined
#define RING_BUFFER_H    // Defines RING_BUFFER_H

#include <stdint.h>
#include <stddef.h>
#include <stdatomic.h>

#define BUFFER_SIZE 16
#define HIGH_WATER_MARK 12
#define CLEAR_WATER_MARK 8

_Static_assert(
    BUFFER_SIZE > 0 &&
    (BUFFER_SIZE & (BUFFER_SIZE - 1)) == 0,
    "BUFFER_SIZE must be a positive power of two"
);

typedef struct {
    uint16_t data[BUFFER_SIZE];
    atomic_size_t head;
    atomic_size_t tail;
} RingBuffer;


void ring_buffer_init(RingBuffer *rb);

int ring_buffer_is_empty(const RingBuffer *rb);

int ring_buffer_is_full(const RingBuffer *rb);

int ring_buffer_push(RingBuffer *rb, uint16_t value);

int ring_buffer_pop(RingBuffer *rb, uint16_t *value);

size_t ring_buffer_count(const RingBuffer *rb);

#endif // RING_BUFFER_H
