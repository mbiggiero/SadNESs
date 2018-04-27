// SadNESs.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "SDLGUI.h"
#include "Debugger.h"
#include "CPU.h"
#include "PPU.h"
#include "FileLoader.h"
#include "ROM.h"

/*TODO: NES
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

SDL_Event event;

bool debug = true;

void Initialize() {
	SDLGUI::Initialize();
	CPU::Initialize();
}

void Destroy() {
	SDLGUI::DestroySDL();
}

void Run(std::string romName) {
	bool quit = false;

	FileLoader::LoadRom(romName);

	if (FileLoader::ValidateRom()) {
		Debugger::DumpHeader();
		ROM::Setup();

		while (quit == false) {
			//CPU::Run();		
			while (SDL_PollEvent(&event)) {
				if (event.type == SDL_QUIT) {
					Destroy();
					quit = true;
				}
			}
			//SDL_RenderPresent(renderer);
			SDL_Delay(16);
		}
	}
	else { 
		printf("Run aborted"); 
	}
}

void Pause() {
	//TODO
}

void Reset() {
	//TODO
}

int main(int argc, char* argv[])
{
	Initialize();
	Run("donkey.nes");		
	return 0;
}