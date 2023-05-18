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


//Global constant declarations
#define SYSTEM_FREQ_COUNTER 100                     // Systick and other systems are updated at 100 Hz, and this serves as a counter for the timing based scheduler. 
#define MAX_STR_LEN 16                              // Maximum string length which can be transmitted in one serial command. 
#define DISPLAY_FREQ (SYSTEM_FREQ_COUNTER / 4)      //The display updates at 4 Hz. 
#define BUTTON_FREQ  (SYSTEM_FREQ_COUNTER / 100)    //The buttons are polled at 100 Hz
#define TX_FREQ (SYSTEM_FREQ_COUNTER / 4)           //Serial commands are sent at 4 Hz. 
#define YAW_CALIBRATION_DUTY 60                     // Duty cycle for yaw while the helicopter is calibrating and trying to find the yaw reference point. 
#define ALT_CALIBRATION_DUTY 35                     // Duty cycle for yaw while the helicopter is calibrating and trying to find the yaw reference point.
#define SYS_DELAY 3000000
//char statusStr[MAX_STR_LEN + 1];



//Global variable declarations
static uint16_t display_counter;                    //Counter to regulate the display updating.    
static uint16_t button_counter;                     //Counter to regulate the button and switch polling.
static uint16_t tx_counter;                         //Counter to regulate the serial transmission speed. 



//Prototype function declarations
void SysTickIntHandler(void);
//Generates an interrupt at SAMPLE_RATE_HZ frequency which tells the ADC to sample the altitude of the helicopter.



//Struct and enum declarations
typedef enum {

    LANDED = 0,
    TAKEOFF,
    FLYING,
    LANDING

} heli_state_t; //Represents the state of the helicopter for the finite state machine. 



//Function definitions
void initClock (void)
{
    // Initialises the internal clock and Systick interrupt. 
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

void SysTickIntHandler(void)
{
    // The interrupt handler for the for SysTick interrupt, which triggers at SAMPLE_RATE_HZ.
    // Systick is used to trigger a sample to the ADC and is also used to increment the counters for the timer. 

    ADCProcessorTrigger(ADC0_BASE, 3); //Triggers interrupt for the ADC to sample.

    //Incrementing counters. 
    display_counter++;
    button_counter++;
    tx_counter++;

}

void do_init(void)
{
    //Initialisation function to tidy up the start of the program. Runs and initialises all functions, GPIO pins, and peripherals. 
    initClock();
    initADC();
    initDisplay ();
    initCircBuf (&g_inBuffer, BUF_SIZE);

    initialiseUSB_UART();
    initialisePWM();
    initButtons();
    init_yaw_ISR();
    init_abs_yaw_ISR();

    //Enables PWM outputs, as they are disabled initialisePWM (). 
    PWMOutputState(PWM_MAIN_BASE, PWM_MAIN_OUTBIT, true);
    PWMOutputState(PWM_TAIL_BASE, PWM_TAIL_OUTBIT, true);
}

//#############################################  END OF INITIALISATION ########################################





int main(void) {

    //Performs initialisation. 
    do_init();


    //Variable initialisation
    uint8_t alt_duty = 0;           //Duty cycle for main rotor. 
    uint8_t yaw_duty = 0;           //Duty cycle for tail rotor.
    bool motor_off = 1;             //States the motors are off. 
    int32_t adc_av = 0;             //Defines the average value received from the ADC. 
    int32_t alt_percentage = 0;     //The altitude, measured in percent. 

    //Initialise counters for timing-based scheduler. 
    display_counter = 0;
    button_counter = 0;
    tx_counter = 0;


    //Initial helicopter state 
    heli_state_t heli_state = LANDED;


    // Enable interrupts to the processor.
    IntMasterEnable();


    //Delays the system to allow the circular buffer to fill up.
    SysCtlDelay (SYS_DELAY); 
    
    //ADC_offset 
    uint16_t ADC_offset = give_adc_av(); 


    pos_t desired_pos = {ADC_offset, 0 };
    initialise_adc_offset(ADC_offset);





    while (1)
    {
        // Calculate and display the rounded mean of the buffer contents


        switch(heli_state) {

            case LANDED:

                //In the landed state, the helicopter sets its desired position to the reference yaw, and minimum altitude
                //It then switches its motors off.
                reset_yaw_accum();
                //Change state.
                if (checkButton(SWITCH) == 1 && motor_off == 1) {
                    SysCtlDelay (SYS_DELAY); //Delay to prevent instantaneous state change.
                    heli_state = TAKEOFF;
                }
                break;

            case TAKEOFF:

                motor_off = 0; // Motors are now on and will need to be turned off to land.


                if(get_calibration() == 0) {
                    desired_pos.alt = ADC_offset -20 ; //Sets the hover height.
                    setPWM_main(ALT_CALIBRATION_DUTY);
                    setPWM_tail(YAW_CALIBRATION_DUTY);

                }

                if(get_calibration() == 1) {
                    //Sets calibrated
                     IntDisable(INT_GPIOC_TM4C123);
                     heli_state = FLYING;
                     reset_yaw_incr(); //Resets the yaw increment to 0 for a new flight.1

                 }
                break;

            case FLYING:

                set_desired_pos(&desired_pos);
                alt_duty = alt_controller(desired_pos.alt, adc_av);
                yaw_duty = yaw_controller(desired_pos.yaw, get_yaw_counter());
                setPWM_main(alt_duty);
                setPWM_tail(yaw_duty);
                if (checkButton(SWITCH) == 0) {
                      heli_state = LANDING;
                }


                break;

            case LANDING:


            desired_pos.yaw = 0; //Causes the helicopter's PID controller to rotate to the reference yaw.
            desired_pos.alt = ADC_offset - 500;
            if(get_yaw_counter() == 0 ) {
                desired_pos.alt = ADC_offset - 50; // Causes the PID controller to aim for the minimum altitude after desired yaw has been reached.
            }

            //Sets duty cycle of main and tail rotors to enable the heli to reach desired positions.

            alt_duty = alt_controller(desired_pos.alt, adc_av);
            yaw_duty = yaw_controller(desired_pos.yaw, get_yaw_counter());
            setPWM_main(alt_duty);
            setPWM_tail(yaw_duty);


            //THE FOLLOWING CODE BREAKS IF THE SPEED IS TOO HIGH. DERIVATIVE CONTROL MAY BE NEEDED.
//**********************************************************************************************************************
            if((get_yaw_counter() == 0) && (adc_av > ADC_offset -70) ) {
                //If the helicopter has made it to the reference yaw and minimum altitude, switch the motors off.

                setPWM_main(0);
                setPWM_tail(0);
                motor_off = 1;
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
//        //            UARTSend (statusStr);
//        //            usprintf (statusStr, "Alt Error: %4i \r\n",adc_av - desired_pos.alt); // * usprintf
//
////            usprintf (statusStr, "============ \r\n"); // * usprintf
////
////            UARTSend (statusStr);
////            usprintf (statusStr, "Yaw I error: %4i \r\n", return_iyaw_error()); // * usprintf
////
////            UARTSend (statusStr);
//            usprintf (statusStr, "Yaw Error: %4i \r\n",desired_pos.yaw - get_yaw_counter()); // * usprintf
//            UARTSend (statusStr);
////
//////            usprintf (statusStr, "PID Yaw Error: %4i \r\n", return_yaw_error()); // * usprintf
//////                                    UARTSend (statusStr);
//////                        UARTSend (statusStr);
////
//            usprintf (statusStr, "Alt Error: %4i \r\n",adc_av - desired_pos.alt); // * usprintf
//            UARTSend (statusStr);
////
////            usprintf (statusStr, "Alt I error: %4i \r\n", return_ialt_error());
////            UARTSend (statusStr);
//
            usprintf (statusStr, "State: %4i \r\n", heli_state);
                        UARTSend (statusStr);
//
////            usprintf (statusStr, "D_yaw: %4i \r\n", return_prepre_yaw_error());
////            UARTSend (statusStr);
            tx_counter = 0;
        }

    }

}



