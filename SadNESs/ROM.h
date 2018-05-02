#pragma once
extern byte *loadedROM;
extern char PRG_size;
extern char CHR_size;
extern char RAM_size;
extern char ROM_region;

class ROM
{
public:
	static void Setup();
};

