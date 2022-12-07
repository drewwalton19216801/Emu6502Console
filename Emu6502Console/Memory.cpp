#include "Memory.h"

// constructor
Memory::Memory()
{
	// zero out the memory
	for (int i = 0; i < 32768; i++)
	{
		memory[i] = 0;
	}
}

// destructor
Memory::~Memory()
{
	// free the memory
	delete[] memory;
}

unsigned char Memory::readByte(unsigned short address)
{
	// return the byte at the address
	return memory[address];
}

void Memory::writeByte(unsigned short address, unsigned char data)
{
	// write the byte to the address
	memory[address] = data;
}
