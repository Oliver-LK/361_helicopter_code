// ***********************************************************
// AUTHOR        : Ben Stirling and Oliver Clements 
// CREATE DATE   : 17/3/2023
// PURPOSE       : The main module with int main() loop
//                 
// ***********************************************************

// Libraries
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

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
#include "altitude.h"
#include "State.h"
#include "UART.h"
#include "PWM.h"
#define MAX_STR_LEN 16


#define sys_delay 3000000

uint8_t slow_tick = 0;

char statusStr[MAX_STR_LEN + 1];
void initClock (void)
{
    // Set the clock rate to 20 MHz
    SysCtlClockSet (SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);
    //
    // Set up the period for the SysTick timer.  The SysTick timer period is
    // set as a function of the system clock.
    SysTickPeriodSet(SysCtlClockGet() / SAMPLE_RATE_HZ);
    //
    // Register the interrupt handler
    SysTickIntRegister(SysTickIntHandler);
    //
    // Enable interrupt and device
    SysTickIntEnable();
    SysTickEnable();
}


// *******************************************************

void init_abs_yaw_ISR(void) {

    //Interrupt for setting reference position to 0.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, GPIO_PIN_4);

    GPIOIntTypeSet(GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_FALLING_EDGE);

    GPIOIntRegister(GPIO_PORTC_BASE, abs_yaw_ISR);

    GPIOIntEnable(GPIO_PORTC_BASE,GPIO_INT_PIN_4);

}


void init_yaw_ISR(void)
{
    //  Interrupt Pins initialisation for yaw monitoring.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    //    GPIODirModeSet(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1,
    //                   GPIO_DIR_MODE_IN);

    GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1,
                   GPIO_BOTH_EDGES);

    GPIOIntRegister(GPIO_PORTB_BASE, yaw_ISR);
    GPIOIntEnable(GPIO_PORTB_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1);
}

void do_init(void)
{
    //Initialisation function to tidy up the start of the program.
    initClock ();
    initADC ();
    initDisplay ();
    initCircBuf (&g_inBuffer, BUF_SIZE);
    initButtons ();
    init_yaw_ISR();
    init_abs_yaw_ISR();
    initialisePWM ();
    initialiseUSB_UART();




}

//#############################################  END OF INIT ########################################



int main(void) {

    static uint32_t message_time = 0;
    int32_t sum = 0; // Sum variable for reading circular buffer.
    uint16_t i;
    // Calls initialisation function
    do_init();

    // Enable interrupts to the processor.
    IntMasterEnable();
    PWMOutputState(PWM_MAIN_BASE, PWM_MAIN_OUTBIT, true);
    PWMOutputState(PWM_TAIL_BASE, PWM_TAIL_OUTBIT, true);



    SysCtlDelay (sys_delay); //Delays the system to allow the circular buffer to fill up.

    //This should probably be somewhere else.
    for (i = 0; i < BUF_SIZE; i++)
                sum = sum + readCircBuf (&g_inBuffer);
    uint16_t ADC_offset = give_adc_av(); //Sets ADC offset.


    while (1)
    {
        // Calculate and display the rounded mean of the buffer contents
        int32_t adc_av =  give_adc_av();
        int32_t alt_percentage = give_adc_percent(adc_av, ADC_offset);
        displayPos(alt_percentage, get_yaw(), yaw_decimal()); // Displays the helicopter's position.

        //The following code is test code... I'm very skeptical about it, but reading from uartDemo.c it seems like the was UART works.
        if (message_time >100000) {
            usprintf (statusStr, "yaw| \r\n"); // * usprintf
            UARTSend (statusStr);
            message_time = 0;
        }


        message_time++;

        //Redundant Code
/*        switch(displaystate)
        {
            case(percentageState):
                displayPercentage(percentage);
                break;

            case(meanState):
                displayMeanVal(displayPercentage(percentage), yaw_decimal());
                break;

            case(blankState):
                displayBlank();
                break;

        }*/
        //        displayMeanVal (adc_av, g_ulSampCnt);
        /*
                if(button_event(UP) == PUSHED) {
                    display_change();
                }
        */

        /*if(button_event(LEFT) == PUSHED) {
                    ADC_offset = adc_av; // Sets the new zero-point for the altitude.
                }*/

    }

}
