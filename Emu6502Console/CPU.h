/**
* Class name: CPU
* Purpose: Implement a 6502 CPU
**/
#pragma once

#include "EEPROM.h"
#include "Memory.h"

class CPU
{
	/**
	* CPU Registers:
	*	- 8-bit Accumulator (A)
	*	- 8-bit X Register (X)
	*	- 8-bit Y Register (Y)
	*	- 8-bit Stack Pointer (S)
	*	- 16-bit Program Counter (PC)
	*	- 8-bit Status Register (P)
	**/
public:
	// constructor
	CPU();
	
	// destructor
	~CPU();

	// Accumulator
	unsigned char A;

	// X Register
	unsigned char X;

	// Y Register
	unsigned char Y;

	// Stack Pointer
	unsigned char S;
	
	// Program Counter
	unsigned short PC;

	// Status Register
	unsigned char P;

	// 32K SRAM
	Memory* memory;
};

