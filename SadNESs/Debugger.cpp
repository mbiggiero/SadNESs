#include "stdafx.h"
#include "Debugger.h"
#include "ROM.h"

static TTF_Font * font;
static SDL_Surface* debugSurface = NULL;
static SDL_Texture* debugTexture = NULL;

void drawText(std::string text, int text_size, int x, int y, Uint8 r, Uint8 g, Uint8 b)
{
	font = TTF_OpenFont("arial.ttf", text_size);
	SDL_Color color = { r,b,g };
	debugSurface = TTF_RenderText_Solid(font, text.c_str(), color);
	debugTexture = SDL_CreateTextureFromSurface(SDLGUI::GetDebugRenderer(), debugSurface);
	int texW = 0;
	int texH = 0;
	SDL_QueryTexture(debugTexture, NULL, NULL, &texW, &texH);
	SDL_Rect dstrect = { x, y, texW, texH };

	SDL_RenderCopy(SDLGUI::GetDebugRenderer(), debugTexture, NULL, &dstrect);
	SDL_RenderPresent(SDLGUI::GetDebugRenderer());

	TTF_CloseFont(font);
	SDL_DestroyTexture(debugTexture);

	//OutputDebugString(L"Debugger: OK\n");
	//drawText("Debugger test: OK", 12, 10, 10, 255, 255, 255);
}

#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>

void __cdecl Debugger::printd(const char *format, ...)
{
	char    buf[4096], *p = buf;
	va_list args;
	int     n;

	va_start(args, format);
	n = _vsnprintf(p, sizeof buf - 3, format, args); // buf-3 is room for CR/LF/NUL
	va_end(args);

	p += (n < 0) ? sizeof buf - 3 : n;

	while (p > buf  &&  isspace(p[-1]))
		*--p = '\0';

	*p++ = '\r';
	*p++ = '\n';
	*p = '\0';

	OutputDebugString(buf);
}

void Debugger::DumpHeader() {	//TODO rewrite
	printf("Dumping header...");
	printf("\n");
	printf("%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x ", loadedROM[0], loadedROM[1], loadedROM[2], loadedROM[3], loadedROM[4], loadedROM[5], loadedROM[6], loadedROM[7], loadedROM[8], loadedROM[9], loadedROM[10], loadedROM[11], loadedROM[12], loadedROM[13], loadedROM[14], loadedROM[15]);
	printf("\n");
	printf("\n");
	printf("Byte 4   \t \t%.1d \t \t Number of 16kB ROM banks.", loadedROM[4] & 0x255);
	printf("\n");
	printf("Byte 5   \t \t%.1d \t \t Number of 8kB VROM banks.", loadedROM[5] & 0x255);
	printf("\n");
	printf("Byte 6 bit 0\t \t%.1x \t \t 1 for vertical mirroring, 0 for horizontal mirroring.", loadedROM[6] >> 7 & 0x255);
	printf("\n");
	printf("Byte 6 bit 1\t \t%.1x \t \t 1 for battery-backed RAM at $6000-$7FFF.", loadedROM[6] >> 6 & 0x255);
	printf("\n");
	printf("Byte 6 bit 2\t \t%.1x \t \t 1 for a 512-byte trainer at $7000-$71FF.", loadedROM[6] >> 5 & 0x255);
	printf("\n");
	printf("Byte 6 bit 3\t \t%.1x \t \t 1 for a four-screen VRAM layout.", loadedROM[6] >> 4 & 0x255);
	printf("\n");
	printf("Byte 6 bit 4-7\t \t%.4x \t \t Four lower bits of ROM Mapper Type.", loadedROM[6] >> 3);
	printf("\n");
	printf("Byte 7 bit 0\t \t%.1x \t \t 1 for VS-System cartridges.", loadedROM[7] >> 7 & 0x255);
	printf("\n");
	printf("Byte 7 bit 4-7\t \t%.4x \t \t Four higher bits of ROM Mapper Type.", loadedROM[7] >> 3);
	printf("\n");
	printf("Byte 8   \t \t%.1x \t \t Number of 8kB RAM banks. Assume 1x8kB RAM page when this byte is zero.", loadedROM[8] & 0x255);
	printf("\n");
	printf("Byte 9 bit 0\t \t%.1x \t \t 1 for PAL cartridges, otherwise assume NTSC.", (loadedROM[9] >> 7));
	printf("\n");
	printf("\n");
	printf("Header dumped\n");
}
