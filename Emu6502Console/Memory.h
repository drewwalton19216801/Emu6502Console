/**
* Class name: Memory
* Purpose: Implements a 62256 32K SRAM
**/
#pragma once
class Memory
{
	// constructor
	Memory();

	// destructor
	~Memory();
	
	// 32K SRAM
	unsigned char memory[32768];

	// read a byte from the SRAM
	unsigned char readByte(unsigned short address);

	// write a byte to the SRAM
	void writeByte(unsigned short address, unsigned char data);
};

