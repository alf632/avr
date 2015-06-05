#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include "uart.c"
#include <ctype.h>
#include <string.h>

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
    
uint8_t one;
uint8_t two;
uint8_t three;

long vol = 0;
char vol_str[50];
long target = 0;
char target_str[50];
long tempvol;
long temptarget;

const uint8_t delay = 2;
uint8_t step = 0;
const unsigned long tsteps = 500;
const unsigned long maxsteps = 2000;
const unsigned int FACTOR = 1500 / 256;
char last_direction='r';
char mode = 's'; // run/stop
uint8_t i = 0;

const uint8_t clockwise[] = {BLUE, BLACK, RED, YELLOW};

void rotate (unsigned char direction)
{    
		PORTC = clockwise[step];      
		_delay_ms(delay);    
		if (direction == 'r')
		{
			if (step>=3)
			{
				step=0;
				vol++;				
			}
			else
			{
				step++;
				vol++;
			}
		}
		else
			{
			if (step<=0)
			{
				step=3;
				vol--;
			}
			else
			{
				step--;
				vol--;
			}
		}
		
}

void automat (void)
{
if (mode == 's')
{
	return;
}
	
if ( target <= 0 )
             {
				 target=0;
			 }
if ( target >= maxsteps )
             {
				 target=maxsteps;
			 }

if ( vol < target)
             {
				 rotate('r');
				 mode='r';
			 }
if ( vol > target)
             {
				 rotate('l');
				 mode='r';
			 }
if ( vol == target)
             {
				 mode='s';
				 PORTC = 0x00;
			 }
}

void echo_vol (void)
{
	tempvol=vol/FACTOR;
	temptarget=target/FACTOR;
	ltoa(tempvol,vol_str,10);
	ltoa(temptarget,target_str,10);
	uart_puts("Vol:");
	uart_puts(vol_str);
	uart_puts(" Target:");
	uart_puts(target_str);
	uart_puts("\n\r");
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
	
	echo_vol();
    
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
             //if ( mode == 'r' ){
             //rotate(last_direction);
             cli();
             automat();
             sei();
			//}
        }
        else
		{
			switch (c) {
				case 'r':
					uart_puts_P("RIGHT\n\r");
					target=target+tsteps;
					last_direction = 'r';
					mode = 'r';
					//rotate('r');
					break;
				case 'l':
					uart_puts_P("LEFT\n\r");
					target=target-tsteps;
					last_direction = 'l';
					mode = 'r';
					//rotate('l');
					break;
				case 's':
					uart_puts_P("STOP\n\r");
					mode = 's';
					PORTC = 0x00;
					break;
				case 'c':
					uart_puts_P("CALIBRATE\n\r");
					vol = 0;
					target=0;
					break;
				case 'i':
					echo_vol();
					break;
				case 't':
					uart_puts_P("Target\n\r");
					mode = 'r';
					while (1)
					{
						c = uart_getc();
						if (~ c & UART_NO_DATA )
						{
							target = (unsigned int) c * FACTOR;
							break;
						}
					}
					ltoa(target, target_str, 10);
					uart_puts("New Target:\n\r");
					uart_puts(target_str);
					uart_puts("\n\r");					
					break;
				default:
					uart_puts_P("\n\rr - Right\n\rl - Left\n\rs - stop\n\rc - Calibrate\n\rt - Target\n\ri - Info\n\r");
					echo_vol();
					break;
			}
		}
    }
    
}
