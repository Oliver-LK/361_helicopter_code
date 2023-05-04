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
#define TICKS_PER_ROTATION 448
#define DEG_PER_ROTATION 360
#define PRECISION 100
#define MAX_ROTATION_THRESHOLD 448/2
#define MIN_ROTATION_THRESHOLD (-448/2 + 1)


static uint8_t prev_yaw_state = 0; //static variable to represent the previous state of the encoder
static int16_t yaw_counter = 0; //


void yaw_ISR(void)
{
    uint8_t state = GPIOPinRead(GPIO_PORTB_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1); //Reads the state of the two encoders.

    switch(state)
    //Switch statement to decide whether the yaw angle is increasing. For clockwise rotation, the encoder states go:
    // {00,01,11,10}, and for anticlockwise rotation, the states go {00,10,11,01}
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
                }

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

    GPIOIntClear(GPIO_PORTB_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1); //Clears the interrupt so the uC can return to its regularly scheduled program.

}




int32_t get_yaw(void)
{
    //Calculates the yaw of the
    if(yaw_counter > MAX_ROTATION_THRESHOLD) {

        yaw_counter -= TICKS_PER_ROTATION;
    } else if (yaw_counter < MIN_ROTATION_THRESHOLD){
        yaw_counter += TICKS_PER_ROTATION;
    }
    int16_t rotation = ((yaw_counter * DEG_PER_ROTATION)/TICKS_PER_ROTATION);
    return rotation;
}

uint8_t yaw_decimal(void)
{

    uint8_t decimal = 0;
    decimal = ((yaw_counter * DEG_PER_ROTATION * PRECISION) / TICKS_PER_ROTATION) % PRECISION;
    if (decimal >= 10) {
        decimal /= 10;
    }
    return decimal;
}
