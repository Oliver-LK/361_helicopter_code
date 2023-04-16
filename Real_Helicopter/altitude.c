// ***********************************************************
// AUTHOR        : Ben Stirling and Oliver Clements
// CREATE DATE   : 17/4/2023
// PURPOSE       : Altitude module
//
// **********************************************************


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
#include "altitude.h"

// Global Constants
#define ADC_min 1241 // 1 volt in ADC counts when ADC is set to 3.3V
#define ADC_max 2482 // 2 volts in ADC counts when ADC is set to 3.3V
#define ADC_range (ADC_max - ADC_min)

uint16_t give_adc_offset(void)
{
    uint16_t i;
    int32_t sum = 0;

    for (i = 0; i < BUF_SIZE; i++)
                    sum = sum + readCircBuf (&g_inBuffer);
        uint16_t ADC_offset = (2 * sum + BUF_SIZE) / 2 / BUF_SIZE;

    return ADC_offset;
}

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

int32_t percentage_calc(int32_t adc_av, int32_t ADC_offset)
{

    int32_t percentage = ((  ADC_offset - adc_av )*100/ADC_range);

    return percentage;

}

int32_t give_adc_percent(int32_t adc_av, uint32_t ADC_offset)
{
    int32_t percentage = percentage_calc(adc_av, ADC_offset);
    return percentage;
}


