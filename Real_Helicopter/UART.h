
//********************************************************
//
// UART.h - derived from uartDemo.c by Prof. Phil Bones.
//
//
//
// Author:  P.J. Bones  UCECE
// Last modified:   16.4.2018
//

#ifndef UART_H
#define UART_H
#include <stdint.h>
#include <stdbool.h>
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


void initialiseUSB_UART (void);
//Initialises the UART modules per the #defines.

void UARTSend (char *pucBuffer);
//Sends a message over UART to the host computer.

#endif
