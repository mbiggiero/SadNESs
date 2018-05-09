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

std::string debugString;
int debugPC;
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
		case(0x4C):	Debugger::Log(" %.2X %.2X  JMP $%.4X                       A", RAM::ReadByte(debugPC + 0x1), RAM::ReadByte(debugPC + 0x2), RAM::ReadDWORD(debugPC + 0x1)); break;
		//JMP - Jump - Indirect
		//case(0x6C): break;

		//LDX - Load X Register - Immediate
		case(0xA2):	Debugger::Log(" %.2X     LDX #$%.2X                        A", RAM::ReadByte(debugPC + 0x1), RAM::ReadByte(debugPC + 0x1)); break;
		//case(0xA6): break;
		//case(0xB6): break;
		//case(0xAE): break;
		//case(0xBE): break;

		//STX - Store X Register - Zero Page
		case(0x86): Debugger::Log(" %.2X     STX $%.2X = %.2X                    A", RAM::ReadByte(debugPC + 0x1), RAM::ReadByte(debugPC + 0x1), debugX); break;
		//case(0x96): break;
		//case(0x8E): break;
	}
}

void CPUDebugLog() {	
	Debugger::Log("%-5X %X ", debugPC, opcode);	DebugASM();	Debugger::Log(":%.2X X:%.2X Y:%.2X P:%.2X SP:%.2X CYC:%3d\r", debugA, debugX, debugY, debugP, debugSP, debugPP); OutputDebugString("\n");
	//printf("FLAGS: %x %x %x %x %x %x %x %x\n", C, Z, I, D, B, O ,V, N);
}

void setZ() {
	if (X == 0) { Z = 1; };
}

void setN() {
	if ((X & 0x80 >> 7) == 1) { N = 1; };
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
}

void CPU::Decode() {
	bool unknownOpcode = false; //debug

	if (debug) { debugPC = PC; debugPP = PP; debugA = A; debugX = X; debugY = Y; debugSP = SP; debugP = FlagsAsByte(); }

	switch (opcode) {
		//JMP - Jump - Absolute
	case(0x4C):	PC = RAM::ReadDWORD(PC + 0x1);						PC--; CC = CC + 3; PP = PP + 9; break;
		//JMP - Jump - Indirect
		//case(0x6C): PC = RAM::ReadDWORD(RAM::ReadDWORD(PC+0x1)); PC--; CC = CC + 5; break;

		//LDX - Load X Register - Immediate
	case(0xA2):	X = RAM::ReadByte(PC + 1); setZ(); setN();			PC++; CC = CC + 2; PP = PP + 6; break;
		//case(0xA6): break;//fix ^^
		//case(0xB6): break;
		//case(0xAE): break;
		//case(0xBE): break;

		//STX - Store X Register - Zero Page
	case(0x86): RAM::WriteByte(RAM::ReadByte(PC + 1), X);			PC++; CC = CC + 3; PP = PP + 9; break;
		//case(0x96): break;
		//case(0x8E): break;



		//--------------------------------------------------------------------------

		/*case(0xD8): //CLD
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
			break;*/

		default: {
			Debugger::Log("\nUnknown opcode: %.2X\n", opcode);
			unknownOpcode = true;			
			isRunning = false; 
		}
	}

	PC++;
	if (debug & !unknownOpcode) CPUDebugLog();
}