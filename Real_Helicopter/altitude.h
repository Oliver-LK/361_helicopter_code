// ***********************************************************
// AUTHOR        : Ben Stirling and Oliver Clements
// CREATE DATE   : 17/4/2023
// PURPOSE       : Altitude module
//
// **********************************************************


#ifndef ALTITUDE_H
#define ALTITUDE_H

// Libaries
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





int32_t give_adc_percent(int32_t adc_av, uint32_t ADC_OFFSET);

int32_t percentage_calc(int32_t adc_av, int32_t ADC_offset);

int32_t give_adc_av(void);

//uint16_t give_adc_offset(void);

#endif
