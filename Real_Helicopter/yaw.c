// ***********************************************************
// AUTHOR        : Ben Stirling and Oliver Clements
// CREATE DATE   : 04/05/2023
// PURPOSE       : Yaw Module
//
// ***********************************************************


// Libraries
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
#include "ADC.h"
#include "buttons4.h"
#include "yaw.h"
#define TICKS_PER_REV 448
#define DEG_PER_REV 360
#define PRECISION 100
#define MAX_REV_THRESHOLD 448/2
#define MIN_REV_THRESHOLD (-448/2 + 1)

// Global variables
static uint8_t prev_yaw_state = 0; // static variable to represent the previous state of the encoder
static int16_t yaw_counter = 0; //


void yaw_ISR(void)
{
    state = GPIOPinRead(GPIO_PORTB_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1); // Reads the state of the two encoders.

    switch(state)
    // Switch statement to decide whether the yaw angle is increasing. For clockwise yaw, the encoder states go:
    // {00,01,11,10}, and for anticlockwise yaw, the states go {00,10,11,01}
    {
        case 0b00:
            if(prev_yaw_state == 0b10) {
                yaw_counter++;
            } else {
                yaw_counter--;
            }

             break;

        case 0b01:
                if(prev_yaw_state == 0b00) {
                    yaw_counter++;
                } else {
                    yaw_counter--;
                }
                
                break;

        case 0b11:
                if(prev_yaw_state == 0b01) {
                    yaw_counter++;
                } else {
                    yaw_counter--;
                }

                break;

        case 0b10:
                if(prev_yaw_state == 0b11) {
                    yaw_counter++;
                } else {
                    yaw_counter--;
                }

                break;
    }

    prev_yaw_state = state;
    if(yaw_counter > MAX_REV_THRESHOLD) {

        yaw_counter -= TICKS_PER_REV;
    } else if (yaw_counter < MIN_REV_THRESHOLD){
        yaw_counter += TICKS_PER_REV;
    }

    GPIOIntClear(GPIO_PORTB_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1); // Clears the interrupt so the micro can return to its regularly scheduled program.
}


int32_t get_yaw(void)
{
    // Calculates the yaw of the helicopter and converts it into degrees. Returns the integer component of the yaw.

    int16_t yaw = ((yaw_counter * DEG_PER_REV)/TICKS_PER_REV);
    return yaw;
}

int8_t yaw_decimal(void)
{
    // Finds the decimal component of the yaw.
    int8_t decimal = 0;
    decimal = ((yaw_counter * DEG_PER_REV * PRECISION) / TICKS_PER_REV) % PRECISION;
    if (decimal >= 10) {
        decimal /= 10;
    }
    return abs(decimal);
}


int32_t get_yaw_counter(void) {
    // returns the raw yaw count
    return yaw_counter;
}

void init_abs_yaw_ISR(void) {

    // Interrupt for setting reference position to 0.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, GPIO_PIN_4);

    GPIOIntTypeSet(GPIO_PORTC_BASE, GPIO_PIN_0, GPIO_FALLING_EDGE);

    GPIOIntRegister(GPIO_PORTC_BASE, abs_yaw_ISR);

    GPIOIntEnable(GPIO_PORTC_BASE,GPIO_INT_PIN_4);

}

void init_yaw_ISR(void)
{
    //  Interrupt Pins initialisation for yaw monitoring.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1,
                   GPIO_BOTH_EDGES);

    GPIOIntRegister(GPIO_PORTB_BASE, yaw_ISR);
    
    GPIOIntEnable(GPIO_PORTB_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1);
}

void abs_yaw_ISR(void) 
{
    // Interrupt that triggers when the 0 point is found
    // Sets a flag so this can not be called again
    yaw_counter = 0;
    GPIOIntClear(GPIO_PORTC_BASE, GPIO_INT_PIN_4);
    calibration_complete = 1;

}

bool is_calibrated(void)
{
    // returns true if calibration sequence is complete
    return calibration_complete;
}

int8_t return_state(void) 
{
    // returns the state of the helicopter
    return state;
}
