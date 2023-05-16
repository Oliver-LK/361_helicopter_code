// ***********************************************************
// AUTHOR        : Ben Stirling and Oliver Clements
// CREATE DATE   : 7/3/2023
// PURPOSE       : PID contollers for yaw and alt
//
// ***********************************************************

// Libraries
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

// Modules and drivers
#include "PID_Control.h"
#include "buttons4.h"

// Global Constants
#define PWM_DUTY_MAX 70
#define PWM_DUTY_MIN 5


//int16_t array[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15, 224};

gains_t YAW_GAINS = {10,0,0, -10};
gains_t ALT_GAINS = {8,0,0, -10};


//static int16_t previous_desired_pos = 0;
static int16_t accumulated_error = 0;

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

   if (yaw_incr > 12) {

       yaw_incr -= 24;
   } else if (yaw_incr < -12) {

       yaw_incr += 24;
   }
   desired_pos->yaw = (yaw_incr * 15*448)/360; //Calculates the desired yaw.

}


int16_t alt_controller(int32_t desired_position, int32_t current_position)
{
    gains_t gains = ALT_GAINS;
    //if (desired_position == previous_desired_pos) {
   int32_t error =  desired_position - current_position;


   int16_t PWM_duty = gains.Kp * error;  //+ gains.Ki * alt_accumulated_error; //+ gains.Kd * (error - prev_yaw_error) //Previous error will cause some trouble, so only have PI implemented at the moment.
   PWM_duty /= gains.divisor; //Reduces duty cycle to reasonable size

    if(PWM_duty > PWM_DUTY_MAX) {
        PWM_duty = PWM_DUTY_MAX; //Ensures the duty cycle does not exceed the maximum of PWM_DUTY_MAX.
    } else if (PWM_duty < PWM_DUTY_MIN) {
        PWM_duty = PWM_DUTY_MIN; //Ensures the duty cycle does not drop below the minimum of PWM_DUTY_MIN.
    }

    //prev_yaw_error = error;
    //alt_accumulated_error += error;

    /*} else {

        accumulated_error = 0; //If the desired position changes, the accumulated error is reset to combat integral windup.
    }*/
    //previous_desired_pos = desired_position;

    return PWM_duty;
}


int16_t yaw_controller(int32_t desired_position, int32_t current_position)
{
    gains_t gains = YAW_GAINS;

    //if (desired_position == previous_desired_pos) {
   int32_t error =  desired_position - current_position;

   if (error > 224) {

       error -= 448;
   } else if (error < -223) {

          error += 448;
   }

   int16_t PWM_duty = gains.Kp * error;//  + gains.Ki * yaw_accumulated_error; //+ gains.Kd * (error - prev_yaw_error) //Previous error will cause some trouble, so only have PI implemented at the moment.
   PWM_duty /= gains.divisor; //Reduces duty cycle to reasonable size

    if(PWM_duty > PWM_DUTY_MAX) {
        PWM_duty = PWM_DUTY_MAX; //Ensures the duty cycle does not exceed the maximum of PWM_DUTY_MAX.
    } else if (PWM_duty < PWM_DUTY_MIN) {
        PWM_duty = PWM_DUTY_MIN; //Ensures the duty cycle does not drop below the minimum of PWM_DUTY_MIN.
    }

    //prev_yaw_error = error;
    //yaw_accumulated_error += error;

    /*} else {

        accumulated_error = 0; //If the desired position changes, the accumulated error is reset to combat integral windup.
    }*/
    //previous_desired_pos = desired_position;

    return PWM_duty;
}

