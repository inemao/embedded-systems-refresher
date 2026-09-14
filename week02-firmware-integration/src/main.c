#include <stdio.h>
#include <stdint.h>
#include "ring_buffer.h"
#include "adc.h"
#include "fsm.h"
#include "status.h"


void adc_isr(RingBuffer *buffer)
{
    uint16_t sample = adc_read();

    if (!ring_buffer_push(buffer, sample)) {
        status_set(STATUS_BUFFER_OVERFLOW);
        overflow_count_increment();
    }
}

int main(void)
{
    RingBuffer buffer;
    DeviceState state = STATE_OFF;
    uint16_t sample;
    int high_water_active = 0;

    ring_buffer_init(&buffer);
    adc_init();
    status_init();

    state = handle_event(state, EVENT_POWER_ON);

   
    state = handle_event(state, EVENT_START);

    
    for (size_t time_ms = 0; time_ms < 30; time_ms++) {

    if (state == STATE_ACQUIRING) {

        
        if (!(time_ms >= 5 && time_ms <= 24)) {
            for (size_t i = 0; i < 2; i++) {
                if (ring_buffer_pop(&buffer, &sample)) {
                    printf("Processed at %zu ms: %u\n",
                    time_ms,
                    sample);
                }
            }
        }   

        adc_isr(&buffer);
        size_t count = ring_buffer_count(&buffer);

        if (count >= HIGH_WATER_MARK && high_water_active == 0) {
            printf("WARNING: high water\n");
        high_water_active = 1;
        }

        if (count <= CLEAR_WATER_MARK && high_water_active == 1) {
            printf("High water cleared\n");
            high_water_active = 0;
        }

        /*if (status_is_set(STATUS_BUFFER_OVERFLOW)) {
            state = handle_event(
                state,
                EVENT_SENSOR_FAULT
            );

            status_clear(
                STATUS_BUFFER_OVERFLOW
            );
        }*/

        if (time_ms == 25) {
        size_t overflows = overflow_count_take();

        if (overflows > 0) {
            printf("Overflow events: %zu\n", overflows);
        }
}

        printf("State: %s\n", state_to_string(state));

        if (state == STATE_ERROR) {
            ring_buffer_init(&buffer);
            state = handle_event(state, EVENT_RESET);

            printf("State: %s\n", state_to_string(state));
        }
    }
    

    }



    return 0;
}
