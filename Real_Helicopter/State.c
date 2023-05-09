#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "State.h"


#define ALT_MIN 2358 //95%  of the maximum voltage, which is 5% above 0 altitude.
#define ALT_MAX 1365 //5% of the maximum voltage, which is 95% above 0 altitude.
#define TEN_PERCENT_CHANGE 121 //10% of the voltage range from 0% altitude to 100% altitude.

static heli_state_t current_heli_state;



heli_state_t change_state(heli_state_t current_heli_state, task_t* tasks) {
    // FSM which decides when the helicopter can transition between the flying, landed, and takeoff states.
    if (current_heli_state == LANDED && (tasks->motors_on == 0)) {
        //State can only change if the motors are off.
        current_heli_state = TAKEOFF;

    } else if (current_heli_state == TAKEOFF && tasks->is_calibrated == 1){
        //State can only change if the yaw has been calibrated absolutely.
       current_heli_state = FLYING;

    } else if (current_heli_state == FLYING ){
        //Helicopter can only land if it is flying, not in the takeoff state. The helicopter needs to land facing the reference direction, so it needs to be calibrated before it can land.
        current_heli_state = LANDED;
    }

    return current_heli_state;

}


error_t* set_desired_pos(error_t* desired_pos, heli_state_t current_heli_state) {
    static int8_t yaw_incr = 0; // multiplication counter to ensure the desired yaw doesn't drift off the true yaw angle after multiple yaw increases (as 15 deg * 448/360 ~ 18.67)
    switch(current_heli_state) {

    case LANDED:

        desired_pos->yaw_desired = 0; //Causes the helicopter's PID controller to rotate to the reference yaw.
        desired_pos->alt_desired = ALT_MIN + TEN_PERCENT_CHANGE/2; // Causes the PID controller to aim for the minimum altitude.
        break;

    case TAKEOFF:

        desired_pos->alt_desired = ALT_MIN; //Sets the hover height.
        //The heli needs to yaw until the reference is found. This is yet to be implemented.
        break;
    case FLYING:
        if(button_event(UP) == PUSHED) {
            desired_pos->alt_desired -= TEN_PERCENT_CHANGE; // Up button increases altitude
            if (desired_pos->alt_desired < ALT_MAX) {   // Ensures the desired altiude can never exceed 95%.
                desired_pos->alt_desired = ALT_MAX;
            }
        } else if (button_event(DOWN) == PUSHED) {
            desired_pos->alt_desired += TEN_PERCENT_CHANGE; // Down button increases altitude
            if (desired_pos->alt_desired > ALT_MIN) {   // Ensures the desired altiude can never drop below 5%.
                desired_pos->alt_desired = ALT_MIN;
            }
        } else if (button_event(LEFT) == PUSHED) {
           yaw_incr--;  //Anticlockwise is negative.
       } else if (button_event(RIGHT) == PUSHED) {
           yaw_incr++; //Clockwise is negative.
       }
       desired_pos->yaw_desired = (yaw_incr * 15*448)/360; //Calculates the desired yaw.
       break;
    }


    return desired_pos;
}

heli_state_t get_heli_state(void) {

    return current_heli_state;
}
