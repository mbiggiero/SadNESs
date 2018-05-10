#pragma once
extern byte memory[0xFFFF];
extern byte debugByte;

class RAM
{
public:
	static void WriteByte(int, byte);
	static byte ReadByte(int);
	static void WriteDWORD(int, int);
	static int ReadDWORD(int);
};

