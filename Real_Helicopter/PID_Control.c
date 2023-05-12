

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "PID_Control.h"
#include "buttons4.h"

#define PWM_DUTY_MAX 70
#define PWM_DUTY_MIN 2

#define TEN_PERCENT_CHANGE 121 //10% of the voltage range from 0% altitude to 100% altitude.


void set_desired_pos(pos_t* desired_pos) {

    static int8_t yaw_incr = 0;
    if(checkButton(UP) == PUSHED) {
        desired_pos->alt -= TEN_PERCENT_CHANGE; // Up button increases altitude
        if (desired_pos->alt < ALT_MAX) {   // Ensures the desired altiude can never exceed 95%.
            desired_pos->alt = ALT_MAX;
        }
    } else if (checkButton(DOWN) == PUSHED) {
        desired_pos->alt += TEN_PERCENT_CHANGE; // Down button increases altitude
        if (desired_pos->alt > ALT_MIN) {   // Ensures the desired altiude can never drop below 5%.
            desired_pos->alt = ALT_MIN;
        }
    } else if (checkButton(LEFT) == PUSHED) {
       yaw_incr--;  //Anticlockwise is negative.
   } else if (checkButton(RIGHT) == PUSHED) {
       yaw_incr++; //Clockwise is negative.
   }
   desired_pos->yaw = (yaw_incr * 15*448)/360; //Calculates the desired yaw.

}


int16_t yaw_controller(int32_t desired_position, int32_t current_position, gains_t gains)
{
    //This function is designed to be called twice - once for altitude, once for yaw.

    //if (desired_position == previous_desired_pos)
   int16_t PWM_duty = gains.Kp * yaw_error  + gains.Ki * yaw_error_incr + gains.Kd * (yaw_error_deriv); //Previous error will cause some trouble, so only have PI implemented at the moment.
   PWM_duty /= gains.divisor; //Reduces duty cycle to reasonable size

    if(PWM_duty > PWM_DUTY_MAX) {
        PWM_duty = PWM_DUTY_MAX; //Ensures the duty cycle does not exceed the maximum of PWM_DUTY_MAX.
    } else if (PWM_duty < PWM_DUTY_MIN) {
        PWM_duty = PWM_DUTY_MIN; //Ensures the duty cycle does not drop below the minimum of PWM_DUTY_MIN.
    }



    /*} else {

        accumulated_error = 0; //If the desired position changes, the accumulated error is reset to combat integral windup.
    }*/

    return PWM_duty;
}

int16_t alt_controller(int32_t desired_position, int32_t current_position, gains_t gains)
{
    //This function is designed to be called twice - once for altitude, once for yaw.

    //if (desired_position == previous_desired_pos)
   int16_t PWM_duty = gains.Kp * alt_error  + gains.Ki * alt_error_incr + gains.Kd * (alt_error_deriv); //Previous error will cause some trouble, so only have PI implemented at the moment.
   PWM_duty /= gains.divisor; //Reduces duty cycle to reasonable size

    if(PWM_duty > PWM_DUTY_MAX) {
        PWM_duty = PWM_DUTY_MAX; //Ensures the duty cycle does not exceed the maximum of PWM_DUTY_MAX.
    } else if (PWM_duty < PWM_DUTY_MIN) {
        PWM_duty = PWM_DUTY_MIN; //Ensures the duty cycle does not drop below the minimum of PWM_DUTY_MIN.
    }

    /*} else {

        accumulated_error = 0; //If the desired position changes, the accumulated error is reset to combat integral windup.
    }*/

    return PWM_duty;
}
