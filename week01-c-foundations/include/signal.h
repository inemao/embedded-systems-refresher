#ifndef SIGNAL_H    // Include guard: checks if SIGNAL_H is not defined
#define SIGNAL_H    // Defines SIGNAL_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint32_t timestamp_ms;
    uint16_t adc_value;
} Sample;


uint16_t find_min(
    const Sample *samples,
    size_t length
);

uint16_t find_max(
    const Sample *samples,
    size_t length
);

double calculate_mean(
    const Sample *samples,
    size_t length
);

size_t count_above_threshold(
    const Sample *samples,
    size_t length,
    uint16_t threshold
);

void normalize(
    const Sample *input,
    float *output,
    size_t length
);

#endif // SIGNAL_H
