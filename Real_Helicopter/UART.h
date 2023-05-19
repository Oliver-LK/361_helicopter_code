#ifndef UART_H
#define UART_H

// ***********************************************************
// AUTHOR        : Ben Stirling and Oliver Clements, derived from code by Phil Bones
// CREATE DATE   : 04/05/2023
// PURPOSE       : Enables serial transmissions to a console. 
//
// ***********************************************************


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

#define UART_LANDED   0
#define UART_TAKE_OFF 1
#define UART_FLYING   2
#define UART_LANDING  3

#define MAX_STR_LEN 16
char statusStr[MAX_STR_LEN + 1];

// Function decelerations
void initialiseUSB_UART (void);
//Initialises the UART modules per the #defines.

void UARTSend (char *pucBuffer);
//Sends a message over UART to the host computer.

void UART_transmit(uint8_t heli_state, uint8_t alt_PWM, uint8_t yaw_PWM, int16_t alt, int16_t yaw);

#endif
