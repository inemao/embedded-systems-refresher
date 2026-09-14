#include "fsm.h"


DeviceState handle_event(DeviceState current_state, DeviceEvent event)
{
    switch (current_state) {

        case STATE_OFF:
            switch (event) {

                case EVENT_POWER_ON:
                    return STATE_IDLE;

                default:
                    return STATE_OFF;
            }

        case STATE_IDLE:
            switch (event) {

                case EVENT_START:
                    return STATE_ACQUIRING;

                case EVENT_SENSOR_FAULT:
                    return STATE_ERROR;

                default:
                    return STATE_IDLE;

            }

        case STATE_ACQUIRING:
            switch (event) {

                case EVENT_STOP:
                    return STATE_IDLE;

                case EVENT_SENSOR_FAULT:
                    return STATE_ERROR;

                default:
                    return STATE_ACQUIRING;

            }

        case STATE_ERROR:
            switch (event) {

                case EVENT_RESET:
                    return STATE_IDLE;

                default:
                    return STATE_ERROR;

            }

        default:
            return STATE_ERROR;
    }
}

const char *state_to_string(DeviceState state){
    switch (state) {

        case STATE_OFF:
            return "OFF";

        case STATE_IDLE:
            return "IDLE";

        case STATE_ACQUIRING:
            return "ACQUIRING";

        case STATE_ERROR:
            return "ERROR";

        default:
            return "UNKNOWN";

    }
}
