/*
 * AVRGCC1.c
 *
 * Created: 03.11.2011 11:37:00
 *  Author: eproo
 */ 

#include <avr/io.h>          // (1)
#define F_CPU 1000000
#include <util/delay.h>          // (1)

int main(void)
{
    DDRB |= (1 << PB3)|(1 << PB2); /* LEDs Output */
    DDRB &= ~((1 << PB1)|(1 << PB0)); /* Button Input */

    PORTB &= ~((1 << PB3)|(1 << PB2));/*LED OFF*/

    while(1)
    {
      if (!(PINB & 1<< PB1)){ /* Taster 1 pressed*/
        PORTB |= (1 << PB2); /*LED1 ON*/
      } else  {
        PORTB &= ~(1 << PB2); /*LED1 OFF*/
      }

      if (!(PINB & 1<<PB0)){ /*Taster 2 pressed*/
	PORTB |= (1 << PB3); /*LED2 ON*/
      }
      else {
	PORTB &= ~(1 << PB3); /*LED2 OFF*/
      }
    }

    return 0;         
}
