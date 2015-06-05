/*----------------------------------------------------------------------------
 Copyright:      Ulrich Radig (mail@ulrichradig.de)
 Author:         Ulrich Radig
 Remarks:        
 known Problems: none
 Version:        29.06.2010
 Description:    Hauptprogramm
------------------------------------------------------------------------------*/

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <avr/io.h>

#include "usart.h"
#include "TMC222_AVR.h"

//############################################################################
//Hauptprogramm
int main (void) 
//############################################################################
{
	char buffer[10];
	struct TMC222Par * value;
	value = (struct TMC222Par*)(unsigned char*)&buffer;
	
	struct TMC222Sta * value_sta1;
	value_sta1 = (struct TMC222Sta*)(unsigned char*)&buffer;
	
//	for(unsigned long a =0;a<1000000;a++){asm("nop");};
	usart_init(9600);
	sei();
	usart_write("System Ready!\r\n");
	InitTWI();
	
	value->IHold = 0x0;
	value->IRun = 0x8;
	value->VMin = 0x1;
	value->VMax = 0xF;
	value->Acc = 0x0;
	value->Shaft = 0x00;
	value->SecPosHi= 0x00;
	value->SecPosLo = 0x00;	
	value->StepMode =  0x3;
	value->AccShape = 0;
	
	SetMotorParameters(value);
	GetFullStatus1(value_sta1);
	
	while(1)
	{       

		SetPosition(0);
		for(unsigned long a =0;a<1000000;a++){asm("nop");};

		SetPosition(2000);
		usart_write("%li\r\n",value_sta1->ESW);
		for(unsigned long a =0;a<1000000;a++){asm("nop");};
		usart_write("%1i\r\n",value_sta1->ESW);
		
		
		//Ausgabe des Schalters
		GetFullStatus1(value_sta1);
		usart_write("%1i\r\n",value_sta1->ESW);
	}
}
