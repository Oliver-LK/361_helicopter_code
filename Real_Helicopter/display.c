// ***********************************************************
// AUTHOR        : Ben Stirling and Oliver Clements 
// CREATE DATE   : 17/3/2023
// PURPOSE       : Modules that controls the LED display on
//                 Tiva board
//                 
// ***********************************************************

// Libaries
#include <stdint.h>
#include <stdbool.h>

// Modules and Drivers
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



void displayMeanVal(uint16_t meanVal, uint32_t count)
{
    char string[17];  // 16 characters across the display

    OLEDStringDraw ("ADC demo 1", 0, 0);

    // Form a new string for the line.  The maximum width specified for the
    //  number field ensures it is displayed right justified.
    usnprintf (string, sizeof(string), "Mean ADC = %4d", meanVal);
    // Update line on display.
    OLEDStringDraw (string, 0, 1);

    usnprintf (string, sizeof(string), "Sample # %5d", count);
    OLEDStringDraw (string, 0, 3);
}

void displayPercentage(void)
{
    char string[17];  // 16 characters across the display

    OLEDStringDraw ("Hi World", 0, 0);

    // Form a new string for the line.  The maximum width specified for the
    //  number field ensures it is displayed right justified.
    usnprintf (string, sizeof(string), "Mean ADC = %4d", 1);
    // Update line on display.
    OLEDStringDraw (string, 0, 1);

    usnprintf (string, sizeof(string), "Sample # %5d", 5);
    OLEDStringDraw (string, 0, 3);
}


void displayBlank(void)
{
//    char string[17];  // 16 characters across the display

    OLEDStringDraw ("                ", 0, 0);
    OLEDStringDraw ("                ", 0, 1);
    OLEDStringDraw ("                ", 0, 3);

}


// Could be chnaged to interrupt
void display_change(void)
{
    updateButtons ();
//    uint8_t butState = checkButton (UP);

    switch (checkButton (UP))
    {
         case PUSHED:
             if(displaystate == blankState) {
                 displaystate = meanState;
             } else {
                 displaystate++;
             }
             displayBlank();
    }
}
