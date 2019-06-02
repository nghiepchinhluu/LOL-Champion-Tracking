#include <stdio.h>
#include <Windows.h>
#include <time.h>
#include "SDL2-2.0.8/include/SDL.h"
#include "SDL2-2.0.8/include/SDL_ttf.h"
#include "SDL2-2.0.8/include/SDL_image.h"
#include "SDL2-2.0.8/include/SDL_mixer.h"
// Load library

#pragma comment(lib,"SDL2-2.0.8\\lib\\x86\\SDL2.lib")
#pragma comment(lib,"SDL2-2.0.8\\lib\\x86\\SDL2main.lib")
#pragma comment(lib,"SDL2-2.0.8\\lib\\x86\\SDL2_ttf.lib")
#pragma comment(lib,"SDL2-2.0.8\\lib\\x86\\SDL2_image.lib")
#pragma comment(lib,"SDL2-2.0.8\\lib\\x86\\SDL2_mixer.lib")

#pragma comment(linker,"/subsystem:console")

SDL_Renderer *renderer = NULL;
SDL_Window *window = NULL;
SDL_Texture *Background_Texture = NULL;
Mix_Music *music;
TTF_Font *font;


SDL_Texture* Write(char* text, TTF_Font *font, SDL_Color color, SDL_Renderer* renderer, int pos_x, int pos_y)
{
	SDL_Surface *textSurface = TTF_RenderText_Solid(font, "Whatever", color);
	SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_FreeSurface(textSurface);
	return textTexture;
}














void Init()
{
	// Kill Console
	HWND windowHandle = GetConsoleWindow();
	ShowWindow(windowHandle, SW_HIDE);

	// Init Mixer
	Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
	music = Mix_LoadMUS("League of Legends.mp3");
	Mix_PlayMusic(music, -1);

	// Init Font
	TTF_Init();
	font = TTF_OpenFont("OpenSans-Bold.ttf", 100);
	if (font == NULL)
	{
		printf("Failed!");
	}

	// Setup Background
	SDL_Surface *Background_Surface = IMG_Load("GIF.jpg");
	Background_Texture = SDL_CreateTextureFromSurface(renderer, Background_Surface);
	SDL_FreeSurface(Background_Surface);
}


int main(int argc, char **argv)
{
	window = SDL_CreateWindow(
		"WONDERFUL",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		640, 480, SDL_WINDOW_SHOWN);

	renderer = SDL_CreateRenderer(window,
		-1, SDL_RENDERER_SOFTWARE);

	Init();

	SDL_Rect background_frame = { 0,0,500,281 };
	SDL_RenderCopyEx(renderer, Background_Texture, &background_frame, NULL, 0, NULL, SDL_FLIP_NONE);
	SDL_RenderPresent(renderer);

	int previous_change_time = clock();
	SDL_Rect dest;
	dest.x = 320;
	dest.y = 240;
	dest.w = 100;
	dest.h = 100;
	SDL_Color color = { 255,255,255,255 };
	SDL_Surface *textSurface = TTF_RenderText_Solid(font, "Whatever", color);
	SDL_Texture *sprite_texture = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_FreeSurface(textSurface);

	SDL_SetTextureAlphaMod(sprite_texture, 255);
	
	for (;;)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				exit(0);
			}
		}

		if (clock() - previous_change_time > 100)
		{
			background_frame.x = (background_frame.x + 500) % 25000;
			printf("%d\n", background_frame.x);
			previous_change_time = clock();
		}

		SDL_RenderCopyEx(renderer, Background_Texture, &background_frame, NULL , 0, NULL, SDL_FLIP_NONE);

		SDL_RenderCopyEx(renderer, sprite_texture, NULL, &dest, 0, NULL, SDL_FLIP_NONE);
		
		SDL_RenderPresent(renderer);
	}
	return 0;
}