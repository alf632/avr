/*----------------------------------------------------------------------------
 Copyright:      Ulrich Radig (mail@ulrichradig.de)
 Author:         Ulrich Radig
 Remarks:        
 known Problems: none
 Version:        29.06.2010
 Description:    RS232 Routinen
------------------------------------------------------------------------------*/

#include "usart.h"

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <avr/io.h>

volatile unsigned char usart_rx_ready = 0;
volatile unsigned char buffercounter = 0;
volatile unsigned char usart_rx_buffer[BUFFER_SIZE];

//----------------------------------------------------------------------------
//Init serielle Schnittstelle
void usart_init(unsigned long baudrate) 
{
  	//Enable TXEN im Register UCR TX-Data Enable
	UCSRB = (1 << TXEN | 1 << RXEN | 1<< RXCIE);
	//Teiler wird gesetzt 
	UBRRL = (SYSCLK / (baudrate * 16L) - 1) & 0x00FF;
	UBRRH = (((SYSCLK / (baudrate * 16L) - 1) & 0xFF00)>>8);

/*
	UBRRL = (SYSCLK / (baudrate * 16L) - 1) & 0x00FF;
	UBRRH = (((SYSCLK / (baudrate * 16L) - 1) & 0xFF00)>>8);
    UCSRB = (1<<RXEN)|(1<<TXEN)| (1<<RXCIE) ;
   // UCSRC = (1<<URSEL)|(1<<USBS)|(3<<UCSZ0);
*/
}

//----------------------------------------------------------------------------
//Routine für die Serielle Ausgabe eines Zeichens
void usart_write_char(char c)
{
	if (c == '\n')
		usart_write_char('\r');
	//Warten solange bis Zeichen gesendet wurde
	while(!(USR & (1<<UDRE)));
	//Ausgabe des Zeichens
	UDR = c;
	return;
}

//------------------------------------------------------------------------------
void usart_write_P (const char *Buffer,...)
{
	va_list ap;
	va_start (ap, Buffer);	
	
	int format_flag;
	char str_buffer[10];
	char str_null_buffer[10];
	char move = 0;
	char Base = 0;
	int tmp = 0;
	char by;
	char *ptr;
		
	//Ausgabe der Zeichen
    for(;;)
	{
		by = pgm_read_byte(Buffer++);
		if(by==0) break; // end of format string
            
		if (by == '%')
		{
            by = pgm_read_byte(Buffer++);
			if (isdigit(by)>0)
				{
                                 
 				str_null_buffer[0] = by;
				str_null_buffer[1] = '\0';
				move = atoi(str_null_buffer);
                by = pgm_read_byte(Buffer++);
				}

			switch (by)
				{
                case 's':
                    ptr = va_arg(ap,char *);
                    while(*ptr) { usart_write_char(*ptr++); }
                    break;
				case 'b':
					Base = 2;
					goto ConversionLoop;
				case 'c':
					//Int to char
					format_flag = va_arg(ap,int);
					usart_write_char (format_flag++);
					break;
				case 'i':
					Base = 10;
					goto ConversionLoop;
				case 'o':
					Base = 8;
					goto ConversionLoop;
				case 'x':
					Base = 16;
					//****************************
					ConversionLoop:
					//****************************
					itoa(va_arg(ap,int),str_buffer,Base);
					int b=0;
					while (str_buffer[b++] != 0){};
					b--;
					if (b<move)
						{
						move -=b;
						for (tmp = 0;tmp<move;tmp++)
							{
							str_null_buffer[tmp] = '0';
							}
						//tmp ++;
						str_null_buffer[tmp] = '\0';
						strcat(str_null_buffer,str_buffer);
						strcpy(str_buffer,str_null_buffer);
						}
					usart_write_str (str_buffer);
					move =0;
					break;
				}
			
			}	
		else
		{
			usart_write_char ( by );	
		}
	}
	va_end(ap);
}

//----------------------------------------------------------------------------
//Ausgabe eines Strings
void usart_write_str(char *str)
{
	while (*str)
	{
		usart_write_char(*str++);
	}
}

//----------------------------------------------------------------------------
//Empfang eines Zeichens
ISR (USART_RX)
{
	return;
}


