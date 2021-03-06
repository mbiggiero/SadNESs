#include "stdafx.h"
#include "FileLoader.h"
#include "SadNESs.h"
#include "ROM.h"

FILE *filePointer;
int fileLenght;
extern bool debug;

void dumpRom() {
	int hexCount = 0;
	for (int i = 0; i < fileLenght; ++i) {

		printf("%.2x ", loadedROM[i] & 255);
		if (hexCount < 15) {
			hexCount++;
		}
		else {
			std::cout << std::endl;
			hexCount = 0;
		}
	}

	std::cout << std::endl;
}

void FileLoader::LoadRom(std::string romName)
{
	filePointer = fopen(romName.c_str(), "rb"); //TODO replace with something safe
	fseek(filePointer, 0, SEEK_END);
	fileLenght = ftell(filePointer);
	rewind(filePointer); 

	loadedROM = (byte *)malloc((fileLenght + 1) * sizeof(byte));
	fread(loadedROM, fileLenght, 1, filePointer);
	fclose(filePointer); 
}

bool FileLoader::ValidateRom() {
	if ((loadedROM[0] == 0x4e) && (loadedROM[1] == 0x45) && (loadedROM[2] == 0x53) && (loadedROM[3] == 0x1a)) {
		printf("NES rom detected.");
		return true;
	}
	else {
		printf("Invalid ROM");
		return false;
	}
}