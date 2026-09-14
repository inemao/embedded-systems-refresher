#ifndef FSM_H    // Include guard: checks if FSM_H is not defined
#define FSM_H    // Defines FSM_H


typedef enum {
    STATE_OFF,
    STATE_IDLE,
    STATE_ACQUIRING,
    STATE_ERROR
} DeviceState;

typedef enum {
    EVENT_POWER_ON,
    EVENT_START,
    EVENT_STOP,
    EVENT_SENSOR_FAULT,
    EVENT_RESET
} DeviceEvent;


DeviceState handle_event(
    DeviceState current_state,
    DeviceEvent event
);


const char *state_to_string(DeviceState state);

#endif // FSM_H
