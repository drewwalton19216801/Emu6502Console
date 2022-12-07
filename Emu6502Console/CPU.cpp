#include "CPU.h"
#include <iostream>
#include <assert.h>

CPU::CPU()
{
	// new memory object
	Memory* memory = new Memory();
	// initialize memory
	memory->init();
}

CPU::~CPU()
{
}

Byte CPU::FetchByte(s32& cycles, Memory& memory)
{
	Byte data = memory.read(registers.PC);
	registers.PC++;
	cycles--;
	return data;
}

SByte CPU::FetchSByte(s32& cycles, Memory& memory)
{
	SByte data = memory.read(registers.PC);
	registers.PC++;
	cycles--;
	return data;
}

Word CPU::FetchWord(s32& cycles, Memory& memory)
{
	Word data = memory.read(registers.PC);
	registers.PC++;
	cycles--;
	data |= (memory.read(registers.PC) << 8);
	registers.PC++;
	cycles--;
	return data;
}

Byte CPU::ReadByte(s32& cycles, Word address, Memory& memory)
{
	Byte data = memory.read(address);
	cycles--;
	return data;
}

Word CPU::ReadWord(s32& cycles, Word address, Memory& memory)
{
	Byte LoByte = ReadByte(cycles, address, memory);
	Byte HiByte = ReadByte(cycles, address + 1, memory);
	return LoByte | (HiByte << 8);
}

void CPU::WriteByte(s32& cycles, Word address, Byte value, Memory& memory)
{
	memory.write(address, value);
	cycles--;
}

void CPU::WriteWord(s32& cycles, Word address, Word value, Memory& memory)
{
	WriteByte(cycles, address, value & 0xFF, memory);
	WriteByte(cycles, address + 1, value >> 8, memory);
}

Word CPU::SPToAddress()
{
	return 0x0100 | registers.SP;
}

void CPU::PushWord(s32& cycles, Word value, Memory& memory)
{
	WriteByte(cycles, SPToAddress(), value >> 8, memory);
	registers.SP--;
	WriteByte(cycles, SPToAddress(), value & 0xFF, memory);
	registers.SP--;
}

void CPU::PushPC(s32& cycles, Memory& memory)
{
	PushWord(cycles, registers.PC, memory);
}

void CPU::PushPCMinusOne(s32& cycles, Memory& memory)
{
	PushWord(cycles, registers.PC - 1, memory);
}

void CPU::PushPCPlusOne(s32& cycles, Memory& memory)
{
	PushWord(cycles, registers.PC + 1, memory);
}

void CPU::PushByte(s32& cycles, Byte value, Memory& memory)
{
	WriteByte(cycles, SPToAddress(), value, memory);
	registers.SP--;
}

Byte CPU::PopByte(s32& cycles, Memory& memory)
{
	registers.SP++;
	return ReadByte(cycles, SPToAddress(), memory);
}

Word CPU::PopWord(s32& cycles, Memory& memory)
{
	Word ValueFromStack = ReadWord(cycles, SPToAddress(), memory);
	registers.SP += 2;
	return ValueFromStack;
}

void CPU::SetZNFlags(Byte reg)
{
	status.Z = (reg == 0);
	status.N = (reg & NegativeFlagBit) > 0;
}

Word CPU::LoadPrg(const Byte* prg, u32 NumBytes, Memory& memory)
{
	Word LoadAddress = 0;
	if (prg && NumBytes > 2)
	{
		u32 At = 0;
		const Word Lo = prg[At++];
		const Word Hi = prg[At++] << 8;
		LoadAddress = Lo | Hi;
		for (Word i = 0; i < NumBytes - 2; i++)
		{
			memory.write(LoadAddress + i, prg[At++]);
		}
	}
	return LoadAddress;
}

// reset
void CPU::reset(Memory& memory)
{
	reset(0xFFFC, memory);
}

// reset to reset vector
void CPU::reset(Word ResetVector, Memory& memory)
{
	// reset registers
	registers.A = 0x00;
	registers.X = 0x00;
	registers.Y = 0x00;
	registers.SP = 0xFF;
	registers.PC = ResetVector;
	// reset flags
	status.C = 0;
	status.Z = 0;
	status.I = 0;
	status.D = 0;
	status.B = 0;
	status.V = 0;
	status.N = 0;

	// reset memory
	memory.init();
}

// load ROM file into EEPROM
void CPU::loadROM(std::string path, Memory& memory)
{
	// open the file using fopen_s
	FILE* file;
	fopen_s(&file, path.c_str(), "rb");
	
	// check if the file is open
	if (file == NULL) {
		std::cout << "Error: Could not open file: " << path << std::endl;
		return;
	}
	
	// write the file into memory one byte at a time
	Byte byte;
	Word address = 0x0000;
	while (fread(&byte, sizeof(byte), 1, file)) {
		memory.write(address++, byte);
	}

	// close the file
	fclose(file);
}

// print status
void CPU::printStatus() const
{
	// print CPU status
	std::cout << "CPU Status:" << std::endl;
	std::cout << "A: " << (int)registers.A << std::endl;
	std::cout << "X: " << (int)registers.X << std::endl;
	std::cout << "Y: " << (int)registers.Y << std::endl;
	std::cout << "PC: " << (int)registers.PC << std::endl;
	std::cout << "SP: " << (int)registers.SP << std::endl;
	std::cout << "C: " << (int)status.C << std::endl;
	std::cout << "Z: " << (int)status.Z << std::endl;
	std::cout << "I: " << (int)status.I << std::endl;
	std::cout << "D: " << (int)status.D << std::endl;
	std::cout << "B: " << (int)status.B << std::endl;
	std::cout << "V: " << (int)status.V << std::endl;
	std::cout << "N: " << (int)status.N << std::endl;
}

s32 CPU::execute(s32 cycles, Memory& memory)
{
	// load a register with the value from the memory address
	auto LoadRegister = [&cycles, &memory, this] (Word Address, Byte& Register)
	{
		Register = ReadByte(cycles, Address, memory);
		SetZNFlags(Register);
	};

	// AND the A register with the value from the memory address
	auto AND = [&cycles, &memory, this](Word Address)
	{
		registers.A &= ReadByte(cycles, Address, memory);
		SetZNFlags(registers.A);
	};

	// OR the A register with the value from the memory address
	auto ORA = [&cycles, &memory, this](Word Address)
	{
		registers.A |= ReadByte(cycles, Address, memory);
		SetZNFlags(registers.A);
	};

	// XOR the A register with the value from the memory address
	auto EOR = [&cycles, &memory, this](Word Address)
	{
		registers.A ^= ReadByte(cycles, Address, memory);
		SetZNFlags(registers.A);
	};
	
	// conditional branch
	auto BranchIf = [&cycles, &memory, this](bool Test, bool Expected)
	{
		SByte Offset = FetchSByte(cycles, memory);
		if (Test == Expected)
		{
			const Word PCOld = registers.PC;
			registers.PC += Offset;
			cycles--;

			const bool PageChanged = (registers.PC >> 8) != (PCOld >> 8);
			if (PageChanged)
			{
				cycles--;
			}
		}
	};

	// Add with carry given the operand
	auto ADC = [&cycles, &memory, this](Byte Operand)
	{
		// decimal mode not supported
		if (status.D)
		{
			std::cout << "Error: Decimal mode not supported" << std::endl;
			return;
		}
		const bool AreSignBitsTheSame = !((registers.A ^ Operand) & NegativeFlagBit);
		Word Sum = registers.A;
		Sum += Operand;
		Sum += status.C;
		registers.A = (Sum & 0xFF);
		SetZNFlags(registers.A);
		status.C = Sum > 0xFF;
		status.V = AreSignBitsTheSame && ((registers.A ^ Operand) & NegativeFlagBit);
	};

	// subtract with carry
	auto SBC = [&ADC](Byte Operand)
	{
		ADC(~Operand);
	};

	// set CPU status for a CMP/CPX/CPY operation
	auto RegisterCompare = [&cycles, &memory, this](Byte Operand, Byte RegisterValue)
	{
		const Byte Difference = RegisterValue - Operand;
		status.N = (Difference & NegativeFlagBit) > 0;
		status.Z = RegisterValue == Operand;
		status.C = RegisterValue >= Operand;
	};

	// arithmetic shift left
	auto ASL = [&cycles, &memory, this](Byte Operand) -> Byte
	{
		status.C = (Operand & NegativeFlagBit) > 0;
		Byte Result = Operand << 1;
		SetZNFlags(Result);
		cycles--;
		return Result;
	};

	// logical shift right
	auto LSR = [&cycles, this](Byte Operand) -> Byte
	{
		status.C = (Operand & NegativeFlagBit) > 0;
		Byte Result = Operand >> 1;
		SetZNFlags(Result);
		cycles--;
		return Result;
	};

	// rotate left
	auto ROL = [&cycles, this](Byte Operand) -> Byte
	{
		const bool Carry = status.C;
		status.C = (Operand & NegativeFlagBit) > 0;
		Byte Result = Operand << 1;
		Result |= Carry;
		SetZNFlags(Result);
		cycles--;
		return Result;
	};

	// rotate right
	auto ROR = [&cycles, this](Byte Operand) -> Byte
	{
		const bool Carry = status.C;
		status.C = (Operand & NegativeFlagBit) > 0;
		Byte Result = Operand >> 1;
		Result |= Carry << 7;
		SetZNFlags(Result);
		cycles--;
		return Result;
	};

	// push status onto the stack, setting bits 4 & 5 on the stack
	auto PushStatus = [&cycles, &memory, this]()
	{
		Byte PSStack = PS | BreakFlagBit | UnusedFlagBit;
		PushByte(cycles, PSStack, memory);
	};

	// pop CPU status from the stack, clearing bits 4 & 5 (break & unused)
	auto PopStatus = [&cycles, &memory, this]()
	{
		PS = PopByte(cycles, memory);
		status.B = false;
		status.U = false;
	};

	const s32 CyclesRequested = cycles;
	while (cycles > 0)
	{
		Byte Instruction = FetchByte(cycles, memory);

		switch (Instruction)
		{
		case INS_JMP_ABS:
		{
			Word Address = AddrMode_ABS(cycles, memory);
			registers.PC = Address;
		}
		case INS_JMP_IND:
		{
			Word Address = AddrMode_ABS(cycles, memory);
			registers.PC = ReadWord(cycles, Address, memory);
			break;
		}
		case INS_LDA_ABS:
		{
			Word Address = AddrMode_ABS(cycles, memory);
			LoadRegister(Address, registers.A);
			break;
		}
		case INS_LDA_ABSX:
		{
			Word Address = AddrMode_ABSX(cycles, memory);
			LoadRegister(Address, registers.A);
			break;
		}
		case INS_LDA_ABSY:
		{
			Word Address = AddrMode_ABSY(cycles, memory);
			LoadRegister(Address, registers.A);
			break;
		}
		case INS_LDA_IM:
		{
			registers.A = FetchByte(cycles, memory);
			SetZNFlags(registers.A);
			break;
		}
		case INS_LDA_INDX:
		{
			Word Address = AddrMode_INDX(cycles, memory);
			LoadRegister(Address, registers.A);
			break;
		}
		case INS_LDA_INDY:
		{
			Word Address = AddrMode_INDY(cycles, memory);
			LoadRegister(Address, registers.A);
			break;
		}
		case INS_LDA_ZP:
		{
			Word Address = AddrMode_ZP(cycles, memory);
			LoadRegister(Address, registers.A);
			break;
		}
		case INS_LDA_ZPX:
		{
			Word Address = AddrMode_ZPX(cycles, memory);
			LoadRegister(Address, registers.A);
			break;
		}
		case INS_ROR:
		{
			registers.A = ROR(registers.A);
			break;
		}
		case INS_ROR_ABS:
		{
			Word Address = AddrMode_ABS(cycles, memory);
			Byte Operand = ReadByte(cycles, Address, memory);
			Byte Result = ROR(Operand);
			WriteByte(cycles, Address, Result, memory);
			break;
		}
		case INS_ROR_ABSX:
		{
			Word Address = AddrMode_ABSX(cycles, memory);
			Byte Operand = ReadByte(cycles, Address, memory);
			Byte Result = ROR(Operand);
			WriteByte(cycles, Address, Result, memory);
			break;
		}
		case INS_ROR_ZP:
		{
			Word Address = AddrMode_ZP(cycles, memory);
			Byte Operand = ReadByte(cycles, Address, memory);
			Byte Result = ROR(Operand);
			WriteByte(cycles, Address, Result, memory);
			break;
		}
		case INS_ROR_ZPX:
		{
			Word Address = AddrMode_ZPX(cycles, memory);
			Byte Operand = ReadByte(cycles, Address, memory);
			Byte Result = ROR(Operand);
			WriteByte(cycles, Address, Result, memory);
			break;
		}
		case INS_STA_ABS:
		{
			Word Address = AddrMode_ABS(cycles, memory);
			WriteByte(cycles, Address, registers.A, memory);
			break;
		}
		case INS_STA_ABSX:
		{
			Word Address = AddrMode_ABSX(cycles, memory);
			WriteByte(cycles, Address, registers.A, memory);
			break;
		}
		case INS_STA_ABSY:
		{
			Word Address = AddrMode_ABSY(cycles, memory);
			WriteByte(cycles, Address, registers.A, memory);
			break;
		}
		case INS_STA_INDX:
		{
			Word Address = AddrMode_INDX(cycles, memory);
			WriteByte(cycles, Address, registers.A, memory);
			break;
		}
		case INS_STA_INDY:
		{
			Word Address = AddrMode_INDY(cycles, memory);
			WriteByte(cycles, Address, registers.A, memory);
			break;
		}
		case INS_STA_ZP:
		{
			Word Address = AddrMode_ZP(cycles, memory);
			WriteByte(cycles, Address, registers.A, memory);
			break;
		}
		case INS_STA_ZPX:
		{
			Word Address = AddrMode_ZPX(cycles, memory);
			WriteByte(cycles, Address, registers.A, memory);
			break;
		}
		default:
			std::cout << "Error: Unknown instruction: " << std::hex << (int)Instruction << std::endl;
			break;
		}
	}

	const s32 NumCyclesUsed = CyclesRequested - cycles;
	return NumCyclesUsed;
}

Word CPU::AddrMode_ZP(s32& cycles, Memory& memory)
{
	Byte ZPAddress = FetchByte(cycles, memory);
	return ZPAddress;
}

Word CPU::AddrMode_ZPX(s32& cycles, Memory& memory)
{
	Byte ZPAddress = FetchByte(cycles, memory);
	ZPAddress += registers.X;
	cycles--;
	return ZPAddress;
}

Word CPU::AddrMode_ZPY(s32& cycles, Memory& memory)
{
	Byte ZPAddress = FetchByte(cycles, memory);
	ZPAddress += registers.Y;
	cycles--;
	return ZPAddress;
}

Word CPU::AddrMode_ABS(s32& cycles, Memory& memory)
{
	Word ABSAddress = FetchWord(cycles, memory);
	return ABSAddress;
}

Word CPU::AddrMode_ABSX(s32& cycles, Memory& memory)
{
	Word ABSAddress = FetchWord(cycles, memory);
	Word ABSAddressX = ABSAddress + registers.X;
	const bool CrossedPageBoundary = (ABSAddress ^ ABSAddressX) >> 8;
	if (CrossedPageBoundary)
	{
		cycles--;
	}
	return ABSAddressX;
}

Word CPU::AddrMode_ABSX5(s32& cycles, Memory& memory)
{
	Word ABSAddress = FetchWord(cycles, memory);
	Word ABSAddressX = ABSAddress + registers.X;
	cycles--;
	return ABSAddressX;
}

Word CPU::AddrMode_ABSY(s32& cycles, Memory& memory)
{
	Word ABSAddress = FetchWord(cycles, memory);
	Word ABSAddressY = ABSAddress + registers.Y;
	const bool CrossedPageBoundary = (ABSAddress ^ ABSAddressY) >> 8;
	if (CrossedPageBoundary)
	{
		cycles--;
	}
	return ABSAddressY;
}

Word CPU::AddrMode_ABSY5(s32& cycles, Memory& memory)
{
	Word ABSAddress = FetchWord(cycles, memory);
	Word ABSAddressY = ABSAddress + registers.Y;
	cycles--;
	return ABSAddressY;
}

Word CPU::AddrMode_INDX(s32& cycles, Memory& memory)
{
	Byte ZPAddress = FetchByte(cycles, memory);
	ZPAddress += registers.X;
	cycles--;
	Word EffectiveAddress = ReadWord(cycles, ZPAddress, memory);
	return EffectiveAddress;
}

Word CPU::AddrMode_INDY(s32& cycles, Memory& memory)
{
	Byte ZPAddress = FetchByte(cycles, memory);
	Word EffectiveAddress = ReadWord(cycles, ZPAddress, memory);
	Word EffectiveAddressY = EffectiveAddress + registers.Y;
	const bool CrossedPageBoundary = (EffectiveAddress ^ EffectiveAddressY) >> 8;
	if (CrossedPageBoundary)
	{
		cycles--;
	}
	return EffectiveAddressY;
}

Word CPU::AddrMode_INDY6(s32& cycles, Memory& memory)
{
	Byte ZPAddress = FetchByte(cycles, memory);
	Word EffectiveAddress = ReadWord(cycles, ZPAddress, memory);
	Word EffectiveAddressY = EffectiveAddress + registers.Y;
	cycles--;
	return EffectiveAddressY;
}
