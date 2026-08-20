#include <stdio.h>
#include "signal.h"

uint16_t find_min(
    const Sample *samples,
    size_t length
){
    if(length==0){
        printf("Error: Length is 0\n");
        return 0;
    }
    uint16_t min = samples[0].adc_value;
    for(size_t i = 1; i < length; i++){
        if(min > samples[i].adc_value){
            min = samples[i].adc_value;
        }
    }
    return min;    
}

uint16_t find_max(
    const Sample *samples,
    size_t length
){
    if(length==0){
        printf("Error: Length is 0\n");
        return 0;
    }
    uint16_t max = samples[0].adc_value;
    for(size_t i = 1; i < length; i++){
        if(max < samples[i].adc_value){
            max = samples[i].adc_value;
        }
    }
    return max;    
}

double calculate_mean(
    const Sample *samples,
    size_t length
){
    if(length==0){
        printf("Error: Length is 0\n");
        return 0.0;
    }
    uint32_t sum = 0; 
    for(size_t i = 0; i < length; i++){
        sum += samples[i].adc_value;
    }
    double mean = (double)sum/(double)length;
    return mean;    
}

size_t count_above_threshold(
    const Sample *samples,
    size_t length,
    uint16_t threshold
){
    if(length==0){
        printf("Error: Length is 0\n");
        return (size_t)0;
    }
    size_t count =0;
    for (size_t i = 0; i < length; i++){
        if(samples[i].adc_value > threshold){
            count++;
        }
    }
    return count;
}

void normalize(
    const Sample *input,
    float *output,
    size_t length
){  
    if(length==0){
        printf("Error: Length is 0\n");
        return;
    }
    uint16_t min = find_min(input, length);
    uint16_t max = find_max(input, length);
    uint16_t den = (min == max) ? 1 : (max-min);
    for (size_t i = 0; i < length; i++){
        output[i] = (float)(input[i].adc_value - min)/den;
    }
}
