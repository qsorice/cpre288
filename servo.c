/*
 * servo.c
 *
 *  Created on: Mar 26, 2019
 *      Author: qsorice
 */
#include "servo.h"

volatile double current_degree = 0;
volatile int clockwise = 0;
volatile unsigned int matcher;

void servo_init(void) {

    SYSCTL_RCGCGPIO_R = SYSCTL_RCGCGPIO_R | 0b000010; // 0x02
    SYSCTL_RCGCTIMER_R |= 0b00000010; // Enable Timer 1’s clock

    GPIO_portb_init();

    TIMER1_CTL_R &= ~0x100; //disable timer 1B
    timer_waitMicros(5);

    TIMER1_CFG_R = 0x4;      // set to 16-bit mode
    TIMER1_TBMR_R |= 0b01010; // Count down
    TIMER1_TBMR_R &= 0b11011; // CLEAR BIT 2

    unsigned int pwm_period = 320000; //clock cycles in 20ms
    TIMER1_TBILR_R  = pwm_period & 0x00FFFF; //storing the last 16 bit
    TIMER1_TBPR_R  = pwm_period >> 16; //storing the other 8 bits in a prescale
  //TIMER1_ICR_R |= 0b10000000000;  // Clear capture event interrupt status
  //TIMER1_IMR_R |= 0b10000000000;  // Enable capture event interrupts

  //NVIC_PRI9_R |= 0x00000020; //Timer1B IRQ pri=1,
  //NVIC_EN1_R |= 0x00000010;  //Enable Timer3B IRQ (36), bit 4

  //IntRegister(INT_TIMER3B, ping_interrupt_handler);

    TIMER1_CTL_R |= 0b100000000; //enable timer 1b

   // IntMasterEnable();
}

void GPIO_portb_init(void) {

    GPIO_PORTB_AFSEL_R |= 0b00100000; // ping input is PB5 (pin 5)
    GPIO_PORTB_PCTL_R |= 0x00700000;  // Use function T1CCP0
    GPIO_PORTB_DEN_R |= 0b00100000; // Set pin 5 digital mode
    GPIO_PORTB_DIR_R |= 0b00100000;  // Set up pin 5 as output
}

void servo_move(double degrees) {

    //TBR == BMR


    matcher = 310000 - 152.77 * degrees; //180 degrees is 282500 for bot 6, 0 degrees is 310

    TIMER1_TBMATCHR_R = matcher & 0x00FFFF;
    TIMER1_TBPMR_R = matcher >> 16; //shift the last 8 bits

    //unnecessary?
    unsigned int duty_cycle = (TIMER1_TBILR_R - TIMER1_TBMATCHR_R)/TIMER1_TBILR_R; // high/period
}

void servo_button_press(int button)
{
    //take in the button # returned by button_getButton()
    if(button == 1)
    {
        if(clockwise)
        {
            current_degree -= 1;
        }
        else
        {
            current_degree += 1;
        }
        //rotate the bot by 1 degree

    }
    else if(button == 2)
    {
        if(clockwise)
        {
            current_degree -= 2.5;
        }
        else
        {
            current_degree += 2.5;
        } //rotate the bot by 2.5 degrees

    }
    else if(button == 3)
    {
        if(clockwise)
        {
            current_degree -= 5;
        }
        else
        {
            current_degree += 5;
        } //rotate the bot by 5 degrees

    }
    else if(button == 4)
    {
        if(clockwise)
            clockwise = 0;
        else
            clockwise = 1;
        timer_waitMillis(120); // change the direction of the rotation
    }
    else if(button == 5)
    {
        current_degree = 0; //set the bot to 0 degrees
    }
    else if(button == 6)
    {
        current_degree = 180; //set the bot to 180 degrees
    }
    if(current_degree > 180)
        current_degree = 180;
    else if(current_degree < 0)
        current_degree = 0;

    servo_move(current_degree); //change the servo's degree now
    timer_waitMillis(180); //psuedo-button-debouncing
}

double servo_get_degree(void)
{
    return current_degree; //return the current degree value
}
