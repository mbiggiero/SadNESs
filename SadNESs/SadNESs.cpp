#include "stdafx.h"
#include "SDLGUI.h"
#include "Debugger.h"
#include "CPU.h"
#include "PPU.h"
#include "FileLoader.h"
#include "ROM.h"

/*TODO: NES
implement cpu completely
test cpu with nestest
rewrite cpu as core for future projects
write ppu/io debugger ui and use it for everything
implement PPU without scrolling
test games with mapper 0/no scroll = donkey/popey/baloon
add vertical scrolling scrolling
test ice climber
add horizontal scrolling
test smb
implement apu
add unrom
add cnrom
add mmc1
add mmc2
more test
*/

SDL_Event event;

bool debug = true;
bool isRunning = false;

void Initialize() {
	isRunning = true;
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
		Debugger::StartLogging();

		while (quit == false) {
			CPU::Run();		
			while (SDL_PollEvent(&event)) {
				if (event.type == SDL_QUIT) {
					Destroy();
					quit = true;
				}
			}
			//SDL_RenderPresent(renderer);
			SDL_Delay(16);
		}
		isRunning = false;
	}
	else { 
		printf("Run aborted"); 
	}
}

void Pause() {
	isRunning = false;
}

void Reset() {
	isRunning = false;
}

int main(int argc, char* argv[])
{
	Initialize();
	Run("nestest.nes");		
	return 0;
}