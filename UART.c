/*
 * UART.c
 *
 *  Created on: Feb 21, 2019
 *      Author: ers99
 */
#include "UART.h"

int flag = 0;
int count = 0;
volatile int receive;
volatile char c;
volatile char result[20];

void uart_init()
{
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;
    SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R1;
    GPIO_PORTB_AFSEL_R |= (BIT0 | BIT1);
    GPIO_PORTB_PCTL_R |= 0x11;
    GPIO_PORTB_DEN_R |= (BIT0 | BIT1);
    GPIO_PORTB_DIR_R &= ~BIT0;
    GPIO_PORTB_DIR_R |= BIT1;
    UART1_CTL_R &= 0b0; //Disable the UART for setup
    UART1_IBRD_R = 8; //UART1_IBRD_R = 104;
    UART1_FBRD_R = 44; //Set the baud rate fraction portion
    UART1_LCRH_R = UART_LCRH_WLEN_8; //Write the serial parameters
    UART1_CC_R = UART_CC_CS_SYSCLK; //Set the UART clock to 16MHz
    UART1_CTL_R |= 0b1100000001; //Turn the UART back on
}

void uart_interrupts_init(){
    UART1_ICR_R |= 0b00110000; //Clear RX and TX interrupt flags
    UART1_IM_R |= 0b00110000; //Enable RX and TX interrupts
    NVIC_PRI1_R |= 0x200000; //Set UART1 IRQ priority to 1 (bits 23-21) (Meeting 9 slides pg.23)
    NVIC_EN0_R |= 0b1000000; //Enable UART1 IRQ 6 (Datasheet pg. 104)

    IntRegister(INT_UART1, My_UART1_Handler);
}

void My_UART1_Handler()
{
        UART1_ICR_R |= 0b11101111; //set ICR received but to 1
        if(UART1_MIS_R & 0b00010000) { //if a receive interrupt has happened
            UART1_ICR_R |= 0b00010000; //set ICR received but to 1
            c = uart_receive(); //save the character that was received
            result[count] = c;  //store that character in an array of size 20
            if(c != '\r')       //track if enter was the last received char
                flag = 0;
            count++;            //increment the array position counter
        }
}

char uart_receive()
{
    char data = 0;
   // while(UART1_FR_R & UART_FR_RXFE)     //disabled for interrupts
        //wait until data is received
    data = (char) (UART1_DR_R & 0xFF);      //take in whatever data was received by the bot
    return data;                            //return it
}

void uart_sendChar(char data)
{
    while(UART1_FR_R & 0x20) { //wait until there is data
    }
    UART1_DR_R = data;
}

void uart_sendStr(const char *data)
{
    //until we reach a null character
    while (*data != '\0')
    {
        //send the current character
        uart_sendChar(*data);
        // increment the pointer.
        data++;
    }

}
