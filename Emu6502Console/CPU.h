/**
* Class name: CPU
* Purpose: Implement a 6502 CPU
**/
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string>
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

	struct StatusFlags
	{
		unsigned char C : 1; // Carry Flag
		unsigned char Z : 1; // Zero Flag
		unsigned char I : 1; // Interrupt Disable
		unsigned char D : 1; // Decimal Mode
		unsigned char B : 1; // Break Command
		unsigned char U : 1; // Unused
		unsigned char V : 1; // Overflow Flag
		unsigned char N : 1; // Negative Flag
	} flags;

	struct Registers
	{
		// 8-bit Accumulator (A)
		unsigned char A;
		// 8-bit X Register (X)
		unsigned char X;
		// 8-bit Y Register (Y)
		unsigned char Y;
		// 8-bit Stack Pointer (S)
		unsigned char S;
		// 16-bit Program Counter (PC)
		unsigned short PC;
		// 8-bit Status Register (P)
		unsigned char P;
		// status flags
		StatusFlags flags;
	} registers;
	
	struct Memory {
		static constexpr unsigned int MAX_MEM = 1024 * 64;
		unsigned char data[MAX_MEM];

		void init() {
			for (unsigned int i = 0; i < MAX_MEM; i++) {
				data[i] = 0x00;
			}
		}

		// read 1 byte
		unsigned char read(unsigned short address) {
			return data[address];
		}

		// write 1 byte
		void write(unsigned short address, unsigned char data) {
			this->data[address] = data;
		}
	};

	struct EEPROM {
		static constexpr unsigned int MAX_MEM = 1024 * 64;
		unsigned char data[MAX_MEM];

		void init() {
			for (unsigned int i = 0; i < MAX_MEM; i++) {
				data[i] = 0x00;
			}
		}

		// read 1 byte
		unsigned char read(unsigned short address) {
			return data[address];
		}

		// write 1 byte
		void write(unsigned short address, unsigned char data) {
			this->data[address] = data;
		}
	};
	
	// memory
	EEPROM eeprom;
	Memory memory;

	// reset CPU
	void reset();
	
	// load ROM file into EEPROM
	void loadROM(std::string path);
	
	// print registers
	void printRegisters();

	// print N bytes of memory
	void printMemory(unsigned short start, unsigned short end);

	// print N bytes of EEPROM
	void printEEPROM(unsigned short start, unsigned short end);
};

