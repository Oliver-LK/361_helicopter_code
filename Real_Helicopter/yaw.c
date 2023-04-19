


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


static uint8_t prevoius_val = 0;
static int16_t rotation_times = 448/2;


void yaw_ISR(void)
{
    uint8_t encoder_0  = GPIOPinRead(GPIO_PORTB_BASE, GPIO_INT_PIN_0);
    uint8_t encoder_1 = GPIOPinRead(GPIO_PORTB_BASE, GPIO_INT_PIN_1);
    uint8_t state = encoder_0 + encoder_1;


    switch(state)
    {
        case 0:
            if(prevoius_val == 2) {
                rotation_times++;
            } else {
                rotation_times--;
            }

             break;
        case 1:
                if(prevoius_val == 0) {
                    rotation_times++;
                }
                else {
                    rotation_times--;
                }

                 break;
        case 3:
                if(prevoius_val == 1) {
                    rotation_times++;
                }
                else {
                    rotation_times--;
                }

                 break;
        case 2:
                if(prevoius_val == 3) {
                    rotation_times++;
                }
                else {
                    rotation_times--;
                }

                 break;


    }

    prevoius_val = state;

    GPIOIntClear(GPIO_PORTB_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1);

}




int32_t get_yaw(void)
{
    if(rotation_times > 448) {

        rotation_times -= 448;
    } else if (rotation_times < 0 ){
        rotation_times += 448;
    }
    int16_t rotation = ((rotation_times)*360/448) - 180;
    return rotation;
}

uint8_t yaw_decimal(void)
{

    uint8_t decimal = 0;
    decimal = (rotation_times * 36000 / 448) % 100;
    if (decimal >= 10) {
        decimal /= 10;
    }
    return decimal;
}
