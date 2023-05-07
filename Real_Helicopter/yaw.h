// ***********************************************************
// AUTHOR        : Ben Stirling and Oliver Clements
// CREATE DATE   : 04/05/2023
// PURPOSE       : Yaw Module
//
// ***********************************************************

#ifndef YAW_H
#define YAW_H

// Libraries
#include <stdint.h>
#include <stdbool.h>

// Modules and drivers
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/adc.h"
#include "driverlib/pwm.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "utils/ustdlib.h"
#include "circBufT.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "display.h"
#include "ADC.h"
#include "buttons4.h"

void yaw_ISR(void);
//ISR to increment/decrement yaw_counter which keeps track of the rotation of the helicopter.

int32_t get_yaw(void);
//Returns the yaw (in degree form) of the helicopter for displaying the yaw. This avoids global variable use.

int8_t yaw_decimal(void);
//Calculates the decimal part of the yaw position.

int32_t get_yaw_counter(void);
//returns the yaw counter variable.

void abs_yaw_ISR(void);
#endif
