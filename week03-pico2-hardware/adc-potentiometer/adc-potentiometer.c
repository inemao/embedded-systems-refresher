#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

int main(void)
{
    stdio_init_all();

    adc_init();

    // GPIO26 is ADC0
    adc_gpio_init(26);
    adc_select_input(0);

    while (true) {
        uint16_t raw = adc_read();

        printf("ADC raw: %u\n", raw);

        sleep_ms(250);
    }
}