// ***********************************************************
// AUTHOR        : Ben Stirling and Oliver Clements 
// CREATE DATE   : 17/3/2023
// PURPOSE       : Modules that controls the LED display on
//                 Tiva board
//                 
// ***********************************************************

// Libraries
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
#include "display.h"
#include "ADC.h"


void initDisplay (void)
{
    // intialise the Orbit OLED display
    OLEDInitialise ();
}


// Displays the helicopter positional information on the tiva dispay
void displayPos(int32_t alt_percentage, int16_t yaw_int, int8_t yaw_decimal)
{
    char string[17];  // 16 characters across the display

    OLEDStringDraw ("Heli Position", 0, 0);

    // Form a new string for the line.  The maximum width specified for the
    //  number field ensures it is displayed right justified.
    usnprintf (string, sizeof(string), "Alt(%%) %8d", alt_percentage);
    // Update line on display.
    OLEDStringDraw (string, 0, 1);

    usnprintf (string, sizeof(string), "Yaw(deg) %5d.%d", yaw_int, yaw_decimal);


    OLEDStringDraw (string, 0, 3); //Places the line on the 3rd line of the module.

}



//Redundant code
// displays the percentage
/*void displayPercentage(int16_t percentage)
{
    char string[17];  // 16 characters across the display

    OLEDStringDraw ("Altitude  (%)", 0, 0);

    // Form a new string for the line.  The maximum width specified for the
    //  number field ensures it is displayed right justified.
    usnprintf (string, sizeof(string), "Percent+ = %4d", percentage);
    // Update line on display.
    OLEDStringDraw (string, 0, 1);


}


//displays a blank screen
void displayBlank(void)
{
//    char string[17];  // 16 characters across the display

    OLEDStringDraw ("                ", 0, 0);
    OLEDStringDraw ("                ", 0, 1);
    OLEDStringDraw ("                ", 0, 3);

}


// changes the enum of the display amd updates it
void display_change(void)
{
    displayBlank();

     if(displaystate == blankState) {
         displaystate = percentageState;
     } else {
         displaystate++;
     }




}*/
