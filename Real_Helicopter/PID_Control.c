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


// PID gains
gains_t YAW_GAINS = {90, 200, 1, -100};
gains_t ALT_GAINS = {500 , 0, 5, -1000};

static int32_t adc_offset = 0;

static int8_t yaw_incr = 0;

void set_desired_pos(pos_t* desired_pos) {
    // Sets the desired position struct of the helicopter
    // based on the buttons pushed

    if(checkButton(UP) == PUSHED) {
        desired_pos->alt -= TEN_PERCENT_CHANGE; // Up button increases altitude
        if (desired_pos->alt < ALT_MAX) {   // Ensures the desired altitude can never exceed 95%.
            desired_pos->alt = ALT_MAX;

        }

    } else if (checkButton(DOWN) == PUSHED) {
        desired_pos->alt += TEN_PERCENT_CHANGE; // Down button increases altitude
        if (desired_pos->alt > adc_offset - ADC_HOVER_OFFSET) {   // Ensures the desired altitude can never drop below 5%.
            desired_pos->alt = adc_offset - ADC_HOVER_OFFSET;

        }

    } else if (checkButton(LEFT) == PUSHED) {
       yaw_incr--;  //Anticlockwise is negative.

   } else if (checkButton(RIGHT) == PUSHED) {
       yaw_incr++; //Clockwise is negative.

   }

   if (yaw_incr > SWITCH_YAW_DIRECTION_COUNT) {
       yaw_incr -= DIRECTION_YAW_SWITCH;

   } else if (yaw_incr < - SWITCH_YAW_DIRECTION_COUNT) {
       yaw_incr += DIRECTION_YAW_SWITCH;

   }

   desired_pos->yaw = (yaw_incr * YAW_STEP_SIZE * FULL_YAW_CYCLE) / CIRCLE_DEG; //Calculates the desired yaw.

}


int16_t alt_controller(int32_t desired_position, int32_t current_position)
{
    // Altitude PI controller. Operates at approximately 10kHz
    gains_t gains = ALT_GAINS;
    static int16_t slow_tick = 0;
   // static int16_t slow_tick1 = 0;
    //static uint16_t last_desired_position = 0;

    int32_t error =  desired_position - current_position;

    // PI equation based on error
    PWM_duty = (gains.Kp * error  + gains.Ki * alt_accumulated_error) / gains.divisor;// + gains.Kd * (error - pre_yaw_error); //Previous error will cause some trouble, so only have PI implemented at the moment.

    // PWM duty cycle limiter
    if(PWM_duty > MAIN_PWM_DUTY_MAX) {
        PWM_duty = MAIN_PWM_DUTY_MAX; //Ensures the duty cycle does not exceed the maximum of PWM_DUTY_MAX.

    } else if (PWM_duty < MAIN_PWM_DUTY_MIN) {
        PWM_duty = MAIN_PWM_DUTY_MIN; //Ensures the duty cycle does not drop below the minimum of PWM_DUTY_MIN.
    }

    // Integral error accumulator if conditions are met
    if (slow_tick > ALT_I_SAMPLE_RATE) {

        if (error > 0 && alt_accumulated_error < ALT_T_MAX) {
            alt_accumulated_error += error;

        } else if ( error < 0 && alt_accumulated_error > (-1 * ALT_T_MAX)) {
            alt_accumulated_error += error;

        }

        slow_tick = 0;
    }

    slow_tick++;

    return PWM_duty;
}



int16_t yaw_controller(int32_t desired_position, int32_t current_position)
{
    // Altitude PID controller. Operates at approximately 10kHz
    gains_t gains = YAW_GAINS;
    static int16_t slow_tick = 0;
    static int16_t slow_tick1 = 0;
    static int16_t slow_tick2 = 0;
    static int32_t last_desired_position = 0;

    int32_t error =  desired_position - current_position;
   
    // Optimises the rotation direction
    if (error > HALF_YAW_CYCLE) {
        error -= FULL_YAW_CYCLE;

    } else if (error < - HALF_YAW_CYCLE - 1) {
        error += FULL_YAW_CYCLE;

    }

    //  Yaw PID calculation
    yaw_PWM_duty = (gains.Kp * error  + gains.Ki * yaw_accumulated_error  + gains.Kd * (derivative_error)) / gains.divisor;
    yaw_PWM_duty += PWM_duty/ 2;
    // PWM duty cycle limiter
    if(yaw_PWM_duty > TAIL_PWM_DUTY_MAX) {
        yaw_PWM_duty = TAIL_PWM_DUTY_MAX; //Ensures the duty cycle does not exceed the maximum of PWM_DUTY_MAX.
    } else if (yaw_PWM_duty < TAIL_PWM_DUTY_MIN) {
        yaw_PWM_duty = TAIL_PWM_DUTY_MIN; //Ensures the duty cycle does not drop below the minimum of PWM_DUTY_MIN.
    }

    // Sets the accumulated integral error to zero when the desired postion changes.
    if (desired_position != last_desired_position) {
                yaw_accumulated_error = 0;
    }

    // Integral error accumulator if conditions are met for correct direction
    if (slow_tick > YAW_I_SAMPLE_RATE) {

        if (error > 0 && yaw_accumulated_error < YAW_T_MAX) {
            yaw_accumulated_error += error;

        } else if ( error < 0 && yaw_accumulated_error > (-1 * YAW_T_MAX)) {
            yaw_accumulated_error += error;

        }

        slow_tick = 0;
    }

    last_desired_position = desired_position;

    // Derivitive error calculation
    if (slow_tick2 > YAW_D_SAMPLE_RATE) {
        slow_tick2 = 0;
        derivative_error = (yaw_prev_pos- current_position);
        yaw_prev_pos = current_position;
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
            usprintf (statusStr, "Yaw D: %4i \r\n", derivative_error); // * usprintf
            UARTSend (statusStr);

        }
        slow_tick1++;
    slow_tick++;
    slow_tick2++;

    return yaw_PWM_duty;
}



int32_t get_yaw_PWM(void)
{
    return yaw_PWM_duty;
}


int32_t get_alt_PWM(void)
{
    return PWM_duty;
}



// Debugging functions
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

void initialise_adc_offset (int32_t init_adc_off) {

    adc_offset = init_adc_off;
}
