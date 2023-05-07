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
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "display.h"
#include "ADC.h"
#include "buttons4.h"
#include "yaw.h"
#define TICKS_PER_REV 448
#define DEG_PER_REV 360
#define PRECISION 100
#define MAX_REV_THRESHOLD 448/2
#define MIN_REV_THRESHOLD (-448/2 + 1)


static uint8_t prev_yaw_state = 0; //static variable to represent the previous state of the encoder
static int32_t yaw_counter = 0; //



void yaw_ISR(void)
{
    uint8_t state = GPIOPinRead(GPIO_PORTB_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1); //Reads the state of the two encoders.

    switch(state)
    //Switch statement to decide whether the yaw angle is increasing. For clockwise yaw, the encoder states go:
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
                }
                else {
                    yaw_counter--;
                }

                 break;
        case 0b11:
                if(prev_yaw_state == 0b01) {
                    yaw_counter++;
                }
                else {
                    yaw_counter--;

                 break;
        case 0b10:
                if(prev_yaw_state == 0b11) {
                    yaw_counter++;
                }
                else {
                    yaw_counter--;
                }

                 break;


    }

    prev_yaw_state = state;

    GPIOIntClear(GPIO_PORTB_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1); //Clears the interrupt so the micro can return to its regularly scheduled program.    :)

}




int32_t get_yaw(void)
{
    //Calculates the yaw of the helicopter and converts it into degrees. Returns the integer component of the yaw.
    if(yaw_counter > MAX_REV_THRESHOLD) {

        yaw_counter -= TICKS_PER_REV;
    } else if (yaw_counter < MIN_REV_THRESHOLD){
        yaw_counter += TICKS_PER_REV;
    }
    int16_t yaw = ((yaw_counter * DEG_PER_REV)/TICKS_PER_REV);
    return yaw;
}

int8_t yaw_decimal(void)
{
    //Finds the decimal component of the yaw.
    int8_t decimal = 0;
    decimal = ((yaw_counter * DEG_PER_REV * PRECISION) / TICKS_PER_REV) % PRECISION + 5;
    decimal /= 10;
    return abs(decimal);
}


int32_t get_yaw_counter(void) {

    return yaw_counter;
}


