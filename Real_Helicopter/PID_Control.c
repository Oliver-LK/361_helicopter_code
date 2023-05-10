


#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "PID_Control.h"

#define PWM_DUTY_MAX 55
#define PWM_DUTY_MIN 5

//int16_t array[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15, 224};


//static int16_t previous_desired_pos = 0;
static int16_t accumulated_error = 0;

gains_t K_yaw = {10,1,10, 100};
gains_t K_alt = {3,1,20, 100};

int16_t controller(int32_t desired_position, int32_t current_position, gains_t gains)
{
    //This function is designed to be called twice - once for altitude, once for yaw.

    //if (desired_position == previous_desired_pos) {
    int32_t error = desired_position - current_position;


   uint8_t PWM_duty = gains.Kp * error  + gains.Ki * accumulated_error; //+ gains.Kd * (error - prev_yaw_error) //Previous error will cause some trouble, so only have PI implemented at the moment.
   PWM_duty /= gains.divisor; //Reduces duty cycle to reasonable size

    if(PWM_duty > PWM_DUTY_MAX) {
        PWM_duty = PWM_DUTY_MAX; //Ensures the duty cycle does not exceed the maximum of PWM_DUTY_MAX.
    } else if (PWM_duty < PWM_DUTY_MIN) {
        PWM_duty = PWM_DUTY_MIN; //Ensures the duty cycle does not drop below the minimum of PWM_DUTY_MIN.
    }

    //prev_yaw_error = error;
    accumulated_error += error;

    /*} else {

        accumulated_error = 0; //If the desired position changes, the accumulated error is reset to combat integral windup.
    }*/
    //previous_desired_pos = desired_position;

    return PWM_duty;
}
