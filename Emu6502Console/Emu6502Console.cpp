// Emu6502Console.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "CPU.h"

int main()
{
    // new Processor
	CPU* cpu = new CPU();
    // reset
	cpu->reset();

	// ask the user for the path to the ROM file
	std::string path;
	std::cout << "Enter the path to the ROM file: ";
	std::cin >> path;
	
	// load the ROM file
	cpu->loadROM(path);

	// print registers
	cpu->printRegisters();

	// print the first 32 bytes of EEPROM
	std::cout << "EEPROM:" << std::endl;
	for (unsigned short i = 0; i < 32; i++) {
		std::cout << "0x" << std::hex << i << ": 0x" << std::hex << (int)cpu->eeprom.read(i) << std::endl;
	}
	
	// delete the processor
	delete cpu;
}
