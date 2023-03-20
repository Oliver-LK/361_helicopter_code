// ***********************************************************
// AUTHOR        : Ben Stirling and Oliver Clements 
// CREATE DATE   : 17/3/2023
// PURPOSE       : Initialisation Module
//                 
// ***********************************************************

#ifndef INIT_H
#define INIT_H

// Libaries
#include <stdint.h>
#include <stdbool.h>
#include "stdlib.h"

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
#include "buttons4.h"

// Function Decelerations
void initClock (void);

void initDisplay(void);

void initADC (void);

uint32_t* initCircBuf (circBuf_t *buffer, uint32_t size);

void do_init(void);

#endif
