#ifndef ALTITUDE_H
#define ALTITUDE_H

// ***********************************************************
// AUTHOR        : Ben Stirling and Oliver Clements
// CREATE DATE   : 17/4/2023
// PURPOSE       : Altitude module
//
// **********************************************************


// Libraries
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

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
#include "ADC.h"
#include "buttons4.h"

// Predefined alts max and min
#define ALT_MIN 3015 //95%  of the maximum voltage, which is 5% above 0 altitude.
#define ALT_MAX 1600 //5% of the maximum voltage, which is 95% above 0 altitude.

// Function decelerations
int32_t give_adc_percent(int32_t adc_av, int32_t ADC_offset);
//Returns calculates and returns altitude percentage, avoiding the use of global variables.

int32_t give_adc_av(void);
// Returns the average adv value from teh circular buffer

#endif
