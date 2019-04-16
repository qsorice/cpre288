
#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include <stdbool.h>
#include "timer.h"
#include "lcd.h"
#include <inc/tm4c123gh6pm.h>
#include "driverlib/interrupt.h"
#include "lab5_scan_data.h"

extern volatile int receive;
extern volatile char c;
extern volatile char result[20];

void uart_interrupts_init();

void uart_init();

char uart_receive();

void uart_sendChar(char data);

void transmit_recieved();

void My_UART1_Handler();

char print_recieved();

void uart_sendInt(int data);

void uart_sendStr(const char *data);

#endif
