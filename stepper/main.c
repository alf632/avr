/* Playing with getting the small stepper motors driven. */

/* Include delay function */
#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>

#define DELAY  100 /* milliseconds between steps */

int main(void){  
  DDRB = 0xff;    /* Enable output on all of the B pins */  
  PORTB = 0x00;            /* Set them all to 0v */  
  unsigned int Steps = 0;
  while(1){                     /* main loop here */    
	  
	 if ( Steps > 7 )
	 {
	    Steps = 0;
	 }
	  
switch(Steps){
   case 0:
		PORTB = 0b00000001;
   break; 
   case 1:
		PORTB = 0b00000011;
   break; 
   case 2:
		PORTB = 0b00000010;
   break; 
   case 3:
		PORTB = 0b00000110;
   break; 
   case 4:
		PORTB = 0b00000100;
   break; 
   case 5:
     	PORTB = 0b00001100;
   break; 
     case 6:
		PORTB = 0b00001000;
   break; 
   case 7:
     	PORTB = 0b00001001;
   break; 
   default:
     	PORTB = 0b00000000;
   break;  
   }
   Steps ++;
   
   _delay_ms(1);
}
}
