/*******************************************************************************
  Example library for using a TMC222 together with AVR processors that have a 
  hardware TWI interface (e.g. ATmega8, ATmega48, ATmega88, ATmega168,
  ATmega16, ATmega32, ATmega64, ATmega128, AT90CAN128 and many more).

  It implements and interrupt driven TWI master communication over the TWI
  of the AVR MCU and utilizes this for communication with the TMC222.
  Routines for all TMC222 commands are provided to make it easy to use the
  TMC222.

	It has been developed using the AVR GCC (WinARM distribution).

   Copyright (C) 2008 TRINAMIC Motion Control GmbH & Co KG
                      Sternstraﬂe 67
                      D - 20357 Hamburg, Germany
                      http://www.trinamic.com/
  
   This program is free software; you can redistribute it and/or modify it 
   freely.
   
   This program is distributed in the hope that it will be useful, but 
   WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
   or FITNESS FOR A PARTICULAR PURPOSE.
   
******************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stddef.h>
#include "TMC222_AVR.h"

#define FALSE 0
#define TRUE 1

//Useful macros for accessing single bytes of int and long variables
#define BYTE3(var) *((unsigned char *) &var+3)
#define BYTE2(var) *((unsigned char *) &var+2)
#define BYTE1(var) *((unsigned char *) &var+1)
#define BYTE0(var) *((unsigned char *) &var)

//TWI address of the TMC222
//(when only one TMC222 is connected zero can be used which
// always works (it is the broadcast address)).
#define TWI_ADDR 0x00


//Variables needed by this library to handle the TWI communication
static volatile unsigned char TWIMasterSendBuffer[9];
static volatile unsigned char TWIMasterReceiveBuffer[9];
static volatile unsigned char TWIMasterSendPtr;
static volatile unsigned char TWIMasterReceivePtr;
static volatile unsigned char TWIMasterSendCount;
static volatile unsigned char TWIMasterReceiveCount;
static volatile unsigned char TWIInUse;
static volatile unsigned char TWIErrorFlag;


//TWI interrupt handler
//Handles all states of the TWI of the AVR that can occur in master mode
SIGNAL(SIG_2WIRE_SERIAL)
{
  if(TWSR==0x08 || TWSR==0x10)  //Start condition or repeated start condition transmitted by us => we are becoming bus master!
  {
    TWDR=TWIMasterSendBuffer[0];   //Load the address byte ("SLA+W" or "SLA+R")
    TWCR=0x85;                     //Generate start condition and send the address byte
    TWIMasterSendPtr=1;
  }
  else if(TWSR==0x18 || TWSR==0x20)  //Address byte ("SLA+W") has been transmitted => send our first data byte.
  {
    TWDR=TWIMasterSendBuffer[TWIMasterSendPtr++];
    TWCR=0x85;
  }
  else if(TWSR==0x28 || TWSR==0x30)  //We can send all the other data bytes.
  {
    if(TWIMasterSendPtr<TWIMasterSendCount)     //more data bytes to be sent
    {
      TWDR=TWIMasterSendBuffer[TWIMasterSendPtr++];
      TWCR=0xC5;   //send next data byte
    }
    else
    {
      TWCR=0xD5;   //No more data bytes to be sent => Generate stop condition to release the bus
      TWIInUse=FALSE;
    }
  }
  else if(TWSR==0x38) //Bus arbitration lost => release bus immediately
  {
    TWIErrorFlag=TRUE;
    TWIInUse=FALSE;
    TWCR=0xC5;
  }
  else if(TWSR==0x40)  //Address byte ("SLA+R") has been transmitted and ACK has been received => we can now start to receive data bytes
  {
    TWIMasterReceivePtr=0;
    TWCR=0xC5;  //request first data byte
  }
  else if(TWSR==0x48)  //Address byte ("SLA+R") has been transmitted and NOT ACK has been received => abort the read process
  {
    TWIErrorFlag=TRUE;
    TWCR=0xd5;  //generate stop condition and release the bus  0x95
    TWIInUse=FALSE;
  }
  else if(TWSR==0x50)  //a data byte has been read from the slave and acknowledged
  {
    TWIMasterReceiveBuffer[TWIMasterReceivePtr++]=TWDR;
    if(TWIMasterReceivePtr<TWIMasterReceiveCount)
      TWCR=0xC5;  //at least two more data bytes to get => send ACK
    else
      TWCR=0x85;  //only one more data byte to get => send NOT ACK
  }
  else if(TWSR==0x58)  //the last data byte has been read from the slave
  {
    TWIMasterReceiveBuffer[TWIMasterReceivePtr++]=TWDR;
    TWCR=0xD5;   //generate a stop condition and release the bus  0x95
    TWIInUse=FALSE;
  }
  else  //Unknown state, just reset the interrupt flag.
  {
    TWIErrorFlag=TRUE;
    TWIInUse=FALSE;
    TWCR = 0xC5;  //Acknowledge and reset the interrupt flag
  }
}



/*****************************************************************************
   Function: InitTWI()
   Parameters: ---
   Return value: ---

   Purpose: Initialization of the TWI. This function must be called prior to 
            using any other function of this library.
            Furthermore, interrupts must be activated before using any other
            function of this library (with GCC this is can be done using
            sei() ).
******************************************************************************/
void InitTWI(void)
{
  TWBR=64;    //TWI bit rate; do not set below 64 for use with TMC222 and 16MHz clock frequency
  TWCR=0xC5;  //Switch on TWI using interrupts
  
  TWIErrorFlag=FALSE;
  TWIInUse=FALSE;
}


/*****************************************************************************
   Function: GetFullStatus1()
   Parameters: *TMC222Status: Pointer to variable that gets the TMC222 status 
                              information.

   Return value: ---

   Zweck: Read out the status information of the TMC222.
          It is also possible to set the parameter to NULL, when only the 
          error flags are to be reset without the need to know the status.
******************************************************************************/
void GetFullStatus1(TTMC222Status *TMC222Status)
{
	unsigned int i;
	
	while(TWIInUse);              //Wait until TWI is free

  //Send command byte to the TMC222
	TWIMasterSendBuffer[0]=TWI_ADDR;  //Address byte
  TWIMasterSendBuffer[1]=0x81;  //Command byte for GetFullStatus1
  TWIMasterSendCount=2;
  TWIInUse=TRUE;
  TWCR=0xA5;                    //Start TWI communication
  while(TWIInUse);              //and wait until done.
  
  //Read 8 bytes from the TMC222
  TWIMasterSendBuffer[0]=TWI_ADDR|0x01;  //Address byte (with read bit set)
  TWIMasterReceiveCount=8;      //8 bytes are to be read
  TWIInUse=TRUE;
  TWCR=0xA5;                    //Start TWI communication
  while(TWIInUse);              //and wait until done.

  //Copy the data to the TMC222Status structure (if pointer not NULL)
  if(TMC222Status!=NULL)
    for(i=0; i<5; i++)
      *(((unsigned char *) TMC222Status)+i)=TWIMasterReceiveBuffer[i+1];    
}


/*****************************************************************************
   Function: GetFullStatus2()
   Parameters: *TargetPosition: Pointer to variable for the target position
                                or NULL.
              *SecurePosition: Pointer to variable for the secure position
                               or NULL.

   Return value: actual position

   Purpose: Read all TMC222 position registers (using the GetFullStatus2 command).
            NULL can be used for values that are not needed.
******************************************************************************/
int GetFullStatus2(int *TargetPosition, int *SecurePosition)
{
	while(TWIInUse);              //Wait until TWI is free

  //Send GetFullStatus2 command (0xfc) to the TMC222
	TWIMasterSendBuffer[0]=TWI_ADDR;  //Address byte
  TWIMasterSendBuffer[1]=0xfc;  //Command byte for GetFullStatus2
  TWIMasterSendCount=2;
  TWIInUse=TRUE;
  TWCR=0xA5;                    //start TWI communication
  while(TWIInUse);              //and wait until done.

  //Read back 8 bytes from the TMC222
  TWIMasterSendBuffer[0]=TWI_ADDR|0x01;  //Address byte (with read bit set)
  TWIMasterReceiveCount=8;      //8 bytes are to be read
  TWIInUse=TRUE;
  TWCR=0xA5;                    //Start TWI communication
  while(TWIInUse);              //and wait until done.

  //Return the position values
  if(TargetPosition!=NULL) *TargetPosition=(TWIMasterReceiveBuffer[3]<<8) | TWIMasterReceiveBuffer[4];
  if(SecurePosition!=NULL) *SecurePosition=((TWIMasterReceiveBuffer[6] & 0x06)<<8) | TWIMasterReceiveBuffer[5];
  return (TWIMasterReceiveBuffer[1]<<8) | TWIMasterReceiveBuffer[2];
}


/*****************************************************************************
   Function: SetMotorParameters()
   Parameters: *TMC222Parameters: Pointer to TTMC222Parameters structure
                                  containing all parameters.

   Return value: ---

   Purpose: Set the TMC222 motor parameters.
******************************************************************************/
void SetMotorParameters(TTMC222Parameters *TMC222Parameters)
{
	unsigned char i;
	
	while(TWIInUse);              //Wait until TWI is free

  //Send command 0x89 with the data
  TWIMasterSendBuffer[0]=TWI_ADDR;  //Address byte
  TWIMasterSendBuffer[1]=0x89;  //Command byte for SetMotorParameters (0x89)
  TWIMasterSendBuffer[2]=0xff;
  TWIMasterSendBuffer[3]=0xff;
  
  for(i=0; i<5; i++)
    TWIMasterSendBuffer[i+4]=*(((unsigned char *) TMC222Parameters)+i);

  TWIMasterSendCount=9;
  TWIInUse=TRUE;
  TWCR=0xA5;                    //Start TWI communication
}


/*****************************************************************************
   Function: SetPosition()
   Parameters: Position: Target position

   Return value: ---

   Purpose: Set the target position. This makes the motor move to the new
            target position (if the motor parameters have been set properly).
******************************************************************************/
void SetPosition(int Position)
{
	while(TWIInUse);              //Wait until TWI is free

  //Send command 0x8b with the data
  TWIMasterSendBuffer[0]=TWI_ADDR;  //Address byte
  TWIMasterSendBuffer[1]=0x8b;  //Command byte for SetPosition (0x8b)
  TWIMasterSendBuffer[2]=0xff;
  TWIMasterSendBuffer[3]=0xff;
  TWIMasterSendBuffer[4]=Position >> 8;
  TWIMasterSendBuffer[5]=Position & 0xff;
  TWIMasterSendCount=6;
  TWIInUse=TRUE;
  TWCR=0xA5;                    //Start TWI communication
}


/*****************************************************************************
   Function: ResetPosition()
   Parameters: ---

   Return value: ---

   Purpose: Set actual position and target position to zero (without moving the
            motor).
******************************************************************************/
void ResetPosition(void)
{
	while(TWIInUse);              //Wait until TWI is free
 
  TWIMasterSendBuffer[0]=TWI_ADDR;  //Address byte
  TWIMasterSendBuffer[1]=0x86;  //Command byte for ResetPosition (0x86)
  TWIMasterSendCount=2;
  TWIInUse=TRUE;
  TWCR=0xA5;                    //Start TWI communication
}


/*****************************************************************************
   Function: GotoSecurePosition()
   Parameters: ---

   Return value: ---

   Purpose: Drive the motor to the secure position (that has been set using 
   the SetMotorParameters command).
******************************************************************************/
void GotoSecurePosition(void)
{
	while(TWIInUse);              //Wait until TWI is free

  TWIMasterSendBuffer[0]=TWI_ADDR;  //Address byte
  TWIMasterSendBuffer[1]=0x84;  //Command byte for GotoSecurePosition (0x84)
  TWIMasterSendCount=2;
  TWIInUse=TRUE;
  TWCR=0xA5;                    //Start TWI communication
}


/*****************************************************************************
   Function: ResetToDefault()
   Parameters: ---

   Return value: ---

   Purpose: Reset all values to the default values (from the OTP).
******************************************************************************/
void ResetToDefault(void)
{
	while(TWIInUse);              //Wait until TWI is free

  TWIMasterSendBuffer[0]=TWI_ADDR;  //Address byte
  TWIMasterSendBuffer[1]=0x87;  //Command byte for ResetToDefault (0x87)
  TWIMasterSendCount=2;
  TWIInUse=TRUE;
  TWCR=0xA5;                    //Start TWI communication
}


/*****************************************************************************
   Function: RunInit()
   Parameters: VMin: Velocity 1 (0..15)
               VMax: Velocity 2 (0..15)
               Position1: Position 1
               Position2: Position 2

   Return value: ---

   Purpose: RunInit command (reference search) of the TMC222 (please see the
            TMC222 data sheet for details).
******************************************************************************/
void RunInit(unsigned char VMin, unsigned char VMax, int Position1, int Position2)
{
	while(TWIInUse);              //Wait until TWI is free

  //Send RunInit command and the data
  TWIMasterSendBuffer[0]=TWI_ADDR;  //Address byte
  TWIMasterSendBuffer[1]=0x88;  //Command byte for RunInit (0x88)
  TWIMasterSendBuffer[2]=0xff;
  TWIMasterSendBuffer[3]=0xff;
  TWIMasterSendBuffer[4]=(VMax << 4) | (VMin & 0x0f);
  TWIMasterSendBuffer[5]=BYTE1(Position1);
  TWIMasterSendBuffer[6]=BYTE0(Position1);
  TWIMasterSendBuffer[7]=BYTE1(Position2);
  TWIMasterSendBuffer[8]=BYTE0(Position2);
  TWIMasterSendCount=9;
  TWIInUse=TRUE;
  TWCR=0xA5;                    //Start TWI communication
}


/*****************************************************************************
   Function: HardStop()
   Parameterss: ---

   Return value: ---

   Purpose: Hard stop of the motor. 
            Afterwards the error flags have to be reset using a GetFullStatus1
            command, as otherwise the motor will not run any more.
            (HardStop sets some error flags in the TMC222 that prevent running
             until they have been reset).
******************************************************************************/
void HardStop(void)
{
	while(TWIInUse);              //Wait until TWI is free

  TWIMasterSendBuffer[0]=TWI_ADDR;  //Address byte
  TWIMasterSendBuffer[1]=0x85;  //Command byte for HardStop (0x85)
  TWIMasterSendCount=2;
  TWIInUse=TRUE;
  TWCR=0xA5;                    //Start TWI communication
}


/*****************************************************************************
   Function: SoftStop()
   Parameters: ---

   Return value: ---

   Purpose: Stop the motor using the normal decelaration.
******************************************************************************/
void SoftStop(void)
{
	while(TWIInUse);              //Wait until TWI is free

  TWIMasterSendBuffer[0]=TWI_ADDR;  //Address byte
  TWIMasterSendBuffer[1]=0x8f;  //Command byte for SoftStop (0x8f)
  TWIMasterSendCount=2;
  TWIInUse=TRUE;
  TWCR=0xA5;                    //Start TWI communication
}


/*****************************************************************************
   Function: GetMotorParameters()
   Parameters TMC222Parameters: Pointer to data structure that will contain
              all parameters read from the TMC222.

   Return value: ---

   Purpose: This function uses a GetFullStatus1 ccommand and a GetFullStatus2
            command to read out motor parameters into a TTTMC222Parmaters
            data structure.
            After that, the desired values can be modified and the same data
            structure can then be used wit SetMotorParameters again.
******************************************************************************/
void GetMotorParameters(TTMC222Parameters *TMC222Parameters)
{
	TTMC222Status TMC222Status;
	int SecurePosition;
	
	GetFullStatus1(&TMC222Status);
	
	TMC222Parameters->IRun=TMC222Status.IRun;
	TMC222Parameters->IHold=TMC222Status.IHold;
	TMC222Parameters->VMin=TMC222Status.VMin;
	TMC222Parameters->VMax=TMC222Status.VMax;
	TMC222Parameters->Acc=TMC222Status.Acc;
	TMC222Parameters->AccShape=TMC222Status.AccShape;
	TMC222Parameters->Shaft=TMC222Status.Shaft;
	TMC222Parameters->StepMode=TMC222Status.StepMode;
	
	GetFullStatus2(NULL, &SecurePosition);
	TMC222Parameters->SecPosHi=BYTE1(SecurePosition);
	TMC222Parameters->SecPosLo=BYTE0(SecurePosition);
}

