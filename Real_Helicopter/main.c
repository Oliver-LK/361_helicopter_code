// ***********************************************************
// AUTHOR        : Ben Stirling and Oliver Clements 
// CREATE DATE   : 17/3/2023
// PURPOSE       : The main module with int main() loop
//                 
// ***********************************************************

// Libaries
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
#include "initialisation.h"
#include "display.h"
#include "ADC.h"
#include "buttons4.h"




// Global Constants
#define ADC_min 1241 // 1 volt in ADC counts when ADC is set to 3.3V
#define ADC_max 2482 // 2 volts in ADC counts when ADC is set to 3.3V

int16_t percentage_calc(adc_av) // this wrong mean - init value * 100
{
    int16_t percentage = 100 - (adc_av)/(ADC_max - ADC_min);

    return percentage;

}


// Detects a if a button is pushed and returns a bool
bool button_event(void)
{
    updateButtons ();
    if(checkButton (UP) == 0) {
        return true;
    } else {
        return false;
    }
}


int main(void) {

    uint16_t i;
    int32_t sum;
    // Calls initlisation function
    do_init();

    // Enable interrupts to the processor.
    IntMasterEnable();

    // in this order
    // ====add delay cos sabine says so====
    // set init alt value to meanVal

    while (1)
    {
        //
        // Background task: calculate the (approximate) mean of the values in the
        // circular buffer and display it, together with the sample number.
        sum = 0;
        for (i = 0; i < BUF_SIZE; i++)
            sum = sum + readCircBuf (&g_inBuffer);
        // Calculate and display the rounded mean of the buffer contents
        uint16_t adc_av = (2 * sum + BUF_SIZE) / 2 / BUF_SIZE;
        int16_t percentage = percentage_calc(adc_av);


//        displayMeanVal (adc_av, g_ulSampCnt);
        if(button_event() == true) {
            display_change();
        }


        switch(displaystate)
        {

            case(percentageState):
                displayPercentage(percentage);
                break;

            case(meanState):
                displayMeanVal(adc_av, g_ulSampCnt);
                break;

            case(blankState):
                displayBlank();
                break;

        }
//


    }

}
