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


//For real helicopter, ALT_MIN 2358  ALT_MAX 1365


#define LOOP_MAX 100
#define MAX_STR_LEN 16
#define DISPLAY_FREQ (SAMPLE_RATE_HZ / 4)
#define BUTTON_FREQ  (SAMPLE_RATE_HZ / 100)
#define TX_FREQ (SAMPLE_RATE_HZ)
#define YAW_CALIBRATION_DUTY 15


char statusStr[MAX_STR_LEN + 1];

static uint16_t display_counter;
static uint16_t button_counter;
static uint16_t tx_counter;


void SysTickIntHandler(void);
//Generates an interrupt at SAMPLE_RATE_HZ frequency which tells the ADC to sample the altitude of the helicopter.

typedef enum {

    LANDED = 0,
    TAKEOFF,
    FLYING

} heli_state_t;



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
    init_yaw_ISR();
    init_abs_yaw_ISR();

    PWMOutputState(PWM_MAIN_BASE, PWM_MAIN_OUTBIT, true);
    PWMOutputState(PWM_TAIL_BASE, PWM_TAIL_OUTBIT, true);
}

//#############################################  END OF INIT ########################################

// The interrupt handler for the for SysTick interrupt, which triggers at SAMPLE_RATE_HZ.
void SysTickIntHandler(void)
{
    // Initiate a conversion
    ADCProcessorTrigger(ADC0_BASE, 3); //Triggers interrupt for the ADC to sample.
    display_counter++;
    button_counter++;
    tx_counter++;


}

int main(void) {

    // Calls initialisation function
    do_init();

    int32_t sum = 0; // Sum variable for reading circular buffer.
    //uint8_t test_duty = 100;
    uint16_t i;

    uint8_t alt_duty = 0;
    uint8_t yaw_duty = 0;

    display_counter = 0;
    button_counter = 0;
    tx_counter = 0;
    heli_state_t heli_state = TAKEOFF;

    bool motor_off = 1;
    // Enable interrupts to the processor.
    IntMasterEnable();


    pos_t desired_pos = {(ALT_MIN + ALT_MAX)/ 2, 0 };

    SysCtlDelay (3000000); //Delays the system to allow the circular buffer to fill up.

    for (i = 0; i < BUF_SIZE; i++)
        sum = sum + readCircBuf (&g_inBuffer);

    uint16_t ADC_offset = give_adc_av(); //Sets ADC offset.
    int32_t adc_av = 0;
    int32_t alt_percentage = 0;



    while (1)
    {
        // Calculate and display the rounded mean of the buffer contents


        switch(heli_state) {

            case LANDED:

                desired_pos.yaw = 0; //Causes the helicopter's PID controller to rotate to the reference yaw.
                desired_pos.alt = ADC_offset; // Causes the PID controller to aim for the minimum altitude.
                alt_duty = alt_controller(desired_pos.alt, adc_av);
                yaw_duty = yaw_controller(desired_pos.yaw, get_yaw_counter());
                setPWM_main(alt_duty);
                setPWM_tail(yaw_duty);

//                if((desired_pos.yaw == get_yaw_counter() ) && (desired_pos.alt == adc_av)) {
//
//                    setPWM_main(0);
//                    setPWM_tail(0);
//                    motor_off = 1;
//                }

                if (checkButton(SWITCH) == 1 && motor_off ==1) {
                    heli_state = TAKEOFF;
                }
                break;

            case TAKEOFF:

                motor_off = 0;
                desired_pos.alt = ADC_offset -20 ; //Sets the hover height.
                alt_duty = alt_controller(desired_pos.alt, adc_av);
                setPWM_main(alt_duty);
                setPWM_tail(YAW_CALIBRATION_DUTY);

                //The heli needs to yaw until the reference is found. This is yet to be implemented.

                if(get_calibration() == 1) {
                     IntDisable(INT_GPIOC_TM4C123);
                     heli_state = FLYING;

                 }
                break;
            case FLYING:

                set_desired_pos(&desired_pos);
                alt_duty = alt_controller(desired_pos.alt, adc_av);
                yaw_duty = yaw_controller(desired_pos.yaw, get_yaw_counter());
                setPWM_main(alt_duty);
                setPWM_tail(yaw_duty);
                if (checkButton(SWITCH)==0) {
                              heli_state = LANDED;
                }

                break;

        }

        adc_av =  give_adc_av();
        alt_percentage = give_adc_percent(adc_av, ADC_offset);





        if (button_counter > (BUTTON_FREQ)) {
            updateButtons();
            button_counter = 0;
        }


        if (display_counter > (DISPLAY_FREQ)) {
            displayPos(alt_percentage, get_yaw(), yaw_decimal()); // Displays the helicopter's position.
            display_counter = 0;

        }

        if (tx_counter > (TX_FREQ)) {


        // Form and send a status message to the console

        //            usprintf (statusStr, "Yaw Duty: %4i \r\n",yaw_duty); // * usprintf
        //            UARTSend (statusStr);
        //            usprintf (statusStr, "Alt Duty: %4i \r\n",alt_duty ); // * usprintf
        //            UARTSend (statusStr);
        //            usprintf (statusStr, "Alt Error: %4i \r\n",adc_av - desired_pos.alt); // * usprintf

            usprintf (statusStr, "============ \r\n"); // * usprintf

            UARTSend (statusStr);
            usprintf (statusStr, "Yaw I error: %4i \r\n", return_iyaw_error()); // * usprintf

            UARTSend (statusStr);
            usprintf (statusStr, "Yaw Error: %4i \r\n",desired_pos.yaw - get_yaw_counter()); // * usprintf

//            usprintf (statusStr, "PID Yaw Error: %4i \r\n", return_yaw_error()); // * usprintf
//                                    UARTSend (statusStr);
//                        UARTSend (statusStr);

            usprintf (statusStr, "Alt Error: %4i \r\n",adc_av - desired_pos.alt); // * usprintf
            UARTSend (statusStr);

            usprintf (statusStr, "Alt I error: %4i \r\n", return_ialt_error());
            UARTSend (statusStr);
            tx_counter = 0;
        }






        //        if(checkButton(LEFT) == PUSHED) {
        //            ADC_offset = adc_av; // Sets the new zero-point for the altitude.
        //        }

    }

}
