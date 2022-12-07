/**
* Class name: EEPROM
* Purpose: Implements a 28C256 32K EEPROM 
**/
#pragma once
class EEPROM
{
public:
	// constructor
	EEPROM();
	// destructor
	~EEPROM();
	
	// 32K EEPROM
	unsigned char memory[32768];
	
	// read a byte from the EEPROM
	unsigned char readByte(unsigned short address);
	
	// write a byte to the EEPROM
	void writeByte(unsigned short address, unsigned char data);

private:
	// the EEPROM file name
	const char* fileName = "EEPROM.bin";

	// load the EEPROM data from a file
	void loadRom();

	// save the EEPROM data to a file
	void saveRom();
};

