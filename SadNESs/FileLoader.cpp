#include "stdafx.h"
#include "FileLoader.h"
#include "SadNESs.h"
#include "ROM.h"

FILE *filePointer;
int fileLenght;
extern bool debug;

FileLoader::FileLoader()
{
}

void FileLoader::LoadRom(std::string)
{
	filePointer = fopen("donkey.nes", "rb"); //TODO replace with something safe
	fseek(filePointer, 0, SEEK_END);
	fileLenght = ftell(filePointer);
	rewind(filePointer); 

	loadedROM = (char *)malloc((fileLenght + 1) * sizeof(char));
	fread(loadedROM, fileLenght, 1, filePointer);
	fclose(filePointer); 

	if (debug) {
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
		//std::cout << buffer[i] << " ";
		std::cout << std::endl;
	}
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