#include "stdafx.h"
#include "SDLGUI.h"
#include "SadNESs.h"

static int SDLerror;
extern bool debug;

static SDL_Window* emulatorWindow = NULL;
static SDL_Window* debugWindow = NULL;
static SDL_Renderer *debugRenderer;

SDLGUI::SDLGUI()
{
	SDLerror = 0;
}

void OpenEmulatorWindow() {
	emulatorWindow = SDL_CreateWindow("ShitNESs", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 256, 240, SDL_WINDOW_SHOWN);
	if (emulatorWindow == NULL)
	{
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		SDLerror = 1;
	}

	//TODO: add main renderer later
}

void OpenDebugWindow() {
	debugWindow = SDL_CreateWindow("Debugger", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 300, 300, SDL_WINDOW_SHOWN);
	if (debugWindow == NULL)
	{
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		SDLerror = 1;
	}

	debugRenderer = SDL_CreateRenderer(debugWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (debugRenderer == nullptr) {
		printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
		SDLerror = 1;
	}

	SDL_RenderPresent(debugRenderer);
}

int SDLGUI::Initialize()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		SDLerror = 1;
	}
	if (TTF_Init() < 0) {
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		SDLerror = 1;
	}

	OpenEmulatorWindow();
	if (debug) {
		OpenDebugWindow();
	}

	return SDLerror;
}

SDL_Renderer* SDLGUI::GetDebugRenderer() {
	return debugRenderer;
}

void SDLGUI::DestroySDL() {
	SDL_DestroyRenderer(debugRenderer); 
	//TODO add emulator renderer to destroy

	TTF_Quit();
	SDL_Quit();
}

