// ***********************************************************
// AUTHOR        : Ben Stirling and Oliver Clements
// CREATE DATE   : 7/3/2023
// PURPOSE       : PWM configuration
//
// ***********************************************************

// Libraries
#include <stdint.h>
#include <stdbool.h>

// Modules and drivers
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/pin_map.h" //Needed for pin configure
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/systick.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "utils/ustdlib.h"
#include "stdlib.h"
#include "PWM.h"


void initialisePWM (void)
{
    // Initiation of the hardware for PWM generators
    SysCtlPeripheralEnable(PWM_MAIN_PERIPH_PWM);
    SysCtlPeripheralEnable(PWM_MAIN_PERIPH_GPIO);

    GPIOPinConfigure(PWM_MAIN_GPIO_CONFIG);

    SysCtlPeripheralEnable(PWM_TAIL_PERIPH_PWM);
    SysCtlPeripheralEnable(PWM_TAIL_PERIPH_GPIO);

    GPIOPinConfigure(PWM_TAIL_GPIO_CONFIG);

    GPIOPinTypePWM(PWM_MAIN_GPIO_BASE, PWM_MAIN_GPIO_PIN);
    GPIOPinTypePWM(PWM_TAIL_GPIO_BASE, PWM_TAIL_GPIO_PIN);

    PWMGenConfigure(PWM_MAIN_BASE, PWM_MAIN_GEN ,
                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);

    PWMGenConfigure(PWM_TAIL_BASE, PWM_TAIL_GEN ,
                        PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    // Set the initial PWM parameters
    setPWM_main (PWM_FIXED_DUTY);
    setPWM_tail (PWM_FIXED_DUTY);

    PWMGenEnable(PWM_MAIN_BASE, PWM_MAIN_GEN);
    PWMGenEnable(PWM_TAIL_BASE, PWM_TAIL_GEN);

    // Disable the output.  Repeat this call with 'true' to turn O/P on.
    PWMOutputState(PWM_MAIN_BASE, PWM_MAIN_OUTBIT, false);
    PWMOutputState(PWM_TAIL_BASE, PWM_TAIL_OUTBIT, false);
    //SysCtlPeripheralReset (PWM_MAIN_PERIPH_GPIO);
}


void setPWM_main (uint32_t ui32Duty)
{
    // Calculate the PWM period corresponding to the freq.
    uint32_t ui32Period =
        SysCtlClockGet() / PWM_DIVIDER / PWM_START_RATE_HZ;

    PWMGenPeriodSet(PWM_MAIN_BASE, PWM_MAIN_GEN, ui32Period);
    PWMPulseWidthSet(PWM_MAIN_BASE, PWM_MAIN_OUTNUM,
        ui32Period * ui32Duty / 100);
}

void setPWM_tail (uint32_t ui32Duty) {

    // Calculate the PWM period corresponding to the freq.
        uint32_t ui32Period =
            SysCtlClockGet() / PWM_DIVIDER / PWM_START_RATE_HZ;

        PWMGenPeriodSet(PWM_TAIL_BASE, PWM_TAIL_GEN, ui32Period);
        PWMPulseWidthSet(PWM_TAIL_BASE, PWM_TAIL_OUTNUM,
            ui32Period * ui32Duty / 100);
}
