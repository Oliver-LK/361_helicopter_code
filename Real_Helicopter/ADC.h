// ***********************************************************
// AUTHOR        : Ben Stirling and Oliver Clements
// CREATE DATE   : 7/3/2023
// PURPOSE       : ADC handler 
//                 
// **********************************************************


#ifndef ADC_H
#define ADC_H
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


// Global Constants
#define BUF_SIZE 16
#define SAMPLE_RATE_HZ 100 //Sets the sample rate of the ADC to 100 Hz by triggering a sampling interrupt at 100Hz.

// Global Variables
circBuf_t g_inBuffer;       // Buffer of size BUF_SIZE integers (sample values)

//uint32_t g_ulSampCnt;       // Counter for the interrupts

// Function declarations
void SysTickIntHandler(void);
//Generates an interrupt at SAMPLE_RATE_HZ frequency which tells the ADC to sample the altitude of the helicopter.

void ADCIntHandler(void);
//Tells the ADC to take a sample when triggered by SysTickIntHandler.

void initADC (void);
//Function to initialise ADC's ports for the altitude measuring.

#endif
