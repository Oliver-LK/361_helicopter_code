


#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "PID_Control.h"

#define PWM_DUTY_MAX 55
#define PWM_DUTY_MIN 5

//int16_t array[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15, 224};

static int16_t prev_yaw_error = 0;
static int16_t previous_desired_pos = 0;
static int16_t accumulated_error = 0;

gains_t K_yaw = {10,1,10, 10000};
gains_t K_alt = {3,1,20, 1000};

int16_t controller(int32_t desired_position, int32_t current_position, gains_t gains)
{
    //This function is designed to be called twice - once for altitude, once for yaw.

    if (desired_position == previous_desired_pos) {
        yaw_error = desired_position - current_position;


       PWM_duty = gains.Kp * yaw_error + gains.Kd * (yaw_error - prev_yaw_error) + gains.Ki * accumulated_error;
       PWM_duty /= gains.divisor;

        if(PWM_duty > PWM_DUTY_MAX) {
            PWM_duty = PWM_DUTY_MAX;
        } else if (PWM_duty < PWM_DUTY_MIN) {
            PWM_duty = PWM_DUTY_MIN;
        }

        prev_yaw_error = yaw_error;
        accumulated_error += yaw_error;

    } else {

        accumulated_error = 0;
    }
    previous_desired_pos = desired_position;

    return PWM_duty;
}
