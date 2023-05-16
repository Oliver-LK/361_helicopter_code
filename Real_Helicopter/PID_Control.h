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

typedef struct {
    int32_t alt;
    int16_t yaw;

} pos_t;

typedef struct {

    int8_t Kp;
    int8_t Kd;
    int8_t Ki;
    int16_t divisor;

} gains_t;


#define TEN_PERCENT_CHANGE ((ALT_MIN - ALT_MAX) * 10 ) / 100



void set_desired_pos(pos_t* desired_pos);

int16_t alt_controller(int32_t desired_position, int32_t current_position);

int16_t yaw_controller(int32_t desired_position, int32_t current_position);



#endif //PID_CONTROL_H
