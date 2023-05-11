#ifndef PID_CONTROL_H
#define PID_CONTROL_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>


typedef struct {

    uint8_t Kp;
    uint8_t Kd;
    uint8_t Ki;
    uint16_t divisor;

} gains_t;


int16_t controller(int32_t desired_position, int32_t current_position, gains_t gains);

#endif //PID_CONTROL_H
