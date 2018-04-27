#include "stdafx.h"
#include "CPU.h"
#include "Memory.h"
#include "PPU.h"

char memory[0xFFFF];

char A;
char X;
char Y;
int PC;
bool Flag_N; //TODO rewrite flags as byte
bool Flag_V;
bool Flag_1;
bool Flag_B;
bool Flag_D;
bool Flag_I;
bool Flag_Z;
bool Flag_C;

char SP;
char opcode;
int cycleCount = 0;
int cyclesLeft = 29780;

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
	PC = 0x8000;
	RAM[0x2002] = 0x0;//A0	
}

void CPU::Run() {
	int elapsedCycles = 0;

	while (elapsedCycles < 29780) {
		CPU::Fetch();
		CPU::Decode();
	}
	PPU::Cycle();
	PPU::Cycle();
	PPU::Cycle();
}

void CPU::Pause() {
	//TODO write CPU code
}

void CPU::Reset() {
	//TODO write CPU code
}

void CPU::Cycle() {
	//TODO write CPU code
}

void CPU::Fetch() {
	opcode = memory[PC] & 0x255;
	//printf("PC %x\n", PC);
}

void CPU::Decode() {
	int addr = 0x0;
	switch (opcode & 0x255) { // 7/255 done!;

	case(0x10):
		//Flag_C = 0;
		printf("%x: \t%x\tBPL\t\t\tA:%.2x X:%x Y:%x SP:%x N:%x V:%x 1:%x B:%x D:%x I:%x Z:%x C:%x CYC:%d\n",
			PC, opcode&0x255, A&0x255, X, Y, SP, Flag_N, Flag_V, Flag_1, Flag_B, Flag_D, Flag_I, Flag_Z, Flag_C, cycleCount);
		if (Flag_N == 0x0) { PC = PC + (memory[PC + 2] & 0x255); cycleCount++; }
		else { PC = PC++; }
		cyclesLeft = cyclesLeft - 1;
		cycleCount = cycleCount + 2;
		break;

	case(0xAD):
		addr = (((memory[PC + 3] & 0x255) << 8) | (memory[PC + 2] & 0x255));
		//printf("%.2x and %.2x\n", (memory[PC + 3+0xF] & 0x255), (memory[PC +2+0xF]&0x255));
		//printf("addr %x\n", ((memory[PC + 3 + 0xF] & 0x255)<<8) | (memory[PC + 2 + 0xF] & 0x255));
		//printf("addr == %x\n", addr);

		if (addr == 0x0) Flag_Z = 1; else Flag_Z = 0;
		A = memory[addr];
		Flag_N = (A >> 7);
		printf("%x: \t%x\tLDA\t\t\tA:%.2x X:%x Y:%x SP:%x N:%x V:%x 1:%x B:%x D:%x I:%x Z:%x C:%x CYC:%d\n",
			PC, opcode&0x255, A&0x255, X, Y, SP, Flag_N, Flag_V, Flag_1, Flag_B, Flag_D, Flag_I, Flag_Z, Flag_C, cycleCount);
		cyclesLeft = cyclesLeft - 1;
		PC = PC + 2;
		cycleCount = cycleCount + 4;
		break;
	case(0x18):
		Flag_C = 0;
		printf("%x: \t%x\tCLC\t\t\tA:%.2x X:%x Y:%x SP:%x N:%x V:%x 1:%x B:%x D:%x I:%x Z:%x C:%x CYC:%d\n",
			PC, opcode&0x255, A&0x255, X, Y, SP, Flag_N, Flag_V, Flag_1, Flag_B, Flag_D, Flag_I, Flag_Z, Flag_C, cycleCount);
		cyclesLeft = cyclesLeft - 1;
		cycleCount = cycleCount + 2;
		break;
	case(0x58):
		Flag_I = 0;
		printf("%x: \t%x\tCLI\t\t\tA:%.2x X:%x Y:%x SP:%x N:%x V:%x 1:%x B:%x D:%x I:%x Z:%x C:%x CYC:%d\n",
			PC, opcode&0x255, A&0x255, X, Y, SP, Flag_N, Flag_V, Flag_1, Flag_B, Flag_D, Flag_I, Flag_Z, Flag_C, cycleCount);
		cyclesLeft = cyclesLeft - 1;
		cycleCount = cycleCount + 2;
		break;
	case(0xD8):
		Flag_D = 0;
		printf("%x: \t%x\tCLD\t\t\tA:%.2x X:%x Y:%x SP:%x N:%x V:%x 1:%x B:%x D:%x I:%x Z:%x C:%x CYC:%d\n",
			PC, opcode&0x255, A&0x255, X, Y, SP, Flag_N, Flag_V, Flag_1, Flag_B, Flag_D, Flag_I, Flag_Z, Flag_C, cycleCount);
		cyclesLeft = cyclesLeft - 1;
		cycleCount = cycleCount + 2;
		break;
	case(0xB8):
		Flag_V = 0;
		printf("%x: \t%x\tCLV\t\t\tA:%.2x X:%x Y:%x SP:%x N:%x V:%x 1:%x B:%x D:%x I:%x Z:%x C:%x CYC:%d\n",
			PC, opcode&0x255, A&0x255, X, Y, SP, Flag_N, Flag_V, Flag_1, Flag_B, Flag_D, Flag_I, Flag_Z, Flag_C, cycleCount);
		cyclesLeft = cyclesLeft - 1;
		cycleCount = cycleCount + 2;
		break;

	case(0x78):
		Flag_I = 1;
		printf("%x: \t%x\tSEI\t\t\tA:%.2x X:%x Y:%x SP:%x N:%x V:%x 1:%x B:%x D:%x I:%x Z:%x C:%x CYC:%d\n",
			PC, opcode&0x255, A&0x255, X, Y, SP, Flag_N, Flag_V, Flag_1, Flag_B, Flag_D, Flag_I, Flag_Z, Flag_C, cycleCount);
		cyclesLeft = cyclesLeft - 1;
		cycleCount = cycleCount + 2;
		break;
	case(0x38):
		Flag_C = 1;
		printf("%x: \t%x\tSEC\t\t\tA:%.2x X:%x Y:%x SP:%x N:%x V:%x 1:%x B:%x D:%x I:%x Z:%x C:%x CYC:%d\n",
			PC, opcode&0x255, A&0x255, X, Y, SP, Flag_N, Flag_V, Flag_1, Flag_B, Flag_D, Flag_I, Flag_Z, Flag_C, cycleCount);
		cyclesLeft = cyclesLeft - 1;
		cycleCount = cycleCount + 2;
		break;
	case(0xF8):
		Flag_D = 1;
		printf("%x: \t%x\tSED\t\t\tA:%.2x X:%x Y:%x SP:%x N:%x V:%x 1:%x B:%x D:%x I:%x Z:%x C:%x CYC:%d\n",
			PC, opcode&0x255, A&0x255, X, Y, SP, Flag_N, Flag_V, Flag_1, Flag_B, Flag_D, Flag_I, Flag_Z, Flag_C, cycleCount);
		cyclesLeft = cyclesLeft - 1;
		cycleCount = cycleCount + 2;
		break;

		//	case(0xAA):
		//to do negative check
		//break;

	default:printf("\nInvalid opcode: %.2x\n", opcode&0x255); cyclesLeft = 0;
	}
	PC++;
}