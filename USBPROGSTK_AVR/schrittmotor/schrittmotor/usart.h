/*----------------------------------------------------------------------------
 Copyright:      Ulrich Radig (mail@ulrichradig.de)
 Author:         Ulrich Radig
 Remarks:        
 known Problems: none
 Version:        29.06.2010
 Description:    RS232 Routinen
------------------------------------------------------------------------------*/

#ifndef _UART_H
	#define _UART_H		

#define BUFFER_SIZE	20

volatile unsigned char usart_rx_ready;
volatile unsigned char usart_rx_buffer[BUFFER_SIZE];

//Der Quarz auf dem Experimentierboard
#ifndef SYSCLK
		//#define SYSCLK 12000000UL
		//#define SYSCLK 14745600UL
		#define SYSCLK 8000000UL
#endif //SYSCLK	
	
	#if defined (__AVR_ATmega644__) || defined (__AVR_ATmega644P__)
		#define USR UCSR0A
		#define UCR UCSR0B
		#define UBRRL UBRR0L
		#define UBRRH UBRR0H
		#define EICR EICRB
		#define TXEN TXEN0
		#define RXEN RXEN0
		#define RXCIE RXCIE0
		#define UDR UDR0
		#define UDRE UDRE0
		#define USART_RX USART0_RX_vect   
	#endif
	
	#if defined (__AVR_ATmega32__)
		#define USR UCSRA
		#define UCR UCSRB
		#define EICR EICRB
		#define USART_RX USART_RXC_vect  
	#endif
		#if defined (__AVR_ATmega8__)
		#define USR UCSRA
		#define UCR UCSRB
		#define EICR EICRB
		#define USART_RX USART_RXC_vect  
	#endif

void usart_init(unsigned long baudrate); 
void usart_write_char(char c);
void usart_write_str(char *str);
	
void usart_write_P (const char *Buffer,...);
#define usart_write(format, args...)   usart_write_P(PSTR(format) , ## args)
	
//----------------------------------------------------------------------------

#endif //_UART_H

