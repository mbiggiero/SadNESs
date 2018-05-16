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

byte FlagsAsByteBE() {
	byte FlagByte = (N | N)
		| (V << 0x1)
		| (O << 0x2)
		| (B << 0x3)
		| (D << 0x4)
		| (I << 0x5)
		| (Z << 0x6)
		| (C << 0x7);
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
		case(0x86): Debugger::Log(" %.2X     STX $%.2X = %.2X                    A", RAM::ReadByte(oldPC + 0x1), RAM::ReadByte(oldPC + 0x1), debugByte); break;
		//case(0x96): break;
		//case(0x8E): break;

		//STA - Store Accumulator Immediate
		case(0x85): Debugger::Log(" %.2X     STA $%.2X = %.2X                    A", RAM::ReadByte(oldPC + 0x1), RAM::ReadByte(oldPC + 0x1), debugByte); break;
		//case(0x95): break;
		//case(0x8D): break;
		//case(0x9D): break;
		//case(0x99): break;
		//case(0x81): break;
		//case(0x91): break;

		//BIT - Bit Test Zero Page
		case(0x24): Debugger::Log(" %.2X     BIT $%.2X = %.2X                    A", RAM::ReadByte(oldPC + 0x1), RAM::ReadByte(oldPC + 0x1), RAM::ReadByte(RAM::ReadByte(oldPC + 0x1))); break;
		//case(0x2C): break;

		//JSR - Jump to Subroutine
		case(0x20):	Debugger::Log(" %.2X %.2X  JSR $%.4X                       A", RAM::ReadByte(oldPC + 0x1), RAM::ReadByte(oldPC + 0x2), RAM::ReadDWORD(oldPC + 0x1)); break;

		//NOP - No Operation
		case(0xEA): Debugger::Log("        NOP                             A"); break;

		//RTS - Return from Subroutine
		case(0x60): Debugger::Log("        RTS                             A"); break;


		//SEC - Set Carry Flag
		case(0x38):	Debugger::Log("        SEC                             A"); break;
		//SEI - Set Interrupt Disable
		case(0x78):	Debugger::Log("        SEI                             A"); break;
		//SED - Set Decimal Flag
		case(0xF8): Debugger::Log("        SED                             A"); break;

		
		//CLC - Clear Carry Flag
		case(0x18):	Debugger::Log("        CLC                             A"); break;

		//LDA - Load Accumulator Immediate
		case(0xA9): Debugger::Log(" %.2X     LDA #$%.2X                        A", RAM::ReadByte(oldPC + 0x1), RAM::ReadByte(oldPC + 0x1)); break;
		//case(0xA5): break;
		//case(0xB5): break;
		//case(0xAD): break;
		//case(0xBD): break;
		//case(0xB9): break;
		//case(0xA1): break;
		//case(0xB1): break;

		//CMP - Compare Immediate
		case(0xC9):  Debugger::Log(" %.2X     CMP #$%.2X                        A", RAM::ReadByte(oldPC + 0x1), RAM::ReadByte(oldPC + 0x1)); break;
		//case(0xC5): break;
		//case(0xD5): break;
		//case(0xCD): break;
		//case(0xDD): break;
		//case(0xD9): break;
		//case(0xC1): break;
		//case(0xD1): break;
		
		//LDA - Load Accumulator Immediate
		case(0x29): Debugger::Log(" %.2X     AND #$%.2X                        A", RAM::ReadByte(oldPC + 0x1), RAM::ReadByte(oldPC + 0x1)); break;
		//case(0x25): break;
		//case(0x35): break;
		//case(0x2D): break;
		//case(0x3D): break;
		//case(0x39): break;
		//case(0x21): break;
		//case(0x31): break;

		//BEQ - Branch if Equal
		case(0xF0): Debugger::Log(" %.2X     BEQ $%.4X                       A", RAM::ReadByte(oldPC + 1), oldPC + RAM::ReadByte(oldPC + 1) + 2); break;
		//BNE - Branch if Not Equal
		case(0xD0): Debugger::Log(" %.2X     BNE $%.4X                       A", RAM::ReadByte(oldPC + 1), oldPC + RAM::ReadByte(oldPC + 1) + 2); break;
		//BCS - Branch if Carry Set
		case(0xB0): Debugger::Log(" %.2X     BCS $%.4X                       A", RAM::ReadByte(oldPC + 1), oldPC + RAM::ReadByte(oldPC + 1) + 2); break;
		//BCC - Branch if Carry Clear
		case(0x90): Debugger::Log(" %.2X     BCC $%.4X                       A", RAM::ReadByte(oldPC + 1), oldPC + RAM::ReadByte(oldPC + 1) + 2); break;
		//BPL - Branch if Positive
		case(0x10): Debugger::Log(" %.2X     BPL $%.4X                       A", RAM::ReadByte(oldPC + 1), oldPC + RAM::ReadByte(oldPC + 1) + 2); break;
		//BVS - Branch if Overflow Set
		case(0x70): Debugger::Log(" %.2X     BVS $%.4X                       A", RAM::ReadByte(oldPC + 1), oldPC + RAM::ReadByte(oldPC + 1) + 2); break;
		//BVC - Branch if Overflow Clear
		case(0x50): Debugger::Log(" %.2X     BVC $%.4X                       A", RAM::ReadByte(oldPC + 1), oldPC + RAM::ReadByte(oldPC + 1) + 2); break;
	
		//PHP - Push Processor Status
		case(0x08): Debugger::Log("        PHP                             A"); break;

		//PLA - Pull Accumulator
		case(0x68): Debugger::Log("        PLA                             A"); break;

	}
}

void CPUDebugLog() {	
	Debugger::Log("%-5X %.2X ", oldPC, opcode);	DebugASM();	Debugger::Log(":%.2X X:%.2X Y:%.2X P:%.2X SP:%.2X CYC:%3d\r", debugA, debugX, debugY, debugP, debugSP, debugPP); OutputDebugString("\n");
	//printf("FLAGS: %x %x %x %x %x %x %x %x\n", C, Z, I, D, B, O ,V, N);
}

void SetZ(byte target) {
	(!target) ? Z = 1: Z = 0;
}

void SetN(byte target) {
	(target >> 7) ? N = 1: N = 0;
}

void PushDWORDToStack(int data) {
	RAM::WriteDWORD(0x1000 + SP, data);
	SP = SP - 2;
}

int PullDWORDFromStack() {
	SP = SP + 2;
	return RAM::ReadDWORD(0x1000 + SP);	
}

void PushByteToStack(byte data) {
	RAM::WriteByte(0x1000 + SP, data);
	SP = SP - 1;
}

byte PullByteFromStack() {
	SP = SP + 1;
	return RAM::ReadByte(0x1000 + SP);
}

void CheckPageSkip() {
	if (PC >= (oldPC - (oldPC % 0xFF) + 0xFF)) {
		CC = CC + 2;
	}
}

byte ByteAtZeroPage() {
	return RAM::ReadByte(RAM::ReadByte(PC + 1));
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
		PP = PP % 341;
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
		//case(0xA6): break;
		//case(0xB6): break;
		//case(0xAE): break;
		//case(0xBE): break;

		//JSR - Jump to Subroutine
		case(0x20):	PushDWORDToStack(PC); PC = RAM::ReadDWORD(PC + 1); PC--; CC = CC + 6; break;

		//RTS - Return from Subroutine
		case(0x60): PC = PullDWORDFromStack(); PC = PC + 2; CC = CC + 6; break;

		//NOP - No Operation
		case(0xEA): CC = CC + 2; break;

		//BIT - Bit Test Zero Page
		case(0x24):	SetZ(A & ByteAtZeroPage()); N = ByteAtZeroPage() << 7 & 0xFF; V = ByteAtZeroPage() << 6 & 0xFF; CC = CC + 3; PC++; break;
				
		//LDA - Load Accumulator Immediate
		case(0xA9): A = RAM::ReadByte(PC + 1); SetZ(A); SetN(A); PC++; CC = CC + 2; break;
		//case(0xA5): break;
		//case(0xB5): break;
		//case(0xAD): break;
		//case(0xBD): break;
		//case(0xB9): break;
		//case(0xA1): break;
		//case(0xB1): break;

		//AND - Logical AND - Immediate
		case(0x29): A = A & RAM::ReadByte(PC + 1); SetZ(A); SetN(A); PC++; CC = CC + 2; break;
		//case(0x25): break;
		//case(0x35): break;
		//case(0x2D): break;
		//case(0x3D): break;
		//case(0x39): break;
		//case(0x21): break;
		//case(0x31): break;

		//CMP - Compare Immediate
		case(0xC9): C = (A >= RAM::ReadByte(PC + 1) ? 1 : 0); Z = (A == RAM::ReadByte(PC + 1) ? 1 : 0); SetN(A); PC++; CC = CC + 2; break;
		//case(0xC5): break;
		//case(0xD5): break;
		//case(0xCD): break;
		//case(0xDD): break;
		//case(0xD9): break;
		//case(0xC1): break;
		//case(0xD1): break;

		//STX - Store X Register - Zero Page
		case(0x86): RAM::WriteByte(RAM::ReadByte(PC + 1), X); PC++; CC = CC + 3; break;
		//case(0x96): break; ^maybe broken
		//case(0x8E): break;

		//STA - Store Accumulator Immediate
		case(0x85): RAM::WriteByte(RAM::ReadByte(PC + 1), A); PC++; CC = CC + 3; break;
		//case(0x95): break; ^maybe broken
		//case(0x8D): break;
		//case(0x9D): break;
		//case(0x99): break;
		//case(0x81): break;
		//case(0x91): break;

		//BCC - Branch if Carry Clear
		case(0x90): if (C == 0) { PC = PC + RAM::ReadByte(PC + 1) + 1; CheckPageSkip(); CC++; }	else { PC++; } CC = CC + 2; break;
		//BCS - Branch if Carry Set
		case(0xB0): if (C == 1) { PC = PC + RAM::ReadByte(PC + 1) + 1; CheckPageSkip(); CC++; }	else { PC++; } CC = CC + 2; break;
		//BEQ - Branch if Equal
		case(0xF0): if (Z == 1) { PC = PC + RAM::ReadByte(PC + 1) + 1; CheckPageSkip(); CC++; }	else { PC++; } CC = CC + 2; break;
		//BNE - Branch if Not Equal
		case(0xD0): if (Z == 0) { PC = PC + RAM::ReadByte(PC + 1) + 1; CheckPageSkip(); CC++; }	else { PC++; } CC = CC + 2; break;
		//BPL - Branch if Positive
		case(0x10): if (N == 0) { PC = PC + RAM::ReadByte(PC + 1) + 1; CheckPageSkip(); CC++; }	else { PC++; } CC = CC + 2; break;
		//BVS - Branch if Overflow Set
		case(0x70): if (V == 1) { PC = PC + RAM::ReadByte(PC + 1) + 1; CheckPageSkip(); CC++; } else { PC++; } CC = CC + 2; break;
		//BVC - Branch if Overflow Clear
		case(0x50): if (V == 0) { PC = PC + RAM::ReadByte(PC + 1) + 1; CheckPageSkip(); CC++; }	else { PC++; } CC = CC + 2; break;

		//SEC - Set Carry Flag
		case(0x38):	C = 1; CC = CC + 2; break;
		//SEI - Set Interrupt Disable
		case(0x78): I = 1; CC = CC + 2; break;
		//SED - Set Decimal Flag
		case(0xF8): D= 1; CC = CC + 2; break;

		//CLC - Clear Carry Flag
		case(0x18): C = 0; CC = CC + 2; break;

		//PHP - Push Processor Status
		case(0x08): PushByteToStack(0x18 | FlagsAsByte()); CC = CC + 3; break;

		//PLA - Pull Accumulator
		case(0x68): A = PullByteFromStack(); SetZ(A); SetN(A); CC = CC + 4; break;


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