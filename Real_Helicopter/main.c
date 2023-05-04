// ***********************************************************
// AUTHOR        : Ben Stirling and Oliver Clements 
// CREATE DATE   : 17/3/2023
// PURPOSE       : The main module with int main() loop
//                 
// ***********************************************************

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
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "display.h"
#include "ADC.h"
#include "buttons4.h"
#include "yaw.h"
#include "altitude.h"



void initClock (void)
{
    // Set the clock rate to 20 MHz
    SysCtlClockSet (SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);
    //
    // Set up the period for the SysTick timer.  The SysTick timer period is
    // set as a function of the system clock.
    SysTickPeriodSet(SysCtlClockGet() / SAMPLE_RATE_HZ);
    //
    // Register the interrupt handler
    SysTickIntRegister(SysTickIntHandler);
    //
    // Enable interrupt and device
    SysTickIntEnable();
    SysTickEnable();
}


// *******************************************************




void do_init(void)
{
    //Initialisation function to tidy up the start of the program.
    initClock ();
    initADC ();
    initDisplay ();
    initCircBuf (&g_inBuffer, BUF_SIZE);
    initButtons ();

    //  Interrupt Pins initialisation for yaw monitoring.

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
//    GPIODirModeSet(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1,
//                   GPIO_DIR_MODE_IN);

    GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1,
                   GPIO_BOTH_EDGES);

    GPIOIntRegister(GPIO_PORTB_BASE, yaw_ISR);

    GPIOIntEnable(GPIO_PORTB_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1);
}

//#############################################  END OF INIT ########################################



int main(void) {

    int32_t sum = 0; // Sum variable for reading circular buffer.
    uint16_t i;
    // Calls initialisation function
    do_init();

    // Enable interrupts to the processor.
    IntMasterEnable();



    SysCtlDelay (3000000); //Delays the system to allow the circular buffer to fill up.

    for (i = 0; i < BUF_SIZE; i++)
                sum = sum + readCircBuf (&g_inBuffer);
    uint16_t ADC_offset = give_adc_offset(); //Sets ADC offset.


    while (1)
    {
        // Calculate and display the rounded mean of the buffer contents
        int32_t adc_av =  give_adc_av();
        int32_t percentage = give_adc_percent(adc_av, ADC_offset);




        if(button_event(LEFT) == PUSHED) {
            ADC_offset = adc_av; // Sets the new zero-point for the altitude.
        }

        displayPos(percentage, get_yaw(), yaw_decimal()); // Displays the helicopter's position.


        //Redundant Code
/*        switch(displaystate)
        {
            case(percentageState):
                displayPercentage(percentage);
                break;

            case(meanState):
                displayMeanVal(displayPercentage(percentage), yaw_decimal());
                break;

            case(blankState):
                displayBlank();
                break;

        }*/
        //        displayMeanVal (adc_av, g_ulSampCnt);
        /*
                if(button_event(UP) == PUSHED) {
                    display_change();
                }
        */

    }

}
