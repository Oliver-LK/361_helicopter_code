// ***********************************************************
// AUTHOR        : Ben Stirling and Oliver Clements, based off code by Phil Bones. 
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
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "utils/ustdlib.h"
#include "circBufT.h"
#include "altitude.h"


int32_t give_adc_av(void)
{
    uint16_t i;
    int32_t sum = 0;

    // Background task: calculate the (approximate) mean of the values in the
    // circular buffer and display it, together with the sample number.
    for (i = 0; i < BUF_SIZE; i++)
        sum = sum + readCircBuf (&g_inBuffer);

    uint32_t adc_av = (2 * sum + BUF_SIZE) / 2 / BUF_SIZE;

    return adc_av;
}

int32_t give_adc_percent(int32_t adc_av, int32_t ADC_offset)
{
    // Calculates the height of the helicopter as a percentage.
    int32_t percentage = ((ADC_offset - adc_av )*100/ADC_range);
    return percentage;
}


