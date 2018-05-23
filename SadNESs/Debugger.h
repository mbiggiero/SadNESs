#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include "SDLGUI.h"
extern byte* loadedROM;

class Debugger
{
public:
	static void DumpHeader();
	static void StartLogging();
	static void EndLogging();
	static void Log(const char*, ...);
};

