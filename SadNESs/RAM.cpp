#include "stdafx.h"
#include "FileLoader.h"
#include "SadNESs.h"
#include "ROM.h"

byte memory[0xFFFF];

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
	else if (address >= 0x8000 && address < 0xFFFF) {
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

void RAM::WriteDWORD(int, byte)
{
}

byte RAM::ReadDWORD(int)
{
	return byte();
}
