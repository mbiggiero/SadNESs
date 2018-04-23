// SadNESs.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
FILE *fileptr;
char *buffer;
long filelen;
int hexCount;
int ander;
char memory;
SDL_Event event;
SDL_Renderer *renderer;
TTF_Font * font;
SDL_Texture * texture;
SDL_Surface * surface;


/*TODO:
organize this mess first
write debugger ui and use it for everything
implement cpu completely
test cpu with nestest
fix and tidy up rom loading
implement PPU without scrolling
test games with mapper 0/no scroll = donkey/popey/baloon
add vertical scrolling scrolling
test ice climber
add horizontal scrolling
test smb
implement apu
add top unrom
add cnrom
add mmc1
add mmc2
more test



*/


int PRG_size;
int CHR_size;
int RAM_size;	//0 = 1
int ROM_region;	//0 NTSC; 1 PAL
char CPU_memory[0xFFFF];
char PPU_memory[];
char A;
char X;
char Y;
int PC;
bool Flag_N;
bool Flag_V;
bool Flag_1;
bool Flag_B;
bool Flag_D;
bool Flag_I;
bool Flag_Z;
bool Flag_C;
//bool PPU_registers
char SP;
char opcode;
int cycleCount = 0;
int cycles = 5;


void validateRom() {
	if ((buffer[0] == 0x4e) && (buffer[1] == 0x45) && (buffer[2] == 0x53) && (buffer[3] == 0x1a)) {
		printf("NES rom detected.");
		std::cout << std::endl;
		std::cout << std::endl;
	}
	else {
		printf("Invalid ROM");
		std::cout << std::endl;
		std::cout << std::endl;
	}
}

void loadRom() {
	ander = 255;
	fileptr = fopen("donkey.nes", "rb");  // Open the file in binary mode//TODO replace with something safe
	fseek(fileptr, 0, SEEK_END);          // Jump to the end of the file
	filelen = ftell(fileptr);             // Get the current byte offset in the file
	rewind(fileptr);         // Jump back to the beginning of the file

	buffer = (char *)malloc((filelen + 1) * sizeof(char)); // Enough memory for file + \0
	fread(buffer, filelen, 1, fileptr); // Read in the entire file
	fclose(fileptr); // Close the file

	hexCount = 0;

	/*for (int i = 0; i < filelen; ++i) {

		printf("%.2x ", buffer[i] & ander);
		if (hexCount < 15) {
			hexCount++;
		}
		else {
			std::cout << std::endl;
			hexCount = 0;
		}
	}
	//std::cout << buffer[i] << " ";
	std::cout << std::endl;
	*/

	validateRom();
	PRG_size = buffer[4] & ander;
	CHR_size = buffer[5] & ander;
	RAM_size = buffer[8] & ander;
	ROM_region = (buffer[9] >> 7);
}



void openWindow() {
	//The window we'll be rendering to
	//SDL_Window* window = NULL;
	SDL_Window* window2 = NULL;

	//The surface contained by the window
	SDL_Surface* screenSurface = NULL;

	

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}
	else
	{
		//Create window
		/*window = SDL_CreateWindow("ShitNESs", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 256, 240, SDL_WINDOW_SHOWN);
		if (window == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		}*/

		window2 = SDL_CreateWindow("Debugger", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 300, 300, SDL_WINDOW_SHOWN);
		if (window2 == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		}

		/* Create a Render */
		renderer = SDL_CreateRenderer(window2, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if (renderer == nullptr) {
			std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;	
		}				
	}
	if (TTF_Init() < 0) {
		// Error handling code
	}
}

void dumpHeader() {



	printf("Dumping header...");
	std::cout << std::endl;
	printf("%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x ", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8], buffer[9], buffer[10], buffer[11], buffer[12], buffer[13], buffer[14], buffer[15]);
	std::cout << std::endl;
	std::cout << std::endl;
	printf("Byte 4   \t \t%.1d \t \t Number of 16kB ROM banks.", PRG_size);
	std::cout << std::endl;
	printf("Byte 5   \t \t%.1d \t \t Number of 8kB VROM banks.", CHR_size);
	std::cout << std::endl;
	printf("Byte 6 bit 0\t \t%.1x \t \t 1 for vertical mirroring, 0 for horizontal mirroring.", buffer[6] >> 7 & ander);
	std::cout << std::endl;
	printf("Byte 6 bit 1\t \t%.1x \t \t 1 for battery-backed RAM at $6000-$7FFF.", buffer[6] >> 6 & ander);
	std::cout << std::endl;
	printf("Byte 6 bit 2\t \t%.1x \t \t 1 for a 512-byte trainer at $7000-$71FF.", buffer[6] >> 5 & ander);
	std::cout << std::endl;
	printf("Byte 6 bit 3\t \t%.1x \t \t 1 for a four-screen VRAM layout.", buffer[6] >> 4 & ander);
	std::cout << std::endl;
	printf("Byte 6 bit 4-7\t \t%.4x \t \t Four lower bits of ROM Mapper Type.", buffer[6] >> 3);//probably broken
	std::cout << std::endl;
	printf("Byte 7 bit 0\t \t%.1x \t \t 1 for VS-System cartridges.", buffer[7] >> 7 & ander);
	std::cout << std::endl;
	printf("Byte 7 bit 4-7\t \t%.4x \t \t Four higher bits of ROM Mapper Type.", buffer[7] >> 3);//probably broken
	std::cout << std::endl;
	printf("Byte 8   \t \t%.1x \t \t Number of 8kB RAM banks. Assume 1x8kB RAM page when this byte is zero.", RAM_size);
	std::cout << std::endl;
	printf("Byte 9 bit 0\t \t%.1x \t \t 1 for PAL cartridges, otherwise assume NTSC.", ROM_region);
	std::cout << std::endl;


	std::cout << std::endl;
	printf("Header dumped");

}

void CPU_initialize() {

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
	CPU_memory[0x2002] = 0x0;//A0

	for (int i = 0; i < filelen; i++) {
		CPU_memory[0x8000 + i] = buffer[i + 0xF + 1];
		//printf("%x %x\n", 0x8000 + i, CPU_memory[0x8000 + i]&ander);
	}
}

void CPU_fetch() {
	opcode = CPU_memory[PC] & ander;
	printf("PC %x\n", PC);
}

void CPU_decode() {
	int addr = 0x0;
	switch (opcode&ander) { // 7/255 done!;

	case(0x10):
		//Flag_C = 0;
		printf("%x: \t%x\tBPL\t\t\tA:%.2x X:%x Y:%x SP:%x N:%x V:%x 1:%x B:%x D:%x I:%x Z:%x C:%x CYC:%d\n",
			PC, opcode&ander, A&ander, X, Y, SP, Flag_N, Flag_V, Flag_1, Flag_B, Flag_D, Flag_I, Flag_Z, Flag_C, cycleCount);
		if (Flag_N == 0x0) { PC = PC + (CPU_memory[PC + 2] & ander); cycleCount++; }
		else { PC = PC++; }
		cycles = cycles - 1;
		cycleCount = cycleCount + 2;
		break;

	case(0xAD):
		addr = (((CPU_memory[PC + 3] & ander) << 8) | (CPU_memory[PC + 2] & ander));
		//printf("%.2x and %.2x\n", (CPU_memory[PC + 3+0xF] & ander), (CPU_memory[PC +2+0xF]&ander));
		//printf("addr %x\n", ((CPU_memory[PC + 3 + 0xF] & ander)<<8) | (CPU_memory[PC + 2 + 0xF] & ander));
		//printf("addr == %x\n", addr);

		if (addr == 0x0) Flag_Z = 1; else Flag_Z = 0;
		A = CPU_memory[addr];
		Flag_N = (A >> 7);
		printf("%x: \t%x\tLDA\t\t\tA:%.2x X:%x Y:%x SP:%x N:%x V:%x 1:%x B:%x D:%x I:%x Z:%x C:%x CYC:%d\n",
			PC, opcode&ander, A&ander, X, Y, SP, Flag_N, Flag_V, Flag_1, Flag_B, Flag_D, Flag_I, Flag_Z, Flag_C, cycleCount);
		cycles = cycles - 1;
		PC = PC + 2;
		cycleCount = cycleCount + 4;
		break;
	case(0x18):
		Flag_C = 0;
		printf("%x: \t%x\tCLC\t\t\tA:%.2x X:%x Y:%x SP:%x N:%x V:%x 1:%x B:%x D:%x I:%x Z:%x C:%x CYC:%d\n",
			PC, opcode&ander, A&ander, X, Y, SP, Flag_N, Flag_V, Flag_1, Flag_B, Flag_D, Flag_I, Flag_Z, Flag_C, cycleCount);
		cycles = cycles - 1;
		cycleCount = cycleCount + 2;
		break;
	case(0x58):
		Flag_I = 0;
		printf("%x: \t%x\tCLI\t\t\tA:%.2x X:%x Y:%x SP:%x N:%x V:%x 1:%x B:%x D:%x I:%x Z:%x C:%x CYC:%d\n",
			PC, opcode&ander, A&ander, X, Y, SP, Flag_N, Flag_V, Flag_1, Flag_B, Flag_D, Flag_I, Flag_Z, Flag_C, cycleCount);
		cycles = cycles - 1;
		cycleCount = cycleCount + 2;
		break;
	case(0xD8):
		Flag_D = 0;
		printf("%x: \t%x\tCLD\t\t\tA:%.2x X:%x Y:%x SP:%x N:%x V:%x 1:%x B:%x D:%x I:%x Z:%x C:%x CYC:%d\n",
			PC, opcode&ander, A&ander, X, Y, SP, Flag_N, Flag_V, Flag_1, Flag_B, Flag_D, Flag_I, Flag_Z, Flag_C, cycleCount);
		cycles = cycles - 1;
		cycleCount = cycleCount + 2;
		break;
	case(0xB8):
		Flag_V = 0;
		printf("%x: \t%x\tCLV\t\t\tA:%.2x X:%x Y:%x SP:%x N:%x V:%x 1:%x B:%x D:%x I:%x Z:%x C:%x CYC:%d\n",
			PC, opcode&ander, A&ander, X, Y, SP, Flag_N, Flag_V, Flag_1, Flag_B, Flag_D, Flag_I, Flag_Z, Flag_C, cycleCount);
		cycles = cycles - 1;
		cycleCount = cycleCount + 2;
		break;

	case(0x78):
		Flag_I = 1;
		printf("%x: \t%x\tSEI\t\t\tA:%.2x X:%x Y:%x SP:%x N:%x V:%x 1:%x B:%x D:%x I:%x Z:%x C:%x CYC:%d\n",
			PC, opcode&ander, A&ander, X, Y, SP, Flag_N, Flag_V, Flag_1, Flag_B, Flag_D, Flag_I, Flag_Z, Flag_C, cycleCount);
		cycles = cycles - 1;
		cycleCount = cycleCount + 2;
		break;
	case(0x38):
		Flag_C = 1;
		printf("%x: \t%x\tSEC\t\t\tA:%.2x X:%x Y:%x SP:%x N:%x V:%x 1:%x B:%x D:%x I:%x Z:%x C:%x CYC:%d\n",
			PC, opcode&ander, A&ander, X, Y, SP, Flag_N, Flag_V, Flag_1, Flag_B, Flag_D, Flag_I, Flag_Z, Flag_C, cycleCount);
		cycles = cycles - 1;
		cycleCount = cycleCount + 2;
		break;
	case(0xF8):
		Flag_D = 1;
		printf("%x: \t%x\tSED\t\t\tA:%.2x X:%x Y:%x SP:%x N:%x V:%x 1:%x B:%x D:%x I:%x Z:%x C:%x CYC:%d\n",
			PC, opcode&ander, A&ander, X, Y, SP, Flag_N, Flag_V, Flag_1, Flag_B, Flag_D, Flag_I, Flag_Z, Flag_C, cycleCount);
		cycles = cycles - 1;
		cycleCount = cycleCount + 2;
		break;

		//	case(0xAA):
		//to do negative check
		//break;

	default:printf("\nInvalid opcode: %.2x\n", opcode&ander); cycles = 0;
	}
	PC++;
}

void PPU_initialize() {

}

void CPU_run() {
	int i = 0;
	while (cycles>0) {
		CPU_fetch();
		CPU_decode();
	}
}

void CPU_pause() {

}

void CPU_reset() {

}

void PPU_run() {
}

void PPU_pause() {
}

void PPU_reset() {
}

void drawText(std::string text, int text_size, int x, int y, Uint8 r, Uint8 g, Uint8 b)
{
	font = TTF_OpenFont("arial.ttf", text_size);
	SDL_Color color = { r,b,g };
	surface = TTF_RenderText_Solid(font, text.c_str(), color);
	texture = SDL_CreateTextureFromSurface(renderer, surface);
	int texW = 0;
	int texH = 0;
	SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
	SDL_Rect dstrect = { x, y, texW, texH };

	SDL_RenderCopy(renderer, texture, NULL, &dstrect);
	SDL_RenderPresent(renderer);
}

void updateTexts()
{
	drawText("Item menu selection", 50, 330, 16, 0, 0, 0);
	
}

int main(int argc, char* argv[])
{
	bool quit = false;

	openWindow();
	loadRom();
	//dumpHeader();
	//CPU_initialize();


	//OutputDebugString(L"asdfasdf\n");
	//test::printerino();
	

	//SDL_RenderPresent(renderer);

	drawText("Debugger test: OK", 24, 10, 10, 255, 255, 255);
	OutputDebugString(L"DEBUGGER: OK\n");



	//While the user hasn't quit
	while (quit == false)
	{

		//CPU_run(29780);
		//CPU_run();



		//While there's an event to handle
		while (SDL_PollEvent(&event))
		{
			//If the user has Xed out the window
			if (event.type == SDL_QUIT)
			{
				//Quit the program
				quit = true;
			}
		}
		
		SDL_Delay(16.67);
		
	}

	SDL_DestroyRenderer(renderer);
	TTF_CloseFont(font);
	SDL_DestroyTexture(texture);
	SDL_FreeSurface(surface);
	TTF_Quit();
	SDL_Quit();

	printf("ending");

	return 0;

}


