#pragma once
extern char RAM[0xFFFF];

class CPU
{
public:
	CPU();
	static void Initialize();
	static void Run();
	static void Pause();
	static void Reset();
	static void Cycle();
	static void Fetch();
	static void Decode();
};

