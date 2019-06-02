#pragma once
//include SDL header
#include "SDL2-2.0.8\include\SDL.h"
#include "SDL2-2.0.8\include\SDL_image.h"


namespace champion
{
	struct Rect_Capture // No starting point
	{
		int width;
		int height;
	};

	int number_of_champions=140;
	int pixel_threshold;
	char* name = NULL;
	Rect_Capture portrait_size = { 12,12 };
	SDL_Surface *portrait= SDL_CreateRGBSurfaceWithFormat(0, portrait_size.width, portrait_size.height, 24, SDL_PIXELFORMAT_BGR24);;
}