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
#define PWM_DUTY_MIN 20


gains_t YAW_GAINS = {3000,0,40, -1000};
gains_t ALT_GAINS = {80 , 0, 5, -100};


//static int16_t previous_desired_pos = 0;
static int8_t yaw_incr = 0;

void set_desired_pos(pos_t* desired_pos) {


    if(checkButton(UP) == PUSHED) {
        desired_pos->alt -= TEN_PERCENT_CHANGE; // Up button increases altitude
        if (desired_pos->alt < ALT_MAX) {   // Ensures the desired altitude can never exceed 95%.
            desired_pos->alt = ALT_MAX;
        }
    } else if (checkButton(DOWN) == PUSHED) {
        desired_pos->alt += TEN_PERCENT_CHANGE; // Down button increases altitude
        if (desired_pos->alt > ALT_MIN) {   // Ensures the desired altitude can never drop below 5%.
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

    static int16_t slow_tick = 0;

    static uint16_t last_desired_position = 0;

    gains_t gains = ALT_GAINS;
    //if (desired_position == previous_desired_pos) {
   int32_t error =  desired_position - current_position;


   int32_t PWM_duty = gains.Kp * error  + gains.Ki * alt_accumulated_error; //+ gains.Kd * (error - prev_yaw_error) //Previous error will cause some trouble, so only have PI implemented at the moment.
   PWM_duty /= gains.divisor; //Reduces duty cycle to reasonable size

    if(PWM_duty > PWM_DUTY_MAX) {
        PWM_duty = PWM_DUTY_MAX; //Ensures the duty cycle does not exceed the maximum of PWM_DUTY_MAX.
    } else if (PWM_duty < PWM_DUTY_MIN) {
        PWM_duty = PWM_DUTY_MIN; //Ensures the duty cycle does not drop below the minimum of PWM_DUTY_MIN.
    }

    if (desired_position != last_desired_position) {
            alt_accumulated_error = 0;
    }

    if (slow_tick > 1000 && alt_accumulated_error < 5000 && alt_accumulated_error > -5000 ) {
            alt_accumulated_error += error;
            slow_tick = 0;
    }

    //prev_yaw_error = error;
    //alt_accumulated_error += error;

    /*} else {

        accumulated_error = 0; //If the desired position changes, the accumulated error is reset to combat integral windup.
    }*/
    //previous_desired_pos = desired_position;

    last_desired_position = desired_position;

    slow_tick++;

    return PWM_duty;
}



int16_t yaw_controller(int32_t desired_position, int32_t current_position)
{
    gains_t gains = YAW_GAINS;
    static int16_t slow_tick = 0;


    //if (desired_position == previous_desired_pos) {
   int32_t error =  desired_position - current_position;
   static int32_t last_desired_position = 0;

   if (error > 224) {

       error -= 448;
   } else if (error < -223) {

          error += 448;
   }

   int16_t PWM_duty = (gains.Kp * error  + gains.Ki * yaw_accumulated_error) / gains.divisor; //+ gains.Kd * (error - prev_yaw_error) //Previous error will cause some trouble, so only have PI implemented at the moment.

    if(PWM_duty > PWM_DUTY_MAX) {
        PWM_duty = PWM_DUTY_MAX; //Ensures the duty cycle does not exceed the maximum of PWM_DUTY_MAX.
    } else if (PWM_duty < PWM_DUTY_MIN) {
        PWM_duty = PWM_DUTY_MIN; //Ensures the duty cycle does not drop below the minimum of PWM_DUTY_MIN.
    }

    if (desired_position != last_desired_position) {
                yaw_accumulated_error = 0;
    }

    if (slow_tick > 1000) {
        if(yaw_accumulated_error < YAW_T_MAX && yaw_accumulated_error > -YAW_T_MAX ) {
        yaw_accumulated_error += error;
        slow_tick = 0;
        }
    }



//    if(error < 2 && error > -2) {
//        yaw_accumulated_error = 0;
//    }

    /*} else {

        accumulated_error = 0; //If the desired position changes, the accumulated error is reset to combat integral windup.
    }*/
    //previous_desired_pos = desired_position;

    last_desired_position = desired_position;

    slow_tick++;
    return PWM_duty;
}




int32_t return_iyaw_error(void){

    return yaw_accumulated_error;
}

int32_t return_ialt_error(void){

    return alt_accumulated_error;
}

void reset_yaw_incr(void) {
    yaw_incr = 0;
}


