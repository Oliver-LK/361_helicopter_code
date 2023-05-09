#ifndef STATE_H
#define STATE_H


#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "buttons4.h"



typedef enum {

    LANDED = 0,
    TAKEOFF,
    FLYING

} heli_state_t;

typedef struct {
    int32_t alt_desired;
    int16_t yaw_desired;

} pos_t;

typedef struct {

    bool motors_on;
    bool is_calibrated;


} task_t;

heli_state_t change_state(heli_state_t current_heli_state, task_t* tasks);
// Changes the helicopter's states using an FSM.



pos_t* set_desired_pos(pos_t* desired_pos, heli_state_t current_heli_state);
//Reads the buttons and alters the desired position for the helicopter. Maybe should be elsewhere.


heli_state_t get_heli_state(void);
//returns the heli_state to avoid the use of global variables.

#endif
