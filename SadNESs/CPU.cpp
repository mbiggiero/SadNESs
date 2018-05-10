#include "stdafx.h"
#include "CPU.h"
#include "RAM.h"
#include "PPU.h"
#include "Debugger.h"

byte A;
byte X;
byte Y;
std::string ASM;
int PC; //16 bit
bool N; //7th bit == 1 => 1
bool V;
bool B;
bool D;
bool I;
bool Z;
bool O;
bool C;

byte SP; //push decrement pull increment
byte opcode;
int addr;
int CC;
int PP;
int oldCC;
int oldPC;

std::string debugString;
int debugPP;
byte debugA;
byte debugX;
byte debugY;
byte debugP;
byte debugSP;

byte FlagsAsByte() {
	byte FlagByte = (C | C)
		| (Z << 0x1)
		| (I << 0x2)
		| (D << 0x3)
		| (B << 0x4)
		| (O << 0x5)
		| (V << 0x6)
		| (N << 0x7);
	return FlagByte;
}

void DebugASM() {
	switch (opcode) {
		//JMP - Jump - Absolute
		case(0x4C):	Debugger::Log(" %.2X %.2X  JMP $%.4X                       A", RAM::ReadByte(oldPC + 0x1), RAM::ReadByte(oldPC + 0x2), RAM::ReadDWORD(oldPC + 0x1)); break;
		//JMP - Jump - Indirect
		//case(0x6C): break;

		//LDX - Load X Register - Immediate
		case(0xA2):	Debugger::Log(" %.2X     LDX #$%.2X                        A", RAM::ReadByte(oldPC + 0x1), RAM::ReadByte(oldPC + 0x1)); break;
		//case(0xA6): break;
		//case(0xB6): break;
		//case(0xAE): break;
		//case(0xBE): break;

		//STX - Store X Register - Zero Page
		case(0x86): Debugger::Log(" %.2X     STX $%.2X = %.2X                    A", RAM::ReadByte(oldPC + 0x1), RAM::ReadByte(oldPC + 0x1), debugX); break;
		//case(0x96): break;
		//case(0x8E): break;

		//JSR - Jump to Subroutine
		case(0x20):	Debugger::Log(" %.2X %.2X  JSR $%.4X                       A", RAM::ReadByte(oldPC + 0x1), RAM::ReadByte(oldPC + 0x2), RAM::ReadDWORD(oldPC + 0x1)); break;

		//NOP - No Operation
		case(0xEA): Debugger::Log("        NOP                             A"); break;

		//SEC - Set Carry Flag
		case(0x38):	Debugger::Log("        SEC                             A"); break;

		//BCS - Branch if Carry Set
		case(0xB0): Debugger::Log(" %.2X     BCS $%.4X                       A", RAM::ReadByte(oldPC + 0x1), PC); break;

		//CLC - Clear Carry Flag
		case(0x18):	Debugger::Log("        CLC                             A"); break;

		//BCC - Branch if Carry Clear
		case(0x90): Debugger::Log(" %.2X     BCC $%.4X                       A", RAM::ReadByte(oldPC + 0x1), PC); break;

		//LDA - Load Accumulator Immediate
		case(0xA9): Debugger::Log(" %.2X     LDA #$%.2X                        A", RAM::ReadByte(oldPC + 0x1), RAM::ReadByte(oldPC + 0x1)); break;
		//case(0xA5): break;
		//case(0xB5): break;
		//case(0xAD): break;
		//case(0xBD): break;
		//case(0xB9): break;
		//case(0xA1): break;
		//case(0xB1): break;

		//BEQ - Branch if Equal
		case(0xF0): Debugger::Log(" %.2X     BEQ $%.4X                       A", RAM::ReadByte(oldPC + 0x1), PC); break;

		//BNE - Branch if Not Equal
		case(0xD0): Debugger::Log(" %.2X     BNE $%.4X                       A", RAM::ReadByte(oldPC + 0x1), PC); break;

	}
}

void CPUDebugLog() {	
	Debugger::Log("%-5X %X ", oldPC, opcode);	DebugASM();	Debugger::Log(":%.2X X:%.2X Y:%.2X P:%.2X SP:%.2X CYC:%3d\r", debugA, debugX, debugY, debugP, debugSP, debugPP); OutputDebugString("\n");
	//printf("FLAGS: %x %x %x %x %x %x %x %x\n", C, Z, I, D, B, O ,V, N);
}

void SetZ(byte target) {
	if (target == 0) { Z = 1; } else Z = 0;
}

void SetN(byte target) {
	if ((target & 0x80 >> 7) == 1) { N = 1; }	else N = 0;
}

void PushToStack() {
	RAM::WriteDWORD(0x1000 + SP, PC);
	SP = SP - 2;
}

void PullFromStack() {
	PC = RAM::ReadDWORD(0x1000 + SP);
	SP = SP + 2;
}

void CheckPageSkip() {
	if (PC >= (oldPC - (oldPC % 0xFF) + 0xFF)) {
		CC = CC + 2;
	}
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
	D = 0x0;
	B = 0x0;
	O = 0x1;
	V = 0x0;
	N = 0x0;
	
	PC = 0xC000; //0x8000 or 0xFFFC (2 bytes)
	memory[0x2002] = 0x0;//A0	
}

void CPU::Run() {
	CC = 0;
	PP = 0;

	while (isRunning && (CC < 29780)) {
		CPU::Cycle();
		while (PP < (CC * 3)) {
			PPU::Cycle();
			PP++;
		}
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
}

void CPU::Decode() {
	bool unknownOpcode = false; //debug
	oldCC = CC;
	oldPC = PC;

	if (debug) {debugPP = PP; debugA = A; debugX = X; debugY = Y; debugSP = SP; debugP = FlagsAsByte(); }

	switch (opcode) {
		//JMP - Jump - Absolute
		case(0x4C):	PC = RAM::ReadDWORD(PC + 0x1);							PC--; CC = CC + 3; break;
		//JMP - Jump - Indirect
		//case(0x6C): PC = RAM::ReadDWORD(RAM::ReadDWORD(PC+0x1)); PC--; CC = CC + 5; break;

		//LDX - Load X Register - Immediate
		case(0xA2):	X = RAM::ReadByte(PC + 1); SetZ(X); SetN(X); PC++; CC = CC + 2; break;
		//case(0xA6): break;//fix ^^
		//case(0xB6): break;
		//case(0xAE): break;
		//case(0xBE): break;

		//STX - Store X Register - Zero Page
		case(0x86): RAM::WriteByte(RAM::ReadByte(PC + 1), X); PC++; CC = CC + 3; break;
		//case(0x96): break;
		//case(0x8E): break;

		//JSR - Jump to Subroutine
		case(0x20):	PushToStack(); PC = RAM::ReadDWORD(PC + 1); PC--; CC = CC + 6; break;

		//NOP - No Operation
		case(0xEA): CC = CC + 2; break;

		//SEC - Set Carry Flag
		case(0x38):	C = 1; CC = CC + 2; break;

		//BCS - Branch if Carry Set
		case(0xB0): if (C == 1) { PC = PC + RAM::ReadByte(PC + 1) + 1; CheckPageSkip(); CC++; }
					else { PC++; } CC = CC + 2; break;

		//CLC - Clear Carry Flag
		case(0x18): C = 0; CC = CC + 2; break;

		//BCC - Branch if Carry Clear
		case(0x90): if (C == 0) { PC = PC + RAM::ReadByte(PC + 1) + 1; CheckPageSkip(); CC++; }	else { PC++; } CC = CC + 2; break;

		//LDA - Load Accumulator Immediate
		case(0xA9): A = RAM::ReadByte(PC + 1); SetZ(A); SetN(A); PC++; CC = CC + 2; break;
		//case(0xA5): break;
		//case(0xB5): break;
		//case(0xAD): break;
		//case(0xBD): break;
		//case(0xB9): break;
		//case(0xA1): break;
		//case(0xB1): break;

		//BEQ - Branch if Equal
		case(0xF0): if (Z == 1) { PC = PC + RAM::ReadByte(PC + 1) + 1; CheckPageSkip(); CC++; }	else { PC++; } CC = CC + 2; break;
		
		//BNE - Branch if Not Equal
		case(0xD0): if (Z == 0) { PC = PC + RAM::ReadByte(PC + 1) + 1; CheckPageSkip(); CC++; }	else { PC++; } CC = CC + 2; break;


		default: {
			if (debug) {
				Debugger::Log("\nUnknown opcode: %.2X\n", opcode);
				Debugger::Log("\nPC: %.2X\n", PC);
				unknownOpcode = true;
			}
			isRunning = false;
		}
	}

	PC++;
	if (debug & !unknownOpcode) CPUDebugLog();
}