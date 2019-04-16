/*
 * ADC_stuff.h
 *
 *  Created on: Mar 5, 2019
 *      Author: qsorice
 */

#ifndef ADC_STUFF_H_
#define ADC_STUFF_H_

#include <stdint.h>
#include <stdbool.h>
#include "timer.h"
#include "lcd.h"
#include <inc/tm4c123gh6pm.h>
#include "driverlib/interrupt.h"

void get_sensor_reading(double *sensor);

void init_ADC();

int adc_read(double average[]);

#endif /* ADC_STUFF_H_ */
