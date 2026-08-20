#include <stdio.h>
#include <stdint.h>
#include <stdlib.h> // Required for rand() and srand()
#include <time.h> 

#include "signal.h"


int main(void)
{
    srand((unsigned int)time(NULL));
    const size_t length = 20;
    const int sampling_frequency = 100; // unit Hz 
    Sample data[length];
    float normalized_data[length];
    const uint16_t threshold = 2000;
    //initialize sample array
    for(size_t i = 0; i < length; i++) {
        data[i].adc_value = (uint16_t)(rand() % 4096);
        data[i].timestamp_ms = (uint32_t)(1000/sampling_frequency * i);
    }

    uint16_t min = find_min(data, length); 
    uint16_t max = find_max(data, length);
    double mean = calculate_mean(data, length); 
    size_t count =  count_above_threshold(data,length, threshold); 

    normalize(data, normalized_data, length);



    printf("Physiological Signal Analyzer\n");
    printf("_____________________________\n");
    printf("Samples:      %zu            \n",length);
    printf("Minimum:      %u             \n",min);
    printf("Maximum:      %u             \n",max);
    printf("Mean:         %f             \n",mean);
    printf("Above 2000:   %zu            \n",count);


    return 0;
}
