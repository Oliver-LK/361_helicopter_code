#ifndef UART_H
#define UART_H

//********************************************************
//
// UART.h - derived from uartDemo.c by Prof. Phil Bones.
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

#define MAX_STR_LEN 16
char statusStr[MAX_STR_LEN + 1];

// Function decelerations
void initialiseUSB_UART (void);
//Initialises the UART modules per the #defines.

void UARTSend (char *pucBuffer);
//Sends a message over UART to the host computer.

void UART_transmit(uint8_t heli_state, int32_t alt_PWM, int32_t yaw_PWM, int32_t alt, int32_t yaw);

#endif

