// Emu6502Console.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "CPU.h"

int main()
{
    // new Processor
	CPU* cpu = new CPU();
	// initialize memory
	cpu->memory.init();
	// reset
	cpu->reset(cpu->memory);

	// ask the user for the path to the ROM file
	std::string path;
	std::cout << "Enter the path to the ROM file: ";
	std::cin >> path;
	
	// load the ROM file
	cpu->loadROM(path, cpu->memory);

	// a run loop
	while (true)
	{
		// ask the user to press enter to execute the next instruction
		std::cout << "Press enter to execute the next instruction" << std::endl;
		std::cin.get();
		// execute the next instruction
		cpu->execute(1, cpu->memory);
		// print the status
		cpu->printStatus();
	}
	
	// delete the processor
	delete cpu;
}
