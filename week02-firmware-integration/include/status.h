#ifndef STATUS_H
#define STATUS_H

#include <stdint.h>
#include <stdatomic.h>

#define BIT(n)                  (1U << (n))
#define STATUS_BUFFER_OVERFLOW  BIT(2)

void status_init(void);
void status_set(uint8_t mask);
int status_is_set(uint8_t mask);
void status_clear(uint8_t mask);
int status_test_and_clear(uint8_t mask);
void overflow_count_init(void);
void overflow_count_increment(void);
size_t overflow_count_take(void);

#endif
