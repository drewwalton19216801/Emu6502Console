#include "EEPROM.h"
#include <fstream>

EEPROM::EEPROM()
{
	// zero out the memory
	for (int i = 0; i < 32768; i++)
	{
		memory[i] = 0;
	}
	
	// load the EEPROM data from a file
	loadRom();
}

EEPROM::~EEPROM()
{
	// free the memory
	delete[] memory;
}

unsigned char EEPROM::readByte(unsigned short address)
{
	return memory[address];
}

void EEPROM::writeByte(unsigned short address, unsigned char data)
{
	memory[address] = data;
}

void EEPROM::loadRom()
{
	// open the file
	std::ifstream file(fileName, std::ios::binary);

	// read the file
	file.read((char*)memory, 32768);

	// close the file
	file.close();
}

void EEPROM::saveRom()
{
	// open the file
	std::ofstream file(fileName, std::ios::binary);

	// write the file
	file.write((char*)memory, 32768);

	// close the file
	file.close();
}
