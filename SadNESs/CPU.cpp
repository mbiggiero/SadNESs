#include "stdafx.h"
#include "CPU.h"
#include "RAM.h"
#include "PPU.h"

byte A;
byte X;
byte Y;
std::string ASM;//change name
int PC;
bool Flag_N;
bool Flag_V;
bool Flag_1;
bool Flag_B;
bool Flag_D;
bool Flag_I;
bool Flag_Z;
bool Flag_C;

byte SP;
byte opcode;
int cyclesCount;

void CPUDebugLog() {
	switch (opcode) {
		case(0x10):
			ASM = "BPL";
			break;
		case(0xAD):
			ASM = "LDA";
			break;
		case(0x18):
			ASM = "CLC";
			break;
		case(0x58):
			ASM = "CLI";
			break;
		case(0xD8):
			ASM = "CLD";
			break;
		case(0xB8):
			ASM = "CLV";
			break;
		case(0x78):
			ASM = "SEI";
			break;
		case(0x38):
			ASM = "SEC";
			break;
		case(0xF8):
			ASM = "SED";
		default: {
			ASM = "N/A";
		}
	}
	printf("%X: \t%X\t%s\t\t\tA:%.2x X:%x Y:%x SP:%x N:%x V:%x 1:%x B:%x D:%x I:%x Z:%x C:%x CYC:%d\n", PC, opcode, ASM.c_str(), A & 0xFF, X, Y, SP, Flag_N, Flag_V, Flag_1, Flag_B, Flag_D, Flag_I, Flag_Z, Flag_C, cyclesCount);
}

void CPU::Initialize() {
	opcode = 0x0;
	SP = 0x24;
	A = 0x0;
	X = 0x0;
	Y = 0x0;
	Flag_N = 0x0;
	Flag_V = 0x0;
	Flag_1 = 0x1;	//always
	Flag_B = 0x0;	//always
	Flag_D = 0x0;	//always?
	Flag_I = 0x0;
	Flag_Z = 0x0;
	Flag_C = 0x0;
	PC = 0xC000; //0x8000 or 0xFFFC (2 bytes)
	memory[0x2002] = 0x0;//A0	
}

void CPU::Run() {
	cyclesCount = 0;

	while (isRunning && (cyclesCount < 29780)) {
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
		case(0x10): //BPL
			if (Flag_N == 0x0) { PC = PC + (memory[PC + 2] & 0xFF); cyclesCount++; }
			else { PC = PC++; }
			cyclesCount = cyclesCount + 2;
			break;

		case(0xAD): //LDA
			addr = (((memory[PC + 3] & 0xFF) << 8) | (memory[PC + 2] & 0xFF));
			if (addr == 0x0) Flag_Z = 1; else Flag_Z = 0;
			A = memory[addr];
			Flag_N = (A >> 7);
			PC = PC + 2;
			cyclesCount = cyclesCount + 4;
			break;

		case(0x18): //CLC
			Flag_C = 0;
			cyclesCount = cyclesCount + 2;
			break;

		case(0x58): //CLI
			Flag_I = 0;
			cyclesCount = cyclesCount + 2;
			break;

		case(0xD8): //CLD
			Flag_D = 0;
			cyclesCount = cyclesCount + 2;
			break;

		case(0xB8): //CLV
			Flag_V = 0;
			cyclesCount = cyclesCount + 2;
			break;

		case(0x78): //SEI
			Flag_I = 1;
			cyclesCount = cyclesCount + 2;
			break;

		case(0x38):  //SEC
			Flag_C = 1;
			cyclesCount = cyclesCount + 2;
			break;

		case(0xF8): //SED
			Flag_D = 1;
			cyclesCount = cyclesCount + 2;
			break;

		default: {
			unknownOpcode = true;
			printf("\nUnknown opcode: %.2X\n", opcode); isRunning = false; 
		}
	}

	if (debug & !unknownOpcode) CPUDebugLog();
	PC++;
}