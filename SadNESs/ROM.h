#pragma once
extern char *loadedROM;

class ROM
{
public:
	static void Setup();
	static char PRG_size;
	static char CHR_size;
	static char RAM_size;
	static char ROM_region;
};

