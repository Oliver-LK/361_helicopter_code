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
#include "PWM.h"
#include "UART.h"
#include "PID_Control.h"

#define MAX_STR_LEN 16


//For real helicopter, ALT_MIN 2358  ALT_MAX 1365




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
    SysCtlPWMClockSet(PWM_DIVIDER_CODE);
}


// *******************************************************




void do_init(void)
{
    //Initialisation function to tidy up the start of the program.
    initClock ();
    initADC ();
    initDisplay ();
    initCircBuf (&g_inBuffer, BUF_SIZE);

    initialiseUSB_UART();
    initialisePWM ();
    initButtons ();

    //  Interrupt Pins initialisation for yaw monitoring.

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
//    GPIODirModeSet(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1,
//                   GPIO_DIR_MODE_IN);

    GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1,
                   GPIO_BOTH_EDGES);

    GPIOIntRegister(GPIO_PORTB_BASE, yaw_ISR);

    GPIOIntEnable(GPIO_PORTB_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1);

    PWMOutputState(PWM_MAIN_BASE, PWM_MAIN_OUTBIT, true);
    PWMOutputState(PWM_TAIL_BASE, PWM_TAIL_OUTBIT, true);
}

//#############################################  END OF INIT ########################################



int main(void) {

    int32_t sum = 0; // Sum variable for reading circular buffer.
    //uint8_t test_duty = 100;
    uint16_t i;
    // Calls initialisation function
    do_init();
    uint8_t slowTick = 0;

    uint8_t alt_duty = 0;
    uint8_t yaw_duty = 0;

    // Enable interrupts to the processor.
    IntMasterEnable();



    gains_t K_yaw = {5,0,0, 10};
    gains_t K_alt = {8,0,0, -10};
    pos_t desired_pos = {(ALT_MIN + ALT_MAX)/ 2, 0 };

    SysCtlDelay (3000000); //Delays the system to allow the circular buffer to fill up.

    for (i = 0; i < BUF_SIZE; i++)
                sum = sum + readCircBuf (&g_inBuffer);
    uint16_t ADC_offset = give_adc_av(); //Sets ADC offset.


    while (1)
    {
        // Calculate and display the rounded mean of the buffer contents
        int32_t adc_av =  give_adc_av();
        int32_t alt_percentage = give_adc_percent(adc_av, ADC_offset);

        set_desired_pos(&desired_pos);

        alt_duty = controller(desired_pos.alt, adc_av, K_alt);
        yaw_duty = controller(desired_pos.yaw, get_yaw_counter(), K_yaw);
        setPWM_main(alt_duty);
        setPWM_tail(yaw_duty);


        displayPos(alt_percentage, get_yaw(), yaw_decimal()); // Displays the helicopter's position.


        if (slowTick > 40)
        {
            slowTick = 0;
            // Form and send a status message to the console

//            usprintf (statusStr, "Yaw Duty: %4i \r\n",yaw_duty); // * usprintf
//            UARTSend (statusStr);
//            usprintf (statusStr, "Alt Duty: %4i \r\n",alt_duty ); // * usprintf
//            UARTSend (statusStr);
            usprintf (statusStr, "Alt Error: %4i \r\n",adc_av - desired_pos.alt); // * usprintf
            UARTSend (statusStr);
            usprintf (statusStr, "Yaw Error: %4i \r\n",get_yaw_counter() - desired_pos.yaw); // * usprintf
                        UARTSend (statusStr);
        }
        slowTick++;

        updateButtons();


        //        if(checkButton(LEFT) == PUSHED) {
        //            ADC_offset = adc_av; // Sets the new zero-point for the altitude.
        //        }

    }

}
