#include "stdafx.h"
#include "CPU.h"
#include "RAM.h"
#include "PPU.h"
#include "Debugger.h"

byte A;
byte X;
byte Y;
std::string ASM;//change name
int PC;
bool N;
bool V;
bool B;
bool D;
bool I;
bool Z;
bool C;

byte SP;
byte opcode;
int CC;

std::string debugString;

byte FlagsAsByte() {
	byte FlagByte = 0x4;
	FlagByte = FlagByte | (C << 0x7)
		| (Z << 0x6)
		| (I << 0x5)
		| (D << 0x4)
		| (B << 0x3)
		| (V << 0x1)
		| (N | N);
	return FlagByte;
}

void CPUDebugLog() {
	switch (opcode) {
		case(0x78):
			ASM = "SEI";
			break;
		case(0xD8):
			ASM = "CLD";
			break;
		case(0x18):
			ASM = "CLC";
			break;
		case(0x58):
			ASM = "CLI";
			break;
		case(0xB8):
			ASM = "CLV";
			break;
		case(0x38):
			ASM = "SEC";
			break;
		case(0xF8):
			ASM = "SED";
	//----------------------TO-IMPROVE----------------------------------//
		case(0x10):
			ASM = "BPL";
			break;
		case(0xAD):
			ASM = "LDA";
			break;
		default: {
			ASM = "N/A";
		}
	}
	Debugger::printd("%-5X %-9X %-31s A:%.2X X:%.2X Y:%.2X P:%.2X SP:%.2X CYC:%3d\n", PC, opcode, ASM.c_str(), A, X, Y, FlagsAsByte(), SP, CC);
}

void CPU::Initialize() {
	opcode = 0x0;
	SP = 0xFD;

	A = 0x0;
	X = 0x0;
	Y = 0x0;

	C = 0x0;
	Z = 0x0;
	I = 0x1;
	D = 0x1;
	B = 0x0;
	//  0x1;
	V = 0x0;
	N = 0x0;
	
	PC = 0xC000; //0x8000 or 0xFFFC (2 bytes)
	memory[0x2002] = 0x0;//A0	
}

void CPU::Run() {
	CC = 0;

	while (isRunning && (CC < 29780)) {
		CPU::Cycle();
		//PPU::Cycle();
		//PPU::Cycle();
		//PPU::Cycle();
	}	
}

void CPU::Pause() {
	//TODO write CPU code
}

void CPU::Reset() {
	//TODO write CPU code
}

void CPU::Cycle() {
	CPU::Fetch();
	CPU::Decode();
}

void CPU::Fetch() {
	opcode = RAM::ReadByte(PC);
	//add memory::read and memory::write to account for mirrored memory
}

void CPU::Decode() {
	int addr = 0x0;
	bool unknownOpcode = false; //debug

	switch (opcode) {

		case(0xD8): //CLD
			D = 0;
			CC = CC + 2;
			break;

		case(0x78): //SEI
			I = 1;
			CC = CC + 2;
			break;

		case(0x10): //BPL
			if (N == 0x0) { PC = PC + (memory[PC + 2] & 0xFF); CC++; }
			else { PC = PC++;}
			CC = CC + 2;
			break;

		case(0xAD): //LDA
			addr = (((memory[PC + 3] & 0xFF) << 8) | (memory[PC + 2] & 0xFF));
			if (addr == 0x0) Z = 1; else Z = 0;
			A = memory[addr];
			N = (A >> 7);
			PC = PC + 2;
			CC = CC + 4;
			break;

		case(0x18): //CLC
			C = 0;
			CC = CC + 2;
			break;

		case(0x58): //CLI
			I = 0;
			CC = CC + 2;
			break;

		case(0xB8): //CLV
			V = 0;
			CC = CC + 2;
			break;


		case(0x38):  //SEC
			C = 1;
			CC = CC + 2;
			break;

		case(0xF8): //SED
			D = 1;
			CC = CC + 2;
			break;

		default: {
			unknownOpcode = true;

			
			Debugger::printd("\nUnknown opcode: %.2X\n", opcode);
			isRunning = false; 
		}
	}

	if (debug & !unknownOpcode) CPUDebugLog();
	PC++;
}