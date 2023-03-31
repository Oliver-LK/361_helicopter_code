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
#include "yaw.h"





// Global Constants
#define ADC_min 1241 // 1 volt in ADC counts when ADC is set to 3.3V
#define ADC_max 2482 // 2 volts in ADC counts when ADC is set to 3.3V

#define ADC_range (ADC_max - ADC_min)




int32_t percentage_calc(int32_t adc_av, int32_t ADC_offset)
{

    int32_t percentage = ((  ADC_offset - adc_av )*100/ADC_range);

    return percentage;

}


// Detects a if a button is pushed and returns a bool
bool button_event(uint8_t butNum)
{
    updateButtons ();
    if(checkButton (butNum) == 0) {
        return true;
    } else {
        return false;
    }
}


int main(void) {

    uint16_t i;
    int32_t sum = 0;

    // Calls initlisation function
    do_init();

    // Enable interrupts to the processor.
    IntMasterEnable();

    // in this order

    // set init alt value to meanVal

    SysCtlDelay (3000000);

    for (i = 0; i < BUF_SIZE; i++)
                sum = sum + readCircBuf (&g_inBuffer);
    uint16_t ADC_offset = (2 * sum + BUF_SIZE) / 2 / BUF_SIZE;


    while (1)
    {
        //
        // Background task: calculate the (approximate) mean of the values in the
        // circular buffer and display it, together with the sample number.
        sum = 0;
        for (i = 0; i < BUF_SIZE; i++)
            sum = sum + readCircBuf (&g_inBuffer);

        // Calculate and display the rounded mean of the buffer contents
        int32_t adc_av =  (2 * sum + BUF_SIZE) / 2 / BUF_SIZE ;
        int32_t percentage = percentage_calc(adc_av, ADC_offset);


//        displayMeanVal (adc_av, g_ulSampCnt);
        if(button_event(UP) == PUSHED) {
            display_change();
        }

        if(button_event(LEFT) == PUSHED) {
            ADC_offset = adc_av;
        }



        switch(displaystate)
        {
            case(percentageState):
                displayPercentage(percentage);
                break;

            case(meanState):
                displayMeanVal(adc_av, get_yaw());
                break;

            case(blankState):
                displayBlank();
                break;

        }



//


    }

}
