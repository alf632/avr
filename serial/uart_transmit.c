/*************************************************************************
Title:    example program for the Interrupt controlled UART library
Author:   Peter Fleury <pfleury@gmx.ch>   http://jump.to/fleury
File:     $Id: test_uart.c,v 1.5 2012/09/14 17:59:08 peter Exp $
Software: AVR-GCC 3.4, AVRlibc 1.4
Hardware: any AVR with built-in UART, tested on AT90S8515 at 4 Mhz

DESCRIPTION:
          This example shows how to use the UART library uart.c
/home/malte/avr/serial/main.c:120: 
*************************************************************************/


#define F_CPU 1000000UL

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

// define some macros
#define BAUD 19200                                   // define baud
#define BAUDRATE ((F_CPU)/(BAUD*16UL)-1)            // set baud rate value for UBRR
  
// function to initialize UART
void uart_init (void)
{
    UBRRH = (BAUDRATE>>8);                      // shift the register right by 8 bits
    UBRRL = BAUDRATE;                           // set baud rate
    UCSRB|= (1<<TXEN)|(1<<RXEN);                // enable receiver and transmitter
    UCSRC|= (1<<URSEL)|(1<<UCSZ0)|(1<<UCSZ1);   // 8bit data format
}

// function to send data
void uart_transmit (unsigned char data)
{
    while (!( UCSRA & (1<<UDRE)));                // wait while register is free
    UDR = data;                                   // load data in the register
}

int main(void)
{
    
     
    /*
     * now enable interrupt, since UART library is interrupt controlled
     */
    sei();
    int i=0;
    uart_init();
    for(;;)
    {
			
		uart_transmit('t');
		uart_transmit('e');
		uart_transmit('s');
		uart_transmit('t');
		unsigned char c = (unsigned char) i;
		uart_transmit(c);
		uart_transmit('\n');
		uart_transmit('\r');
		_delay_ms(1000);
		i++;
    
}
}
