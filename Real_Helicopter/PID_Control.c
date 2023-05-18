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
#include "UART.h"
// Global Constants
#define PWM_DUTY_MAX 70
#define PWM_DUTY_MIN 10


gains_t YAW_GAINS = {300, 000 ,4, -100};
gains_t ALT_GAINS = {80 , 0, 5, -100};

static int32_t adc_offset = 0;

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
        if (desired_pos->alt > adc_offset - 50) {   // Ensures the desired altitude can never drop below 5%.
            desired_pos->alt = adc_offset - 50;
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


   int32_t PWM_duty = gains.Kp * error  + gains.Ki * alt_accumulated_error;// + gains.Kd * (error - pre_yaw_error); //Previous error will cause some trouble, so only have PI implemented at the moment.
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
    static int16_t slow_tick1 = 0;


    //if (desired_position == previous_desired_pos) {
   int32_t error =  desired_position - current_position;
   static int32_t last_desired_position = 0;

   if (error > 224) {

       error -= 448;
   } else if (error < -223) {

          error += 448;
   }

   int16_t yaw_PWM_duty = (gains.Kp * error  + gains.Ki * yaw_accumulated_error ) / gains.divisor;//  + gains.Kd * (error - pre_yaw_error[0])) / gains.divisor; //Previous error will cause some trouble, so only have PI implemented at the moment.

    if(yaw_PWM_duty > PWM_DUTY_MAX) {
        yaw_PWM_duty = PWM_DUTY_MAX; //Ensures the duty cycle does not exceed the maximum of PWM_DUTY_MAX.
    } else if (yaw_PWM_duty < PWM_DUTY_MIN) {
        yaw_PWM_duty = PWM_DUTY_MIN; //Ensures the duty cycle does not drop below the minimum of PWM_DUTY_MIN.
    }

    if (desired_position != last_desired_position) {
                yaw_accumulated_error = 0;
    }

    if (slow_tick > 800) {

        if (error > 0 && yaw_accumulated_error < YAW_T_MAX) {
            yaw_accumulated_error += error;

        } else if ( error < 0 && yaw_accumulated_error > (-1 * YAW_T_MAX)) {
            yaw_accumulated_error += error;

        }


        slow_tick = 0;
    }

    if(slow_tick1 > 10000) {
        slow_tick1 = 0;
        usprintf (statusStr, "============ \r\n"); // * usprintf
        UARTSend (statusStr);
        usprintf (statusStr, "Yaw Error: %4i \r\n", error); // * usprintf
        UARTSend (statusStr);
        usprintf (statusStr, "Yaw Desired: %4i \r\n", desired_position); // * usprintf
        UARTSend (statusStr);
        usprintf (statusStr, "Yaw Current: %4i \r\n", current_position); // * usprintf
        UARTSend (statusStr);
        usprintf (statusStr, "Yaw Duty: %4i \r\n", yaw_PWM_duty); // * usprintf
                UARTSend (statusStr);
                usprintf (statusStr, "Yaw I: %4i \r\n", yaw_accumulated_error); // * usprintf
                                UARTSend (statusStr);


        }


    last_desired_position = desired_position;


//    if(error < 2 && error > -2) {
//        yaw_accumulated_error = 0;
//    }

    /*} else {

        accumulated_error = 0; //If the desired position changes, the accumulated error is reset to combat integral windup.
    }*/
    //previous_desired_pos = desired_position;

//    pre_yaw_error[9] = error;
//    int16_t index = 0;
//
//    // CANT FIGURE EOUT WHY LOOP ISNT WORKING
////    for (index = 0; index < 8; index++) {
////        pre_yaw_error[index] = pre_yaw_error[index + 1];
////    }
//    pre_yaw_error[0] = pre_yaw_error[1];
//    pre_yaw_error[1] = pre_yaw_error[2];
//    pre_yaw_error[2] = pre_yaw_error[3];
//    pre_yaw_error[3] = pre_yaw_error[4];
//    pre_yaw_error[4] = pre_yaw_error[5];
//    pre_yaw_error[5] = pre_yaw_error[6];
//    pre_yaw_error[6] = pre_yaw_error[7];
//    pre_yaw_error[7] = pre_yaw_error[8];
//    pre_yaw_error[8] = pre_yaw_error[9];


    slow_tick++;
    slow_tick1++;
    return yaw_PWM_duty;


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

void reset_yaw_accum(void) {
    yaw_accumulated_error = 0;
}
//int32_t return_prepre_yaw_error(void){
//    return pre_yaw_error[0];
//}
//
void initialise_adc_offset (int32_t init_adc_off) {

    adc_offset = init_adc_off;
}
