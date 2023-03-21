// ***********************************************************
// AUTHOR        : Ben Stirling and Oliver Clements 
// CREATE DATE   : 17/3/2023
// PURPOSE       : Modules that controls the LED display on
//                 Tiva board
//                 
// ***********************************************************

#ifndef DISPLAY_H
#define DISPLAY_H
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

typedef enum {  percentageState = 0,
                meanState = 1,
                blankState = 2
} displayState_t;

displayState_t displaystate;

// Function Declarations
void displayMeanVal(uint16_t meanVal, uint32_t count);

void displayPercentage(int16_t percentage);

void displayBlank(void);

void display_change(void);

#endif
