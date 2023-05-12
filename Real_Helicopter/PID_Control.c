


#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "PID_Control.h"
#include "buttons4.h"

#define PWM_DUTY_MAX 55
#define PWM_DUTY_MIN 5

#define ALT_MIN 2358 //95%  of the maximum voltage, which is 5% above 0 altitude.
#define ALT_MAX 1365 //5% of the maximum voltage, which is 95% above 0 altitude.
#define TEN_PERCENT_CHANGE 121 //10% of the voltage range from 0% altitude to 100% altitude.




//int16_t array[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15, 224};


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
   desired_pos->yaw = (yaw_incr * 15*448)/360; //Calculates the desired yaw.

}


int16_t controller(int32_t desired_position, int32_t current_position, gains_t gains)
{
    //This function is designed to be called twice - once for altitude, once for yaw.

    //if (desired_position == previous_desired_pos) {
   error =  desired_position - current_position;


   int16_t PWM_duty = gains.Kp * error  + gains.Ki * accumulated_error; //+ gains.Kd * (error - prev_yaw_error) //Previous error will cause some trouble, so only have PI implemented at the moment.
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
