#pragma once
#define WINVER 0x0500
#include <iostream>
#include <assert.h>
#include <Windows.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "Champion.h"

using namespace std;

//include SDL header
#include "SDL2-2.0.8\include\SDL.h"
#include "SDL2-2.0.8\include\SDL_image.h"



int champion::champion_select::Champion_Select(SDL_Surface *data_champion_image, int number_of_champions)  // Return the index of the chosen champion
{
	printf("Now please select the champion you want to keep track of in the new window popped up\n");
	printf("Hit Enter to continue\n");
	getchar();

	// Initialize
	window = SDL_CreateWindow(
		"WONDERFUL",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		screen.width, screen.height, SDL_WINDOW_SHOWN);

	renderer = SDL_CreateRenderer(window,
		-1, SDL_RENDERER_SOFTWARE);

	// Showing champion select screen
	SDL_Surface *champ_select = SDL_CreateRGBSurfaceWithFormat(0, 680, 488, 24, SDL_PIXELFORMAT_BGR24);;
	SDL_Rect champ_select_size = { 0,0,680,488 };
	SDL_BlitScaled(data_champion_image, NULL, champ_select, &champ_select_size);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, champ_select);
	SDL_RenderCopyEx(renderer, texture, NULL, NULL, 0, NULL, SDL_FLIP_NONE);
	SDL_RenderPresent(renderer);

	// Get the position of user's mouse click
	SDL_Event event;
	int x_pos = 0, y_pos = 0;
	int index = -1;
	for (;;)
	{
		index = -1;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				exit(0);
			}
			else if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				if (event.button.button == SDL_BUTTON_LEFT)
				{
					x_pos = event.button.x;
					y_pos = event.button.y;
				}
			}
		}
		x_pos -= 3; // left border
		y_pos -= 3; // top border
		if (x_pos < 0) continue;
		if (y_pos < 0) continue;
		index = (y_pos / 48) * 14 + (x_pos / 48);
		if ((index >= 0) && (index < number_of_champions)) break;
	}
	return index;
}