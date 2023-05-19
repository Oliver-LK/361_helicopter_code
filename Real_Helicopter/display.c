// ***********************************************************
// AUTHOR        : Ben Stirling and Oliver Clements, based off code by Phil Bones 
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
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "utils/ustdlib.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "display.h"



void initDisplay (void)
{
    // intialise the Orbit OLED display
    OLEDInitialise ();
}


// Displays the helicopter positional information on the tiva display
void displayPos(int32_t alt_percentage, int16_t yaw_int, int8_t yaw_decimal, int32_t alt_PWM, int32_t yaw_PWM)
{
    char string[17];  // 16 characters across the display

    // Form a new string for the line.  The maximum width specified for the
    //  number field ensures it is displayed right justified.
    usnprintf (string, sizeof(string), "Alt(%%) %7d", alt_percentage);
    // Update line on display.
    OLEDStringDraw (string, 0, 0);

    usnprintf (string, sizeof(string), "Alt PWM (%%) %2d", alt_PWM);
    OLEDStringDraw (string, 0, 1); //Places the line on the 3rd line of the module.

    usnprintf (string, sizeof(string), "Yaw(deg) %5d.%d", yaw_int, yaw_decimal);
    //The yaw angle is split into an integer component and a decimal component which are concatenated to display a float-like number.
    OLEDStringDraw (string, 0, 2); //Places the line on the 3rd line of the module.

    usnprintf (string, sizeof(string), "Yaw PWM (%%) %2d", yaw_PWM);
    OLEDStringDraw (string, 0, 3); //Places the line on the 3rd line of the module.
}
