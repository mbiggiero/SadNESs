#pragma once
extern int fileLenght;

class FileLoader
{
public:
	FileLoader();
	static void LoadRom(std::string);
	static bool ValidateRom();
};

