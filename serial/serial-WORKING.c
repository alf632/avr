/*************************************************************************
Title:    example program for the Interrupt controlled UART library
Author:   Peter Fleury <pfleury@gmx.ch>   http://jump.to/fleury
File:     $Id: test_uart.c,v 1.5 2012/09/14 17:59:08 peter Exp $
Software: AVR-GCC 3.4, AVRlibc 1.4
Hardware: any AVR with built-in UART, tested on AT90S8515 at 4 Mhz

DESCRIPTION:
          This example shows how to use the UART library uart.c

*************************************************************************/
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "uart.c"


/* define CPU frequency in Mhz here if not defined in Makefile */
#ifndef F_CPU
#define F_CPU 1000000UL
#endif

#include <util/delay.h>

/* 9600 baud */
#define UART_BAUD_RATE      1200      

int main(void)
{
    unsigned int c;

    
    /*
     *  Initialize UART library, pass baudrate and AVR cpu clock
     *  with the macro 
     *  UART_BAUD_SELECT() (normal speed mode )
     *  or 
     *  UART_BAUD_SELECT_DOUBLE_SPEED() ( double speed mode)
     */
    uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) ); 
    
    /*
     * now enable interrupt, since UART library is interrupt controlled
     */
    sei();
    uart_putc('\n');
    uart_putc('\r');
    
    for(;;)
    {
        /*
         * Get received character from ringbuffer
         * uart_getc() returns in the lower byte the received character and 
         * in the higher byte (bitmask) the last receive error
         * UART_NO_DATA is returned when no data is available.
         *
         */
        c = uart_getc();
        if ( c & UART_NO_DATA )
        {
            /* 
             * no data available from UART 
             */
        }
        else
		{
			switch (c) {
				case 'r':
					uart_puts_P("READ");
					break;
				case 'w':
					uart_puts_P("WRITE");
					break;
				default:
					uart_puts_P("\n\rr - READ\n\rw - WRITE\n\r");
					break;
			}
		}
    }
    
}
