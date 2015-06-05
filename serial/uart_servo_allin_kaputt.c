#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include "uart.c"
#include <avr/wdt.h>
#include <string.h>
#include <ctype.h>

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
#define MAXSTEPS      4096
#define TARGETSTEPS      1000UL

unsigned long vol;
char *vol_str;
unsigned long target;
char *target_str;
char target_stra[10];

const uint8_t delay = 10;
uint8_t step = 0;
char direction='r';
char mode = 's'; // run/stop
const uint8_t clockwise[] = {BLUE, BLACK, RED, YELLOW};

uint8_t i = 0;
char *one;
char *two;
char *three;

void rotate (unsigned char direction)
{    
		PORTC = clockwise[step];      
		_delay_ms(delay);    
		if (direction == 'r')
		{
			step++;
			vol++;
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
	
if ( vol < target)
             {
				 direction='r';
				 mode='r';
			 }
if ( vol > target)
             {
				 direction='l';
				 mode='r';
			 }
if ( vol == target)
             {
				 mode='s';
				 PORTC = 0x00;
			 }
if ( vol <= 0 || vol >= MAXSTEPS )
             {
				 mode='s';
			 }
}

//void WDT_Init(void)
//{
// Timer 0 konfigurieren
//TIMSK |= (1 << TOIE0);
//TCCR0 |= (1 << CS02);
    // set prescaler to 256 and start the timer
//}
//ISR (TIMER0_OVF_vect)
//{
//	eeprom_update_byte (( uint8_t *) 42 , vol );
//}

int main(void)
{

    unsigned int c;

	//vol = eeprom_read_byte (( uint8_t *) 42 );
	vol = 0;
	target = vol;

    //WDT_Init();
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

	char *test;
	unsigned long bla = 0x123UL;
	ltoa(bla, test, 10);
	uart_puts(test);
    
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
                       
             automat();             
             
             if ( mode == 'r' ){
             rotate(direction);
			}
        }
        else
		{
			switch (c) {
				case 'r':
					uart_puts_P("RIGHT\n\r");
					ltoa(vol, vol_str, 10);
					ltoa(target, target_str,10);
					uart_puts(target_str);
					uart_puts(vol_str);
					uart_puts("\n\r");
					direction = 'r';
					mode = 'r';
					rotate('r'); 
					//if ( target + TARGETSTEPS <= MAXSTEPS)
					//{
						target = target + TARGETSTEPS;
					//}
					//else
					//{
					//	target = vol;
					//	mode = 's';
					//	PORTC = 0x00;
					//}
					break;
				case 'l':
					uart_puts_P("LEFT\n\r");
					ltoa(vol, vol_str, 10);
					ltoa(target, target_str,10);
					uart_puts(target_str);
					uart_puts(vol_str);
					uart_puts("\n\r");
					direction = 'l';
					mode = 'r';
					rotate('l');
					if ( target - TARGETSTEPS >= 0)
					{
						target = target - TARGETSTEPS;
					}
					else
					{
						target = vol;
						mode = 's';
						PORTC = 0x00;
					}
					break;
				case 's':
					uart_puts_P("STOP\n\r");
					mode = 's';
					target = vol;
					PORTC = 0x00;
					break;
				case 'c':
					uart_puts_P("CALIBRATE\n\rBefore:\n\r");
					ltoa(vol, vol_str, 10);
					ltoa(target, target_str,10);
					uart_puts(target_str);
					uart_puts("\n\r");
					uart_puts(vol_str);
					uart_puts("\n\rAfter:\n\r");
					//eeprom_update_byte (( uint8_t *) 42 , 0x00 );
					//eeprom_update_byte (( uint8_t *) 43 , 0x00 );
					ltoa(vol, vol_str, 10);
					ltoa(target, target_str,10);
					vol = 0;
					target = 0;
					ltoa(vol, vol_str, 10);
					ltoa(target, target_str,10);
					uart_puts(target_str);
					uart_puts("\n\r");
					uart_puts(vol_str);
					uart_puts("\n\r");
					break;
				case 't':
					uart_puts_P("Target\n\r");
					mode = 'r';
					target_str="";
					while ( i<3 )
					{
						c = uart_getc();
						if (~ c & UART_NO_DATA )
						{
						switch(i){
						  case 0:
						    one = (char *)c;
						    i++;
						    break;
						  case 1:
						    two = (char *)c;
						    i++;
						    break;
						  case 2:
							i=0;
						    three = (char *)c;
						    break;
						}
						}}
						
						if ( isdigit((int)one) && isdigit((int)two) && isdigit((int)three) )//&& (three == '1' || three == '2'))
						{
						
						strcat(target_str, one);
						strcat(target_str, two);
						strcat(target_str, three);
						//todo: calculate long by factoring maxsteps and target_str
						target = atol(target_str);
						
						uart_puts("New Target:\n\r");
						uart_puts(target_str);
						uart_puts("\n\r");
						//target = one * 1 + two * 10 + three * 100;
						}
						else
						{
							uart_puts("Wrong Input:\n\r");
						}
					
					break;
				default:
					uart_puts_P("\n\rr - Right\n\rl - Left\n\rs - stop\n\rc - Calibrate\n\r");
					ltoa(vol, vol_str, 10);
					ltoa(target, target_str,10);
					uart_puts(target_str);
					uart_puts("\n\r");
					uart_puts(vol_str);
					uart_puts("\n\r");
					uart_puts(test);
					uart_puts("\n\r");
					break;
			
		}
    }
    
}
}
