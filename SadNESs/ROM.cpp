#include "stdafx.h"
#include "ROM.h"
#include "FileLoader.h"
#include "RAM.h"

byte *loadedROM;
static char PRG_size;
static char CHR_size;
static char RAM_size;	//0 = 1
static char ROM_region;	//0 NTSC; 1 PAL

void loadInMemory() {
	for (int i = 0; i < fileLenght; i++) {
		RAM[0x8000 + i] = loadedROM[i + 0xF + 1];
	}
}

void ROM::Setup() {
	PRG_size = loadedROM[4] & 0x255;
	CHR_size = loadedROM[5] & 0x255;
	RAM_size = loadedROM[8] & 0x255;
	ROM_region = (loadedROM[9] >> 7);

	loadInMemory();
}