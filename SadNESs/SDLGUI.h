#include <SDL.h>
#include <SDL_ttf.h>
#include <SadNESs.h>
#pragma once
class SDLGUI
{
public:
	SDLGUI();
	static int Initialize();
	static SDL_Renderer* GetDebugRenderer();
	static void DestroySDL();


};

