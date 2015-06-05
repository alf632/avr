/* Alle Zeichen zwischen Schrägstrich-Stern 
   und Stern-Schrägstrich sind lediglich Kommentare */
 
// Zeilenkommentare sind ebenfalls möglich
// alle auf die beiden Schrägstriche folgenden
// Zeichen einer Zeile sind Kommentar

#include <avr/io.h>

 
#define F_CPU 1000000                /*OSC in Hz*/

#include <util/delay.h>



void InitUART(uint32_t baud)
{
        uint16_t baudrate=(F_CPU/(16*baud))-1;
        /*Set baud rate*/
        UBRRH = (unsigned char)(baudrate>>8);
        UBRRL = (unsigned char)baudrate;
        /* Enable Receiver and Transmitter */
        UCSRB = (1<<RXEN)|(1<<TXEN);
        /* Set frame format: 8data, 1stop bit */
        UCSRC = (1<<URSEL)|(3<<UCSZ0);

}

unsigned char UARTReceive(void)
{
        if (UCSRA & (1<<RXC))
                return UDR;
        else    /* no data pending */
                return 0;
}

void UARTTransmit(unsigned char data)
{
        while (!( UCSRA & (1<<UDRE)) );
        /* Put data into buffer, sends the data */
        UDR = data;
}

// Einen 0-terminierten String übertragen. 
void UARTPuts (const char *s)
{
    do
    {
        UARTTransmit(*s);
    }
    while (*s++);
}



char c;
int main()
{

        InitUART(2400);
        
  

        UARTPuts("Welcome on ATMEGA 8\r\n");
        UARTPuts("Please type and see echo\r\n");


        while(1){

                c=UARTReceive();
                
                if (c){

                      UARTTransmit(c);
                }

        }

        return 0;
}



