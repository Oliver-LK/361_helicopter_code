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
                              // Maximum string length which can be transmitted in one serial command.
#define DISPLAY_FREQ (SYSTEM_FREQ_COUNTER / 4)      //The display updates at 4 Hz. 
#define BUTTON_FREQ  (SYSTEM_FREQ_COUNTER / 100)    //The buttons are polled at 100 Hz
#define TX_FREQ (SYSTEM_FREQ_COUNTER / 4)           //Serial commands are sent at 4 Hz. 
#define YAW_CALIBRATION_DUTY 60                     // Duty cycle for yaw while the helicopter is calibrating and trying to find the yaw reference point. 
#define ALT_CALIBRATION_DUTY 30                   // Duty cycle for yaw while the helicopter is calibrating and trying to find the yaw reference point.
#define SYS_DELAY 3000000
#define HOVER_HEIGHT 30




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
//    tx_counter++;

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
    bool reset = 0;

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
    
    //ADC_offset represents the ground value, which is the initial value measured by the ADC. 
    uint16_t ADC_offset = give_adc_av(); 
    initialise_adc_offset(ADC_offset); //Sends ADC offset to the PID module. 

    //Sets the initial desired position for the controller to be 1% altitude and the initial yaw to be 0. 
    pos_t desired_pos = {ADC_offset - HOVER_HEIGHT, 0 };
    

    while (1)
    {

        //Finite state machine. 
        switch(heli_state) {

            case LANDED:

                //In the LANDED state, the helicopter waits for the switch to be set high, indicating the helicopter can take off. 
                //It also resets any accumulated error which may have wound up in the yaw controller's integral control. 

                reset_yaw_accum();
                
                if (checkButton(L_SWITCH) == 1 && motor_off == 1) {
                    //Change state if the motors are off and the switch is set to high. 
                    SysCtlDelay (SYS_DELAY); //Delay to prevent instantaneous state change.
                    heli_state = TAKEOFF;
                }
                break;

            case TAKEOFF:

                //In the TAKEOFF state, the helicopter either calibrates the yaw if this has not been done, or skips this step if taking off for the second time in a session. 

                motor_off = 0; // Motors are now on and will need to be turned off to land.

                if(is_calibrated() == 0) {

                    //Sets the height and yaw for the calibration state. 
                    setPWM_main(ALT_CALIBRATION_DUTY);      
                    setPWM_tail(YAW_CALIBRATION_DUTY);

                } else {
                    //Disables the reference yaw interrupt and changes state. 
                     IntDisable(INT_GPIOC_TM4C123);
                     heli_state = FLYING;
                     reset_yaw_incr(); //Resets the desired yaw counter to 0 for a new flight.

                 }
                break;

            case FLYING:

                //In the FLYING state, the helicopter responds to button commands and uses the controllers for the yaw and altitude to attain the desired positions. 

                set_desired_pos(&desired_pos);
                alt_duty = alt_controller(desired_pos.alt, adc_av);
                yaw_duty = yaw_controller(desired_pos.yaw, get_yaw_counter());
                setPWM_main(alt_duty);
                setPWM_tail(yaw_duty);

                //If landing switch is set to low, change state. 
                if (checkButton(L_SWITCH) == 0) {
                      heli_state = LANDING;
                }
                break;

            case LANDING:
            //In the LANDING state, the helicopter rotates to the reference yaw and 0 altitude, before switching the motors off when it hits the ground. 

            desired_pos.yaw = 0; 
            desired_pos.alt = ADC_offset - 5 * HOVER_HEIGHT; //Waits above the ground for the yaw to reach 0.

            if(get_yaw_counter() == 0 ) {
                desired_pos.alt = ADC_offset - 2 * HOVER_HEIGHT;
            }

            //Sets duty cycle of main and tail rotors to enable the heli to reach desired positions.
            alt_duty = alt_controller(desired_pos.alt, adc_av);
            yaw_duty = yaw_controller(desired_pos.yaw, get_yaw_counter());
            setPWM_main(alt_duty);
            setPWM_tail(yaw_duty);


            if((get_yaw_counter() == 0) && (adc_av > ADC_offset - 4 * HOVER_HEIGHT) ) {
                //If the helicopter has made it to the reference yaw and minimum altitude, switch the motors off and change state.
                setPWM_main(0);
                setPWM_tail(0);
                motor_off = 1;
                heli_state = LANDED;
            }
            break;
        }



        //Calculates the altitude using the values stored in the circular buffer.
        adc_av =  give_adc_av();
        alt_percentage = give_adc_percent(adc_av, ADC_offset);


        //Timers for low-priority tasks. 

        if (button_counter > (BUTTON_FREQ)) {
            //Poll buttons
            updateButtons();
            button_counter = 0;
        }


        if (display_counter > (DISPLAY_FREQ)) {
            // Displays the helicopter's position.
            if (heli_state != TAKEOFF) {
                displayPos(alt_percentage, get_yaw(), yaw_decimal(), get_alt_PWM(), get_yaw_PWM());

            } else {
                displayPos(alt_percentage, get_yaw(), yaw_decimal(), ALT_CALIBRATION_DUTY, YAW_CALIBRATION_DUTY);

            }

            display_counter = 0;

        }


        if(GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_6) >= 1 && reset == 0) {
            SysCtlReset();
            reset = 1;
        }

//        if (tx_counter > (TX_FREQ)) {
//        // Form and send a status message to the console
//            UART_transmit(heli_state, get_alt_PWM(), get_yaw_PWM(), alt_percentage, get_yaw());
//            tx_counter = 0;
//        }

    }

}



