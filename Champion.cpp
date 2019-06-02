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

#pragma warning (disable:4996)


	void champion::init()
	{
		number_of_champions = 140;
		portrait_size = { 12,12 };
		portrait = SDL_CreateRGBSurfaceWithFormat(0, portrait_size.width, portrait_size.height, 24, SDL_PIXELFORMAT_BGR24);
	}

	void champion::Find_Champion_Position(int index, int* position_x, int* position_y)   // Find champion's location in the image
	{
		int left_border = 7;
		int top_border = 7;
		float t1 = (index % 14) * 24;
		float t2 = (index / 14) * 24;
		*position_x = (int)t1 + left_border;
		*position_y = (int)t2 + top_border;
	}

	char* champion::Get_Champion_Name(int index)
	{
		FILE *file;
		file = fopen("championlist.txt", "r");
		char* result = (char*)malloc(sizeof(char) * 20);
		for (int i = 0; i < index; i++)
		{
			fgets(result, 20, file);
		}
		fgets(result, 20, file);
		if (result[strlen(result) - 1] == '\n') result[strlen(result) - 1] == '\0';
		fclose(file);
		return result;
	}

	int champion::Get_Pixel_Threshold(int index)
	{
		FILE *file;
		file = fopen("limit.txt", "r");
		int result = 0;
		for (int i = 0; i < index; i++)
		{
			fscanf(file, "%d", &result);
		}
		fscanf(file, "%d", &result);
		fclose(file);
		return result;
	}

	void champion::Get_Champion(const char* image_file_name)
	{
		int x = 0, y = 0;
		data_champion_image = IMG_Load(image_file_name);
		int champion_index = champion::champion_select::Champion_Select(data_champion_image,number_of_champions);
		Find_Champion_Position(champion_index, &x, &y);
		name = Get_Champion_Name(champion_index);
		pixel_threshold = Get_Pixel_Threshold(champion_index);
		printf("You chose %s\n", name);
		printf("Hit Enter to begin tracking\n");
		getchar();
		SDL_Rect champ_capture = { x,y,portrait_size.width,portrait_size.height };
		SDL_Rect tmp_champ_capture = { 0,0,portrait_size.width,portrait_size.height };
		SDL_SetSurfaceBlendMode(portrait, SDL_BLENDMODE_NONE);
		SDL_BlitScaled(data_champion_image, &champ_capture, portrait, &tmp_champ_capture);
		SDL_DestroyWindow(champion::champion_select::window);
		printf("Tracking in progress!\n");
		printf("Now please set up your danger ping as your middle mouse!\n");
		printf("If the enemy jungle is detected, the application will beep\n");
		printf("Hit shift to acknowledge the position of enemny jungler and stop the beep.This will automatically ping on enemy jungler as well!\n");
	}