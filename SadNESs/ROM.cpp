#include "stdafx.h"
#include "ROM.h"
#include "FileLoader.h"
#include "RAM.h"

byte *loadedROM;
byte mapper = 0x0;
byte PRG_size;
byte CHR_size;
byte RAM_size;	//0 = 1
byte ROM_region;	//0 NTSC; 1 PAL

void loadInMemory() {
	if (mapper == 0x0) {
		for (int i = 0; i < fileLenght; i++) {
			memory[0x8000 + i] = loadedROM[i + 0xF + 1];
		}
	}
}

void ROM::Setup() {
	PRG_size = loadedROM[4] & 0x255;
	CHR_size = loadedROM[5] & 0x255;
	RAM_size = loadedROM[8] & 0x255;
	ROM_region = (loadedROM[9] >> 7);

	//set mapper
	loadInMemory();
}