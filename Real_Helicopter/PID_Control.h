// ***********************************************************
// AUTHOR        : Ben Stirling and Oliver Clements
// CREATE DATE   : 7/3/2023
// PURPOSE       : PID contollers for yaw and alt
//
// ***********************************************************


#ifndef PID_CONTROL_H
#define PID_CONTROL_H

// Libraries
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

// Modules and drivers
#include "altitude.h"
#include "inc/hw_memmap.h"

typedef struct {
    int32_t alt;
    int16_t yaw;

} pos_t;

typedef struct {

    int32_t Kp;
    int32_t Kd;
    int32_t Ki;
    int16_t divisor;

} gains_t;

#define MAX_STR_LEN 16
char statusStr[MAX_STR_LEN + 1];

#define TEN_PERCENT_CHANGE ((ALT_MIN - ALT_MAX) * 10 ) / 100

static int32_t yaw_accumulated_error = 0;
static int32_t alt_accumulated_error = 0;

static int32_t pre_yaw_error[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


#define YAW_T_MAX 3000

void set_desired_pos(pos_t* desired_pos);

int16_t alt_controller(int32_t desired_position, int32_t current_position);

int16_t yaw_controller(int32_t desired_position, int32_t current_position);

int32_t return_iyaw_error(void);

int32_t return_ialt_error(void);

void reset_yaw_incr(void);

int32_t return_prepre_yaw_error(void);

void initialise_adc_offset (int32_t init_adc_off);

#endif //PID_CONTROL_H
