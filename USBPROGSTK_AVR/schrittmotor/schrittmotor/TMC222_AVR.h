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

//Data structure definition for the TMC222 status information.
//Use variables of this data type togehter with the GetFullStatus1() function.
typedef struct TMC222Sta
{
	unsigned char IHold: 4;
	unsigned char IRun: 4;
	unsigned char VMin: 4;
	unsigned char VMax: 4;
	unsigned char Acc: 4;
	unsigned char Shaft: 1;
	unsigned char StepMode: 2;
	unsigned char AccShape: 1;
	unsigned char TInfo: 2;
	unsigned char TW: 1;
	unsigned char TSD: 1;
	unsigned char UV2: 1;
	unsigned char ElDef: 1;
	unsigned char SteppLoss: 1;
	unsigned char VddReset: 1;
	unsigned char CPFail: 1;
	unsigned char NA: 1;
	unsigned char OVC2: 1;
	unsigned char OVC1: 1;
	unsigned char ESW: 1;
	unsigned char Motion: 3;
} TTMC222Status;

//Data structure definition for the TMC222 motor parameters.
//Use variables of this type together with the SetMotorParameters()
//and GetMotorParameters() functions.
typedef struct TMC222Par
{
	unsigned char IHold: 4;
	unsigned char IRun: 4;
	unsigned char VMin: 4;
	unsigned char VMax: 4;
	unsigned char Acc: 4;
  unsigned char Shaft: 1;
  unsigned char SecPosHi: 3;
  unsigned char SecPosLo: 8;	
  unsigned char NA1: 2;
  unsigned char StepMode: 2;
  unsigned char AccShape: 1;
  unsigned char NA2: 3;
} TTMC222Parameters;


void InitTWI(void);

void GetFullStatus1(TTMC222Status *TMC222Status);
int GetFullStatus2(int *TargetPosition, int *SecurePosition);
void SetMotorParameters(TTMC222Parameters *TMC222Parameters);
void SetPosition(int Position);
void ResetPosition(void);
void GotoSecurePosition(void);
void ResetToDefault(void);
void RunInit(unsigned char VMin, unsigned char VMax, int Position1, int Position2);
void HardStop(void);
void SoftStop(void);

void GetMotorParameters(TTMC222Parameters *TMC222Parameters);
