#include "CPU.h"
#include <iostream>

CPU::CPU()
{
	// initialize memory
	memory.init();
}

CPU::~CPU()
{
}

// reset
void CPU::reset()
{
	// reset registers
	registers.A = 0x00;
	registers.X = 0x00;
	registers.Y = 0x00;
	registers.S = 0x00;
	registers.PC = 0x00;
	registers.P = 0x00;
	registers.flags.C = 0;
	registers.flags.Z = 0;
	registers.flags.I = 0;
	registers.flags.D = 0;
	registers.flags.B = 0;
	registers.flags.U = 0;
	registers.flags.V = 0;
	registers.flags.N = 0;

	// reset memory
	memory.init();
}

// load ROM file into EEPROM
void CPU::loadROM(std::string path)
{
	// open the file using fopen_s
	FILE* file;
	fopen_s(&file, path.c_str(), "rb");
	
	// check if the file is open
	if (file == NULL) {
		std::cout << "Error: Could not open file: " << path << std::endl;
		return;
	}
	
	// write the file into eeprom one byte at a time
	unsigned char byte;
	unsigned short address = 0x0000;
	while (fread(&byte, sizeof(byte), 1, file)) {
		eeprom.write(address++, byte);
	}

	// close the file
	fclose(file);
}

// print registers with the status flags
void CPU::printRegisters()
{
	printf("A: %02X X: %02X Y: %02X S: %02X PC: %04X P: %02X\n", registers.A, registers.X, registers.Y, registers.S, registers.PC, registers.P);
	printf("Status Flags: C: %d Z: %d I: %d D: %d B: %d U: %d V: %d N: %d\n", registers.flags.C, registers.flags.Z, registers.flags.I, registers.flags.D, registers.flags.B, registers.flags.U, registers.flags.V, registers.flags.N);
}