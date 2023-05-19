//********************************************************
//
// UART.c - derived from uartDemo.c by Prof. Phil Bones.
//
//
//
// Author:  P.J. Bones  UCECE
// Last modified:   16.4.2018
//

// Libraries
#include <stdint.h>
#include <stdbool.h>

// Modules and drivers
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/debug.h"
#include "driverlib/pin_map.h"
#include "utils/ustdlib.h"
#include "stdio.h"
#include "stdlib.h"
#include "UART.h"

//********************************************************
// Constants
//********************************************************


#define MAX_STR_LEN 16
//---USB Serial comms: UART0, Rx:PA0 , Tx:PA1
#define BAUD_RATE 9600
#define UART_USB_BASE           UART0_BASE
#define UART_USB_PERIPH_UART    SYSCTL_PERIPH_UART0
#define UART_USB_PERIPH_GPIO    SYSCTL_PERIPH_GPIOA
#define UART_USB_GPIO_BASE      GPIO_PORTA_BASE
#define UART_USB_GPIO_PIN_RX    GPIO_PIN_0
#define UART_USB_GPIO_PIN_TX    GPIO_PIN_1
#define UART_USB_GPIO_PINS      UART_USB_GPIO_PIN_RX | UART_USB_GPIO_PIN_TX


//********************************************************
// Global variables
//********************************************************


//********************************************************
// initialiseUSB_UART - 8 bits, 1 stop bit, no parity
//********************************************************
void
initialiseUSB_UART (void)
{
    //
    // Enable GPIO port A which is used for UART0 pins.
    //
    SysCtlPeripheralEnable(UART_USB_PERIPH_UART);
    SysCtlPeripheralEnable(UART_USB_PERIPH_GPIO);
    //
    // Select the alternate (UART) function for these pins.
    //
    GPIOPinTypeUART(UART_USB_GPIO_BASE, UART_USB_GPIO_PINS);
    GPIOPinConfigure (GPIO_PA0_U0RX);
    GPIOPinConfigure (GPIO_PA1_U0TX);

    UARTConfigSetExpClk(UART_USB_BASE, SysCtlClockGet(), BAUD_RATE,
            UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
            UART_CONFIG_PAR_NONE);
    UARTFIFOEnable(UART_USB_BASE);
    UARTEnable(UART_USB_BASE);
}


//**********************************************************************
// Transmit a string via UART0
//**********************************************************************
void
UARTSend (char *pucBuffer)
{
    // Loop while there are more characters to send.
    while(*pucBuffer)
    {
        // Write the next character to the UART Tx FIFO.
        UARTCharPut(UART_USB_BASE, *pucBuffer);
        pucBuffer++;
    }
}

void UART_transmit(uint8_t heli_state, uint8_t alt_PWM, uint8_t yaw_PWM, int16_t alt, int16_t yaw)

{

    usprintf (statusStr, "=============== \r\n"); // * usprintf
    UARTSend (statusStr);
    usprintf (statusStr, "Alt: %9i \r\n", alt); // * usprintf
    UARTSend (statusStr);
    usprintf (statusStr, "Yaw: %9i \r\n", yaw); // * usprintf
    UARTSend (statusStr);
    usprintf (statusStr, "Alt Duty: %4i \r\n", alt_PWM); // * usprintf
    UARTSend (statusStr);
    usprintf (statusStr, "Yaw Duty: %4i \r\n", yaw_PWM); // * usprintf
    UARTSend (statusStr);

    if (heli_state == 0) {
        usprintf (statusStr, "State: Landed \r\n"); // * usprintf
        UARTSend (statusStr);

    } else if(heli_state == 1) {
        usprintf (statusStr, "State: Take off \r\n"); // * usprintf
        UARTSend (statusStr);

    } else if(heli_state == 2) {
        usprintf (statusStr, "State: Flying \r\n"); // * usprintf
        UARTSend (statusStr);

    } else if (heli_state == 3) {
        usprintf (statusStr, "State: Landing \r\n"); // * usprintf
        UARTSend (statusStr);
    }

}
