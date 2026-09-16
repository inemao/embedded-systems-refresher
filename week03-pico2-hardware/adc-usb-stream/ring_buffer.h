#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
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
    uint32_t sequence;
    uint16_t value;
} Sample;

typedef struct {
    Sample data[BUFFER_SIZE];
    atomic_size_t head;
    atomic_size_t tail;
} RingBuffer;

void ring_buffer_init(RingBuffer *rb);
bool ring_buffer_push(RingBuffer *rb, Sample value);
bool ring_buffer_pop(RingBuffer *rb, Sample *value);
size_t ring_buffer_count(const RingBuffer *rb);

#endif