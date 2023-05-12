#ifndef PID_CONTROL_H
#define PID_CONTROL_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

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


//For real helicopter, ALT_MIN 2358  ALT_MAX 1365

#define ALT_MIN 3015 //95%  of the maximum voltage, which is 5% above 0 altitude.
#define ALT_MAX 1365 //5% of the maximum voltage, which is 95% above 0 altitude.

int16_t yaw_error = 0;
int16_t yaw_error_incr = 0;
int16_t yaw_error_deriv = 0;
int16_t prev_yaw_error = 0;

int16_t alt_error = 0;
int16_t alt_error_incr = 0;
int16_t alt_error_deriv = 0;
int16_t prev_alt_error = 0;


void set_desired_pos(pos_t* desired_pos);

int16_t yaw_controller(int32_t desired_position, int32_t current_position, gains_t gains);

int16_t alt_controller(int32_t desired_position, int32_t current_position, gains_t gains);

#endif //PID_CONTROL_H
