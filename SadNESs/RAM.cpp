#include "stdafx.h"
#include "FileLoader.h"
#include "SadNESs.h"
#include "ROM.h"
#include "Debugger.h"

byte memory[0xFFFF];
//#0000 - 00FF = zero page
//#1000 - 10FF = stack

//non - maskable interrupt handler($FFFA / B)
//the power on reset location($FFFC / D)
//the BRK / interrupt request handler($FFFE / F)

void RAM::WriteByte(int address, byte value) {
	if (address >= 0x0 && address < 0x2000) {
		memory[address % 0x800] = value;
	}
	else if (address >= 0x2000 && address < 0x4000) {
		memory[0x2000 + address % 0x8] = value;
	}
	else if (address >= 0x4000 && address < 0x4020) {
		//deny
	}
	else if (address >= 0x8000 && address < 0xFFFA) {
		//deny
	}
}

byte RAM::ReadByte(int address) {
	if (address >= 0x0 && address < 0x2000) {
		return memory[address % 0x800];
	}
	else if (address >= 0x2000 && address < 0x4000) {
		return memory[0x2000 + address % 0x8];
	}
	else if (address >= 0x4000 && address < 0x4020) {
		Debugger::Log("I/O");
		return memory[address];
	}
	else if (address >= 0x8000 && address < 0xFFFF) {
		if (PRG_size == 0x1) {
			return memory[0x8000 + address % 0x4000];
		}
		else {
			return memory[address];
		}
	}
	else {
		isRunning = false;
		return 0;
	}
}

void RAM::WriteDWORD(int address, int value)
{
	if (address >= 0x0 && address < 0x2000) {
		memory[(address + 1) % 0x800] = value >> 8;
		memory[address % 0x800] = value & 0xFF;
	}
	else if (address >= 0x2000 && address < 0x4000) {
		memory[0x2000 + (address + 1) % 0x8] = value >> 8;
		memory[0x2000 + address % 0x8] = value & 0xFF;
	}
	else if (address >= 0x4000 && address < 0x4020) {
		//deny
	}
	else if (address >= 0x8000 && address < 0xFFFA) {
		//deny
	}
}

int RAM::ReadDWORD(int address)
{
	if (address >= 0x0 && address < 0x2000) {
		return ((memory[address + 1 % 0x800] << 8) | memory[address & 0x800]);
	}
	else if (address >= 0x2000 && address < 0x4000) {
		return ((memory[0x2000 + (address + 1) % 0x8] << 8) | memory[0x2000 + address & 0x8]);
	}
	else if (address >= 0x4000 && address < 0x4020) { //to remove?
		Debugger::Log("I/O"); 
		return ((memory[address + 1] << 8) | memory[address]);
	}
	else if (address >= 0x8000 && address < 0xFFFF) {
		if (PRG_size == 0x1) {
			return (memory[0x8000 + (address + 1) % 0x4000] << 8) | memory[0x8000 + address % 0x4000];
		}
		else {
			return ((memory[address + 1] << 8) | memory[address]);
		}
	}
	else {
		isRunning = false;
		return 0;
	}
}
