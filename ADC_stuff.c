/*
 * ADC_stuff.c
 *
 *  Created on: Mar 5, 2019
 *      Author: qsorice
 */

#include "ADC_stuff.h"
#include <math.h>

void init_ADC()
{
    // 1. Setup GPIO
     // A) Configure GPIO module associated with ADC
     // i. Turn on clock for GPIO Port B
     SYSCTL_RCGCGPIO_R = SYSCTL_RCGCGPIO_R | 0b000010; // 0x10
     SYSCTL_RCGCADC_R |= 0x1; // Enable ADC0 clock
     // ii. Enable alternate function
     GPIO_PORTB_AFSEL_R |= 0b00010000; //ADC channel 10 on pins 4

     // iii. Configure pins for input and analog mode
     GPIO_PORTB_DIR_R &= 0b11101111; //Pin 4 set to input
     GPIO_PORTB_DEN_R &= 0b11101111; //Disable digital mode pin 4
     GPIO_PORTB_AMSEL_R |= 0b00010000; //Enable analog mode pin 4
     // iv. Use default sample trigger source
     // By default, the processor (your program) is selected as the
     // trigger source and is initiated by the sample sequencer (SS)
     GPIO_PORTB_ADCCTL_R = 0x0; //Trigger in software
     // 2. Setup ADC
     // A) Configure ADC (choose ADC0 or ADC1)

     // Choose a sample sequencer (SS0, SS1, SS2 or SS3)
     // Disable ADC sample sequencer SS0 while configuring
     // OK to use SS1 or SS2 (cannot use SS3 for multiple samples)
     ADC0_ACTSS_R &= 0xFFFE; //Disable ADC0 SS0 by clearing bit 0

     // Set ADC SS0 functionality
     ADC0_EMUX_R &= 0xFFF0; //Set SS0 to trigger using ADCPSSI reg.
     ADC0_SSMUX0_R |= 0x0000000A; // Sample channel 10

     // Configure which sample is end of sequence (bit 1 of 4-bit
     // field), after which sample the RIS flag is asserted (bit 2)
     // Use second sample (channel 1 is first, channel 9 is second)
     ADC0_SSCTL0_R = 0x00000000; // First clear register to 0
     ADC0_SSCTL0_R |= 0x00000006; // Set bits 1
     // B) Set up ADC interrupts
     //NONE
     // 3. Initialize NVIC
     // A) Configure NVIC to allow ADC interrupts
     //NONE
     // B) Bind ADC interrupt requests to user’s interrupt handler
     //NONE
     // Re-enable ADC SS0
     ADC0_ACTSS_R |= 0x1; //Re-enable ADC0 SS0 by setting bit 0
}

void get_sensor_reading(double *sensor)
{
     // Initiate SS0 conversion sequence
    ADC0_PSSI_R = 0x1;
    // Wait for SS0 ADC conversions to complete
    while((ADC0_RIS_R & 0x1) == 0){ // or (~ADC0_RIS_R & 0x1)
    //wait
    }
    // Clear raw interrupt status flag for SS0
    ADC0_ISC_R = 0x01;
    // Get converted results from SS0 FIFO
    *sensor = ADC0_SSFIFO0_R; // Channel 10
}

int adc_read(double average[]) //calibrated to BOT 6**************
{
    double sum = 0;   //somewhere to store the sum of the values

    int i, result;
    for(i = 0; i < 16; i++)
        sum+= average[i]; //add everything together
    sum = sum/16.0;       //divide by 16

    double ya = pow(sum, -1.98450954912); //put that value into our function
    double yeet = 60816502.5227*ya;       //that we made on a calculator
    yeet = round(yeet);   //round that value to be a whole integer
    result = (int) yeet;  //now store it as a double
    return result;        //return the nice whole number rounded to the nearest centimeter
}

