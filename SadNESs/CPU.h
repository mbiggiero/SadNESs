#pragma once
extern byte RAM[0xFFFF];
extern bool isRunning;
extern bool debug;

class CPU
{
public:
	static void Initialize();
	static void Run();
	static void Pause();
	static void Reset();
	static void Cycle();
	static void Fetch();
	static void Decode();
};

