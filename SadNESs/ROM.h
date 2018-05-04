#pragma once

#include "RAM.h"
extern byte *loadedROM;
extern byte mapper;
extern byte PRG_size;
extern byte CHR_size;
extern byte RAM_size;
extern byte ROM_region;

class ROM
{
public:
	static void Setup();
};

