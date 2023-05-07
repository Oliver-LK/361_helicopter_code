#ifndef STATE_H
#define STATE_H


#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

typedef enum {

    LANDED = 0,
    TAKEOFF,
    FLYING

} heli_state;

typedef struct {
    int32_t alt_desired;
    int16_t yaw_desired;

} error_t;

typedef struct {

    bool motors_on = 0;
    bool is_calibrated = 0;


} task_t;

heli_state change_state(heli_state current_heli_state, task_t tasks);
// Changes the helicopter's states using an FSM.



error_t set_desired_pos(error_t desired_pos, heli_state current_heli_state);
//Reads the buttons and alters the desired position for the helicopter. Maybe should be elsewhere.


heli_state get_heli_state(void);
//returns the heli_state to avoid the use of global variables.

#endif
