#ifndef PID_CONTROL_H
#define PID_CONTROL_H

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
#include "altitude.h"
#include "inc/hw_memmap.h"

// Global constants
#define TAIL_PWM_DUTY_MAX 50
#define TAIL_PWM_DUTY_MIN 13

#define MAIN_PWM_DUTY_MAX 68
#define MAIN_PWM_DUTY_MIN 15

#define ADC_HOVER_OFFSET 50

#define HALF_YAW_CYCLE 224
#define FULL_YAW_CYCLE 448
#define YAW_T_MAX 2700

#define ALT_I_MAX 5000
#define ALT_I_SAMPLE_RATE 1000
#define ALT_T_MAX 9000

#define YAW_I_SAMPLE_RATE 500
#define YAW_D_SAMPLE_RATE 3000

#define SWITCH_YAW_DIRECTION_COUNT 12
#define DIRECTION_YAW_SWITCH 24
#define YAW_STEP_SIZE 15

#define TEN_PERCENT_CHANGE ((ALT_MIN - ALT_MAX) * 10 ) / 100

#define CIRCLE_DEG 360
#define MAX_STR_LEN 16

typedef struct {
    int32_t alt;
    int16_t yaw;

} pos_t;
// Position struct containing all positional data of the helicopter


typedef struct {

    int32_t Kp;
    int32_t Kd;
    int32_t Ki;
    int16_t divisor;

} gains_t;
// PID gains struct


// Global varaibles

char statusStr[MAX_STR_LEN + 1];
// UART string for send

static int32_t PWM_duty = 0;
static int32_t yaw_PWM_duty = 0;
static int32_t yaw_accumulated_error = 0;
static int32_t yaw_prev_pos = 0;
static int32_t alt_accumulated_error = 0;
static int16_t derivative_error = 0;
// Errors for PID controller

// Function deceleration
void set_desired_pos(pos_t* desired_pos);
// Sets the desired postion of the helicopter using the position struct

int16_t alt_controller(int32_t desired_position, int32_t current_position);
// PID controller for the altitude of the helicopter

int16_t yaw_controller(int32_t desired_position, int32_t current_position);
// PID controller for the yaw of the helicopter

int32_t get_yaw_PWM(void);

int32_t get_alt_PWM(void);

//OH GOD DELETE THIS BEFORE SAM FINDS IT

// Debugging functions
int32_t return_iyaw_error(void);
// returns the integral error from PID yaw controller

int32_t return_ialt_error(void);
// returns the integral error from PID altitude controller

void reset_yaw_incr(void);
// Resets the yaw controller incrementor

int32_t return_prepre_yaw_error(void);
// returns the derivative yaw response

void initialise_adc_offset (int32_t init_adc_off);
// Finds the adc offset for the helicopter for hovering altitude

void reset_yaw_accum(void);
// Force resets the integral build up from the yaw controller



#endif //PID_CONTROL_H
