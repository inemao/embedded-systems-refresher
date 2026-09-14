#include <stdlib.h> // Required for rand() and srand()
#include <time.h> 
#include "adc.h"

void adc_init(void){
    srand((unsigned int)time(NULL));
}

uint16_t adc_read(void)
{
    /* generate simulated 12-bit ADC value */
    uint16_t adc_value = (uint16_t)(rand() % 4096);
    return adc_value;
}
