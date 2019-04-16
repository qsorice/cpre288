/*
 * ping.h
 *
 *  Created on: Mar 12, 2019
 *      Author: qsorice
 */

#ifndef PING_H_
#define PING_H_

#include <stdint.h>
#include <stdbool.h>
#include "timer.h"
#include "lcd.h"
#include <inc/tm4c123gh6pm.h>
#include "driverlib/interrupt.h"

extern volatile unsigned int start, end;

void ping_init(void);
double ping_read(void);
void ping_send(void);
void ping_interrupt_handler(void);
void add_ptrs(int *temp_ptr, unsigned int *temp2_ptr);

#endif /* PING_H_ */
