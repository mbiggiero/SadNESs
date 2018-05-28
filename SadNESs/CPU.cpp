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
byte p;
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
byte debugC;
byte debugN;
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

byte Flip(byte b) {
	b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
	b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
	b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
	return b;
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
		case(0xAE): Debugger::Log(" %.2X %.2X  LDX $%.4X = %.2X                  A", RAM::ReadByte(oldPC + 0x1), RAM::ReadByte(oldPC + 0x2), RAM::ReadDWORD(oldPC + 0x1), RAM::ReadByte(RAM::ReadDWORD(oldPC + 1))); break;
		//case(0xBE): break;

		//LDY - Load Y Register - Immediate
		case(0xA0):	Debugger::Log(" %.2X     LDY #$%.2X                        A", RAM::ReadByte(oldPC + 0x1), RAM::ReadByte(oldPC + 0x1)); break;
		//case(0xA4): break;
		//case(0xB4): break;
		case(0xAC): Debugger::Log(" %.2X %.2X  LDY $%.4X = %.2X                  A", RAM::ReadByte(oldPC + 0x1), RAM::ReadByte(oldPC + 0x2), RAM::ReadDWORD(oldPC + 0x1), RAM::ReadByte(RAM::ReadDWORD(oldPC + 1))); break;
		//case(0xBC): break;

		//STX - Store X Register - Zero Page
		case(0x86): Debugger::Log(" %.2X     STX $%.2X = %.2X                    A", RAM::ReadByte(oldPC + 0x1), RAM::ReadByte(oldPC + 0x1), debugByte); break;
		//case(0x96): break;
		case(0x8E): Debugger::Log(" %.2X %.2X  STX $%.4X = %.2X                  A", RAM::ReadByte(oldPC + 0x1), RAM::ReadByte(oldPC + 0x2), RAM::ReadDWORD(oldPC + 0x1), debugByte); break;

		//STY - Store Y Register - Zero Page
		case(0x84): Debugger::Log(" %.2X     STY $%.2X = %.2X                    A", RAM::ReadByte(oldPC + 0x1), RAM::ReadByte(oldPC + 0x1), debugByte); break;
		//case(0x94): break;
		case(0x8C): Debugger::Log(" %.2X %.2X  STY $%.4X = %.2X                  A", RAM::ReadByte(oldPC + 0x1), RAM::ReadByte(oldPC + 0x2), RAM::ReadDWORD(oldPC + 0x1), debugByte); break;

		//STA - Store Accumulator Immediate
		case(0x85): Debugger::Log(" %.2X     STA $%.2X = %.2X                    A", RAM::ReadByte(oldPC + 0x1), RAM::ReadByte(oldPC + 0x1), debugByte); break;
		//case(0x95): break;
		case(0x8D): Debugger::Log(" %.2X %.2X  STA $%.4X = %.2X                  A", RAM::ReadByte(oldPC + 0x1), RAM::ReadByte(oldPC + 0x2), RAM::ReadDWORD(oldPC + 0x1), debugByte); break;
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
		//CLD - Clear Decimal Mode
		case(0xD8):	Debugger::Log("        CLD                             A"); break;
		//CLV - Clear Overflow Flag
		case(0xB8):	Debugger::Log("        CLV                             A"); break;
		//CLI - Clear Interrupt Disable
		case(0x58):	Debugger::Log("        CLI                             A"); break;

		//INY - Increment Y Register
		case(0xC8):	Debugger::Log("        INY                             A"); break;
		//INX - Increment X Register
		case(0xE8):	Debugger::Log("        INX                             A"); break;

		//DEY - Decrement Y Register
		case(0x88):	Debugger::Log("        DEY                             A"); break;
		//DEX - Decrement X Register
		case(0xCA):	Debugger::Log("        DEX                             A"); break;

		//TAY - Transfer Accumulator to Y
		case(0xA8):	Debugger::Log("        TAY                             A"); break;
		//TAX - Transfer Accumulator to X
		case(0xAA):	Debugger::Log("        TAX                             A"); break;

		//TYA - Transfer Y to Accumulator
		case(0x98):	Debugger::Log("        TYA                             A"); break;
		//TXA - Transfer X to Accumulator
		case(0x8A):	Debugger::Log("        TXA                             A"); break;

		//TSX - Transfer Stack Pointer to X
		case(0xBA):	Debugger::Log("        TSX                             A"); break;
		//TXS - Transfer X to Stack Pointer
		case(0x9A):	Debugger::Log("        TXS                             A"); break;

		//LDA - Load Accumulator Immediate
		case(0xA9): Debugger::Log(" %.2X     LDA #$%.2X                        A", RAM::ReadByte(oldPC + 0x1), RAM::ReadByte(oldPC + 0x1)); break;
		//case(0xA5): break;
		//case(0xB5): break;
		case(0xAD): Debugger::Log(" %.2X %.2X  LDA $%.4X = %.2X                  A", RAM::ReadByte(oldPC + 0x1), RAM::ReadByte(oldPC + 0x2), RAM::ReadDWORD(oldPC + 0x1), RAM::ReadByte(RAM::ReadDWORD(oldPC + 1))); break;
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

		//CPY - Compare Y Register Immediate
		case(0xC0):  Debugger::Log(" %.2X     CPY #$%.2X                        A", RAM::ReadByte(oldPC + 0x1), RAM::ReadByte(oldPC + 0x1)); break;
		//case(0xC4): break;
		//case(0xCC): break;

		//CPX - Compare X Register Immediate
		case(0xE0):  Debugger::Log(" %.2X     CPX #$%.2X                        A", RAM::ReadByte(oldPC + 0x1), RAM::ReadByte(oldPC + 0x1)); break;
		//case(0xE4): break;
		//case(0xEC): break;

		//ADC - Add with Carry
		case(0x69):  Debugger::Log(" %.2X     ADC #$%.2X                        A", RAM::ReadByte(oldPC + 0x1), RAM::ReadByte(oldPC + 0x1)); break;
		//case(0x65): break;
		//case(0x75): break;
		//case(0x6D): break;
		//case(0x7D): break;
		//case(0x79): break;
		//case(0x61): break;
		//case(0x71): break;

		//SBC - Subtract with Carry
		case(0xE9):  Debugger::Log(" %.2X     SBC #$%.2X                        A", RAM::ReadByte(oldPC + 0x1), RAM::ReadByte(oldPC + 0x1)); break;
		//case(0xE5): break;
		//case(0xF5): break;
		//case(0xED): break;
		//case(0xFD): break;
		//case(0xF9): break;
		//case(0xE1): break;
		//case(0xF1): break;
		
		//AND - Logical AND
		case(0x29): Debugger::Log(" %.2X     AND #$%.2X                        A", RAM::ReadByte(oldPC + 0x1), RAM::ReadByte(oldPC + 0x1)); break;
		//case(0x25): break;
		//case(0x35): break;
		case(0x2D):Debugger::Log(" %.2X %.2X  AND $%.4X = %.2X                  A", RAM::ReadByte(oldPC + 0x1), RAM::ReadByte(oldPC + 0x2), RAM::ReadDWORD(oldPC + 0x1), RAM::ReadByte(RAM::ReadDWORD(oldPC + 1))); break;
		//case(0x3D): break;
		//case(0x39): break;
		//case(0x21): break;
		//case(0x31): break;

		//ORA - Logical Inclusive OR Immediate
		case(0x09): Debugger::Log(" %.2X     ORA #$%.2X                        A", RAM::ReadByte(oldPC + 0x1), RAM::ReadByte(oldPC + 0x1)); break;
		//case(0x05): break;
		//case(0x15): break;
		//case(0x0D): break;
		//case(0x1D): break;
		//case(0x19): break;
		//case(0x01): break;
		//case(0x11): break;

		//EOR - Exclusive OR Immediate
		case(0x49): Debugger::Log(" %.2X     EOR #$%.2X                        A", RAM::ReadByte(oldPC + 0x1), RAM::ReadByte(oldPC + 0x1)); break;
		//case(0x45): break;
		//case(0x55): break;
		//case(0x4D): break;
		//case(0x5D): break;
		//case(0x59): break;
		//case(0x41): break;
		//case(0x51): break;

		//LSR - Logical Shift Right
		case(0x4A): Debugger::Log("        LSR A                           A"); break;
		//case(0x46): break;
		//case(0x56): break;
		//case(0x4E): break;
		//case(0x5E): break;

		//ASL - Arithmetic Shift Left
		case(0x0A): Debugger::Log("        ASL A                           A"); break;
		//case(0x06): break;
		//case(0x16): break;
		//case(0x0E): break;
		//case(0x1E): break;

		//ROR - Rotate Right
		case(0x6A): Debugger::Log("        ROR A                           A"); break;
		//case(0x66): break;
		//case(0x76): break;
		//case(0x6E): break;
		//case(0x7E): break;

		//ROL - Rotate Left
		case(0x2A): Debugger::Log("        ROL A                           A"); break;
		//case(0x26): break;
		//case(0x36): break;
		//case(0x2E): break;
		//case(0x3E): break;

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
		//BMI - Branch if Minus
		case(0x30): Debugger::Log(" %.2X     BMI $%.4X                       A", RAM::ReadByte(oldPC + 1), oldPC + RAM::ReadByte(oldPC + 1) + 2); break;


		//PHP - Push Processor Status
		case(0x08): Debugger::Log("        PHP                             A"); break;
		//PHA - Push Accumulator
		case(0x48): Debugger::Log("        PHA                             A"); break;

		//PLA - Pull Accumulator
		case(0x68): Debugger::Log("        PLA                             A"); break;
		//PLP - Pull Processor Status
		case(0x28): Debugger::Log("        PLP                             A"); break;
	
		//RTI - Return from Interrupt
		case(0x40): Debugger::Log("        RTI                             A"); break;
			
	}
}

void CPUDebugLog() {	
	Debugger::Log("%-5X %.2X ", oldPC, opcode);	DebugASM();	Debugger::Log(":%.2X X:%.2X Y:%.2X P:%.2X SP:%.2X CYC:%3d\r", debugA, debugX, debugY, debugP, debugSP, debugPP);
	//printf("FLAGS: %x %x %x %x %x %x %x %x\n", C, Z, I, D, B, O ,V, N);
}

void SetZ(byte target) {
	(!target) ? Z = 1: Z = 0;
}

void SetN(byte target) {
	((target >> 7) & 0x1) ? N = 1: N = 0;
}

void PushByteToStack(byte data) {
	//printf("+%.2X at %.2X\n", data, SP);
	RAM::WriteByte(0x100 | SP, data);
	//for (int i = 0; i < 0xFF; i++) printf("%.2X ", RAM::ReadByte(0x100 | i));printf("\n");
	SP = SP - 1;

}

byte PullByteFromStack() {
	SP = SP + 1;
	//printf("-%.2X at %.2X\n", RAM::ReadByte(0x100 + SP), SP);
	//byte temp = RAM::ReadByte(0x100 | SP);
	//RAM::WriteByte(0x100 | SP, 0);
	//for (int i = 0; i < 0xFF; i++) printf("%.2X ", RAM::ReadByte(0x100 | i));printf("\n");
	return RAM::ReadByte(0x100 | SP);// temp;
}

void PushDWORDToStack(int data) {
	PushByteToStack((data >> 8) & 0xFF);
	PushByteToStack(data & 0xFF);
}

int PullDWORDFromStack() {
	int temp = PullByteFromStack();
	return temp | (PullByteFromStack() << 8);
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

	while (isRunning){ //&& (CC < 29780)) {
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

	if (debug) { debugPP = PP; debugN = N; debugC = C; debugA = A; debugX = X; debugY = Y; debugSP = SP; debugP = FlagsAsByte(); }

	switch (opcode) {
		//JMP - Jump - Absolute
		case(0x4C):	PC = RAM::ReadDWORD(PC + 0x1); PC--; CC = CC + 3; break;
		//JMP - Jump - Indirect
		case(0x6C): PC = RAM::ReadDWORD(RAM::ReadDWORD(PC+0x1)); PC--; CC = CC + 5; break;

		//LDX - Load X Register - Immediate
		case(0xA2):	X = RAM::ReadByte(PC + 1); SetZ(X); SetN(X); PC++; CC = CC + 2; break;
		//case(0xA6): break;
		//case(0xB6): break;
		case(0xAE): X = RAM::ReadByte(RAM::ReadDWORD(PC + 1)); SetZ(X); SetN(X); PC = PC +2; CC = CC + 4; break;
		//case(0xBE): break;

		//LDY - Load Y Register - Immediate
		case(0xA0):Y = RAM::ReadByte(PC + 1); SetZ(Y); SetN(Y); PC++; CC = CC + 2; break;
		//case(0xA4): break;
		//case(0xB4): break;
		case(0xAC): Y = RAM::ReadByte(RAM::ReadDWORD(PC + 1)); SetZ(Y); SetN(Y); PC = PC + 2; CC = CC + 4; break;
		//case(0xBC): break;

		//JSR - Jump to Subroutine
		case(0x20): PushDWORDToStack(PC + 2); PC = RAM::ReadDWORD(PC + 1); PC--; CC = CC + 6; break;

		//RTS - Return from Subroutine
		case(0x60): PC = PullDWORDFromStack(); CC = CC + 6; break;

		//NOP - No Operation
		case(0xEA): CC = CC + 2; break;

		//BIT - Bit Test Zero Page
		case(0x24):	V = (ByteAtZeroPage() >> 6) & 0x1; N = (ByteAtZeroPage() >> 7) & 0x1; !(A & ByteAtZeroPage()) ? Z = 1 : Z = 0;/*maybe remove z=0?*/CC = CC + 3; PC++; break;
				
		//LDA - Load Accumulator Immediate
		case(0xA9): A = RAM::ReadByte(PC + 1); SetZ(A); SetN(A); PC++; CC = CC + 2; break;
		//case(0xA5): break;
		//case(0xB5): break;
		case(0xAD): A = RAM::ReadByte(RAM::ReadDWORD(PC + 1)); SetZ(A); SetN(A); PC = PC + 2; CC = CC + 4; break;
		//case(0xBD): break;
		//case(0xB9): break;
		//case(0xA1): break;
		//case(0xB1): break;

		//ORA - Logical Inclusive OR Immediate
		case(0x09): A = A | RAM::ReadByte(PC + 1); SetZ(A); SetN(A); PC++; CC = CC + 2; break;
		//case(0x05): break;
		//case(0x15): break;
		//case(0x0D): break;
		//case(0x1D): break;
		//case(0x19): break;
		//case(0x01): break;
		//case(0x11): break;

		//AND - Logical AND
		case(0x29): A = A & RAM::ReadByte(PC + 1); SetZ(A); SetN(A); PC++; CC = CC + 2; break;
		//case(0x25): break;
		//case(0x35): break;
		////case(0x2D):  A = A & RAM::ReadByte(RAM::ReadDWORD(PC + 1)); SetZ(A); SetN(A); PC = PC + 2; CC = CC + 4; break;
		//case(0x3D): break;
		//case(0x39): break;
		//case(0x21): break;
		//case(0x31): break;

		//EOR - Exclusive OR Immediate
		case(0x49): A = A ^ RAM::ReadByte(PC + 1); SetZ(A); SetN(A); PC++; CC = CC + 2; break;
		//case(0x45): break;
		//case(0x55): break;
		//case(0x4D): break;
		//case(0x5D): break;
		//case(0x59): break;
		//case(0x41): break;
		//case(0x51): break;

		//LSR - Logical Shift Right
		case(0x4A): C = A & 1; A = A >> 1; SetZ(A); N = 0; /*SetN(A) should always output 0*/ CC = CC + 2; break;
		//case(0x46): break;
		//case(0x56): break;
		//case(0x4E): break;
		//case(0x5E): break;

		//ASL - Arithmetic Shift Left
		case(0x0A): C = A & 0x80; A = A << 1; SetZ(A); SetN(A); CC = CC + 2; break;
		//case(0x06): break;
		//case(0x16): break;
		//case(0x0E): break;
		//case(0x1E): break;

		//ROR - Rotate Right
		case(0x6A):  A = (A >> 1) | (C << 7); C = debugA & 1; SetZ(A); SetN(A); CC = CC + 2; break;
		//case(0x66): break;
		//case(0x76): break;
		//case(0x6E): break;
		//case(0x7E): break;

		//ROL - Rotate Left
		case(0x2A):  A = (A << 1) | C; C = debugA & 0x80; SetZ(A); SetN(A); CC = CC + 2; break;
		//case(0x26): break;
		//case(0x36): break;
		//case(0x2E): break;
		//case(0x3E): break;

		//CMP - Compare Immediate
		case(0xC9): C = (A >= RAM::ReadByte(PC + 1) ? 1 : 0); Z = (A == RAM::ReadByte(PC + 1) ? 1 : 0); SetN(A - RAM::ReadByte(PC + 1)); PC++; CC = CC + 2; break;
		//case(0xC5): break;
		//case(0xD5): break;
		//case(0xCD): break;
		//case(0xDD): break;
		//case(0xD9): break;
		//case(0xC1): break;
		//case(0xD1): break;

		//CPX - Compare X Register Immediate
		case(0xE0): C = (X >= RAM::ReadByte(PC + 1) ? 1 : 0); Z = (X == RAM::ReadByte(PC + 1) ? 1 : 0); SetN(X - RAM::ReadByte(PC + 1)); PC++; CC = CC + 2; break;
		//case(0xE4): break;
		//case(0xEC): break;

		//CPY - Compare Y Register Immediate
		case(0xC0): C = (Y >= RAM::ReadByte(PC + 1) ? 1 : 0); Z = (Y == RAM::ReadByte(PC + 1) ? 1 : 0); SetN(Y - RAM::ReadByte(PC + 1)); PC++; CC = CC + 2; break;
		//case(0xC4): break;
		//case(0xCC): break;

		//ADC - Add with Carry
		case(0x69): A = RAM::ReadByte(PC + 1) + A + C; SetZ(A); SetN(A); V = (debugN != N) && C ? 1 : 0; C = (A < debugA) ? 1 : 0; PC++; CC = CC + 2; break;
		//case(0x65): break;
		//case(0x75): break;
		//case(0x6D): break;
		//case(0x7D): break;
		//case(0x79): break;
		//case(0x61): break;
		//case(0x71): break;

		//SBC - Subtract with Carry
		case(0xE9):	A = A - RAM::ReadByte(PC + 1) - !C;	SetZ(A); SetN(A); C = (A > debugA) ? 0 : 1; V = (debugN != N) && C ? 1 : 0; PC++; CC = CC + 2; break;
		//case(0xE5): break;
		//case(0xF5): break;
		//case(0xED): break;
		//case(0xFD): break;
		//case(0xF9): break;
		//case(0xE1): break;
		//case(0xF1): break;

		//STX - Store X Register - Zero Page
		case(0x86): RAM::WriteByte(RAM::ReadByte(PC + 1), X); PC++; CC = CC + 3; break;
		//case(0x96): break; ^maybe broken
		case(0x8E): RAM::WriteByte(RAM::ReadDWORD(PC + 1), X); PC = PC +2; CC = CC + 4; break;

		//STY - Store Y Register - Zero Page
		case(0x84): RAM::WriteByte(RAM::ReadByte(PC + 1), Y); PC++; CC = CC + 3; break;
		//case(0x94): break; ^maybe broken
		case(0x8C): RAM::WriteByte(RAM::ReadDWORD(PC + 1), Y); PC = PC +2; CC = CC + 4; break;

		//STA - Store Accumulator Immediate
		case(0x85): RAM::WriteByte(RAM::ReadByte(PC + 1), A); PC++; CC = CC + 3; break;
		//case(0x95): break; 
		case(0x8D): RAM::WriteByte(RAM::ReadDWORD(PC + 1), A); PC = PC + 2; CC = CC + 4; break;
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
		//BMI - Branch if Minus
		case(0x30): if (N == 1) { PC = PC + RAM::ReadByte(PC + 1) + 1; CheckPageSkip(); CC++; } else { PC++; } CC = CC + 2; break;
		//BNE - Branch if Not Equal
		case(0xD0): if (Z == 0) { PC = PC + RAM::ReadByte(PC + 1) + 1; CheckPageSkip(); CC++; }	else { PC++; } CC = CC + 2; break;
		//BPL - Branch if Positive
		case(0x10): if (N == 0) { PC = PC + RAM::ReadByte(PC + 1) + 1; CheckPageSkip(); CC++; }	else { PC++; } CC = CC + 2; break;
		//BVC - Branch if Overflow Clear
		case(0x50): if (V == 0) { PC = PC + RAM::ReadByte(PC + 1) + 1; CheckPageSkip(); CC++; } else { PC++; } CC = CC + 2; break;
		//BVS - Branch if Overflow Set
		case(0x70): if (V == 1) { PC = PC + RAM::ReadByte(PC + 1) + 1; CheckPageSkip(); CC++; } else { PC++; } CC = CC + 2; break;

		//SEC - Set Carry Flag
		case(0x38):	C = 1; CC = CC + 2; break;
		//SED - Set Decimal Flag
		case(0xF8): D = 1; CC = CC + 2; break;
		//SEI - Set Interrupt Disable
		case(0x78): I = 1; CC = CC + 2; break;

		//CLC - Clear Carry Flag
		case(0x18): C = 0; CC = CC + 2; break;
		//CLD - CLD - Clear Decimal Mode
		case(0xD8): D = 0; CC = CC + 2; break;
		//CLI - Clear Interrupt Disable
		case(0x58): I = 0; CC = CC + 2; break;
		//CLV - Clear Overflow Flag
		case(0xB8): V = 0; CC = CC + 2; break;

		//INY - Increment Y Register
		case(0xC8): Y++; SetZ(Y); SetN(Y); CC = CC + 2; break;
		//INX - Increment X Register
		case(0xE8): X++; SetZ(X); SetN(X); CC = CC + 2; break;

		//DEY - Decrement Y Register
		case(0x88):	Y--; SetZ(Y); SetN(Y); CC = CC + 2; break;
		//DEX - Decrement X Register
		case(0xCA): X--; SetZ(X); SetN(X); CC = CC + 2; break;

		//TAY - Transfer Accumulator to Y
		case(0xA8):	Y = A; SetZ(Y); SetN(Y); CC = CC + 2; break;
		//TAX - Transfer Accumulator to X
		case(0xAA): X = A; SetZ(X); SetN(X); CC = CC + 2; break;

		//TYA - Transfer Y to Accumulator
		case(0x98): A = Y; SetZ(A); SetN(A); CC = CC + 2; break;
		//TXA - Transfer X to Accumulator
		case(0x8A): A = X; SetZ(A); SetN(A); CC = CC + 2; break;

		//TSX - Transfer Stack Pointer to X
		case(0xBA):	X = SP; SetZ(X); SetN(X); CC = CC + 2; break;
		//TXS - Transfer X to Stack Pointer
		case(0x9A):	SP = X; CC = CC + 2; break;

		//PHP - Push Processor Status
		case(0x08): PushByteToStack(0x10 | FlagsAsByte()); CC = CC + 3; break;
		//PHA - Push Accumulator
		case(0x48): PushByteToStack(A); CC = CC + 3; break;

		//PLA - Pull Accumulator
		case(0x68): A = PullByteFromStack(); SetZ(A); SetN(A); CC = CC + 4; break;
		//PLP - Pull Processor Status
		case(0x28): p = PullByteFromStack(); C = p & 0x1; Z = (p >> 1) & 0x1;	I = (p >> 2) & 0x1;	D = (p >> 3) & 0x1; B = 0; O = 1; V = (p >> 6) & 0x1;	N = (p >> 7) & 0x1;	CC = CC + 4; break;

		//RTI - Return from Interrupt
		case(0x40):  p = PullByteFromStack(); C = p & 0x1; Z = (p >> 1) & 0x1;	I = (p >> 2) & 0x1;	D = (p >> 3) & 0x1; B = 0; O = 1; V = (p >> 6) & 0x1;	N = (p >> 7) & 0x1; PC = PullDWORDFromStack(); PC--; CC = CC + 6; break;

		default: {
			if (debug) {
				Debugger::EndLogging();
				printf("\nUnknown opcode: %.2X at %.2X\n", opcode, PC);
				unknownOpcode = true;
			}
			isRunning = false;
		}
	}

	PC++;
	if (debug & !unknownOpcode) CPUDebugLog();
}