#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include "uart.c"


/* define CPU frequency in Mhz here if not defined in Makefile */
#ifndef F_CPU
#define F_CPU 1000000UL
#endif

#include <util/delay.h>

#define BLUE     _BV(PC5)
#define BLACK    _BV(PC4)
#define RED      _BV(PC3)
#define YELLOW   _BV(PC2)
/* 9600 baud */
#define UART_BAUD_RATE      1200      

unsigned long vol;

const uint8_t delay = 2;
uint8_t step = 0;
char last_direction='r';
char mode = 's'; // run/stop

const uint8_t clockwise[] = {BLUE, BLACK, RED, YELLOW};

void rotate (unsigned char direction)
{    
		PORTC = clockwise[step];      
		_delay_ms(delay);    
		if (direction == 'r')
		{
			step++;
			if (step>3)
			{
				step=0;
			}
		}
		else
			{
			if (step==0)
			{
				step=3;
			}
			else
			{
				step--;
			}
		}
		
}


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
    
    DDRC = 0xff;                  /* Enable output on all of the B pins */  
    PORTC = 0x00;                 /* Set them all to 0v */
    
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
             if ( mode == 'r' ){
             rotate(last_direction);
			}
        }
        else
		{
			switch (c) {
				case 'r':
					uart_puts_P("RIGHT\n\r");
					last_direction = 'r';
					mode = 'r';
					rotate('r');
					break;
				case 'l':
					uart_puts_P("LEFT\n\r");
					last_direction = 'l';
					mode = 'r';
					rotate('l');
					break;
				case 's':
					uart_puts_P("STOP\n\r");
					mode = 's';
					PORTC = 0x00;
					break;
				case 'c':
					uart_puts_P("CALIBRATE\n\r");
					//eeprom_update_byte (( uint8_t *) 42 , 0x00 );
					vol = 0
					break;
				default:
					uart_puts_P("\n\rr - Right\n\rl - Left\n\rs - stop\n\rc - Calibrate\n\r");
					break;
			}
		}
    }
    
}
