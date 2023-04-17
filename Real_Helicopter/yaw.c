


// Libaries
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
static int16_t rotation_times = 0;

static uint8_t encoder_0 = 0;
static uint8_t encoder_1 = 0;

void yaw_ISR(void)
{
    encoder_0 = GPIOPinRead(GPIO_PORTB_BASE, GPIO_INT_PIN_0);
    encoder_1 = GPIOPinRead(GPIO_PORTB_BASE, GPIO_INT_PIN_1);

    if(prevoius_val == 3 && encoder_0 == 0) {
        rotation_times++;
    } else if (prevoius_val == 0 && encoder_1 == 2) {
        rotation_times--;
    }

    prevoius_val = encoder_1 + encoder_0;

    GPIOIntClear(GPIO_PORTB_BASE, GPIO_INT_PIN_0);
    GPIOIntClear(GPIO_PORTB_BASE, GPIO_INT_PIN_1);

}


int32_t get_yaw(void)
{
    return rotation_times;
}
