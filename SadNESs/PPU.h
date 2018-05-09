#pragma once
extern int PP;
class PPU
{
public:
	PPU();
	static void Initialize();
	static void Cycle();
	static void Reset();
};

