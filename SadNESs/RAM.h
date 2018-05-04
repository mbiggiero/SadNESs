#pragma once
extern byte memory[0xFFFF];

class RAM
{
public:
	static void WriteByte(int, byte);
	static byte ReadByte(int);
	static void WriteDWORD(int, byte);
	static byte ReadDWORD(int);
};

