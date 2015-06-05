/*
 * AVRGCC1.c
 *
 * Created: 03.11.2011 11:29:58
 *  Author: eproo
 */ 

#include <avr/io.h>
#define F_CPU 1000000
#include <util/delay.h>         

int main(void)
{
   DDRB  |= (1<<PB2)|(1<<PB3);        
 
   while(1) {              
	PORTB |=(1<<PB2)|(1<<PB3);
	_delay_ms(1000);
	PORTB &=~((1<<PB2)|(1<<PB3));
	_delay_ms(1000);
   }           
}
