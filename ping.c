/*
 * ping.c
 *
 *  Created on: Mar 12, 2019
 *      Author: qsorice
 */

#include "ping.h"
#include "timer.h"
#include <math.h>

volatile unsigned int start= 0, end = 0;
volatile enum {LOW, HIGH, DONE} state;

volatile int* OverFlow_ptr;
volatile unsigned int* clockCycle_ptr;

void ping_init(void) {
    // 1. Set up GPIO
        // A) Configure GPIO module associated with Timer 1A
        // i. Turn on clock for GPIO Port B and Timer 1
        // Note: Timer 1A can use Port B or F, this code uses Port B
        // Note: Port F would use different pins of its port.
        SYSCTL_RCGCGPIO_R = SYSCTL_RCGCGPIO_R | 0b000010; // 0x02
        SYSCTL_RCGCTIMER_R |= 0b00001000; // Enable Timer 3’s clock

        // ii. Set digital or analog mode, and pin direction
        GPIO_PORTB_DEN_R |= 0b00001000; // Set pin 3 digital mode
        GPIO_PORTB_DIR_R |= 0b00001000;  // Set up pin 3 as output

        GPIO_PORTB_PCTL_R |= 0x00007000; // Use function T3CCP1(Which is the timer)

        // 2. Set up Timer 3B
        // A) Configure Timer 3 mode
        //Disable Timer 3B device while we set it up
        TIMER3_CTL_R &= ~0x100;
        timer_waitMicros(5);

        // Configure Timer 3B functionality
        TIMER3_CFG_R = 0x00000004; // Set to 16-bit mode
        TIMER3_TBMR_R |= 0b10111; //Count up, edge-time, capture mode
        TIMER3_CTL_R |= 0xC00; // Detect both edges
        TIMER3_TBPR_R = 0xFF; // Use prescaler extension to 24 bits
        TIMER3_TBILR_R = 0xFFFFFF; // Load max 24-bit value

        // B) Set up Timer 3B interrupts
        TIMER3_ICR_R |= 0b10000000000; //Clear capture event interrupt status
        TIMER3_IMR_R |= 0b10000000000; // Enable capture event interrupts

        // 3. NVIC setup
        // A) Configure NVIC to allow Timer 3B interrupts (use priority=1) //Vector 117 num101
        NVIC_PRI9_R |= 0x000020; //Timer3B IRQ pri=1, bits 7-5
        NVIC_EN1_R |= 0x10; //Enable Timer3B IRQ (20), bit 20

        // B) Bind Timer 3B interrupt requests to user’s interrupt handler
        IntRegister(INT_TIMER3B, ping_interrupt_handler);

        // Re-enable Timer 3B
        TIMER3_CTL_R |= 0x100;

        //Enable interrupts until necessary
        IntMasterEnable();
}

double ping_read(void) {
    ping_send();

        int time = 0;
        while(state != DONE){
        }

        //Calculates the distance in clock cycles
        if (start > end){
            *OverFlow_ptr += 1;
            end += 16777215;
        }

        time = end - start;

        *clockCycle_ptr = time;
        //Converts from clock cycles to distance
        double value = ((time / 16000000.0) * 340/2) * 100;

        return value;
}

void ping_send(void) {

    //Disable AFSEL
        GPIO_PORTB_AFSEL_R &= ~0b00001000;
        // Set up pin 3 as output
        GPIO_PORTB_DIR_R |= 0x08;


        //Send pulse (low HIGH low)
        GPIO_PORTB_DATA_R &= ~0x08;
        timer_waitMicros(2);
        GPIO_PORTB_DATA_R |= 0x08;
        timer_waitMicros(5);
        GPIO_PORTB_DATA_R &= ~0x08;
        //Start switching sequence

        //Enable AFSEL
        GPIO_PORTB_AFSEL_R |= 0x08;
        // Set up pin 3 as input
        GPIO_PORTB_DIR_R &= ~0x08;
        state = LOW;
}

void ping_interrupt_handler(void) {
    if (TIMER3_MIS_R & 0x400)
        {

            if (state == LOW)
            {
                state = HIGH;
                start = TIMER3_TBR_R;
    //            TIMER3_TBR_R = 0x0000;
            }
            else if (state == HIGH)
            {
                state = DONE;
                timer_waitMicros(10);
                end = TIMER3_TBR_R;
    //            TIMER3_TBR_R = 0x0000;
            }
            TIMER3_ICR_R |= 0x400; //clear interrupt
        }
}

//Helper method to initialize pointers
void add_ptrs(int *temp_ptr, unsigned int *temp2_ptr)
{
    OverFlow_ptr = temp_ptr;
    clockCycle_ptr = temp2_ptr;
}

