#pragma warning (disable:4996)

//load libraries
#pragma comment(lib,"SDL2-2.0.8\\lib\\x86\\SDL2.lib")
#pragma comment(lib,"SDL2-2.0.8\\lib\\x86\\SDL2main.lib")
#pragma comment(lib,"SDL2-2.0.8\\lib\\x86\\SDL2_image.lib")

#pragma comment(linker,"/subsystem:console")

//Include functions header files

#include "Champion_Data.h"
#include "Champion_Core.h"
#include "Screen_Grab.h"
#include "Pixels(new).h"
#include "Ping.h"

//General include

#include <iostream>
#include <time.h>

using namespace std;

//include SDL header
#include "SDL2-2.0.8\include\SDL.h"
#include "SDL2-2.0.8\include\SDL_image.h"


struct Rectt
{
	int xo;
	int yo;
	int width;
	int height;
};

Rectt minimap;
Rectt GameScreen;


void Introduction() //1920x1080 only
{
	printf("This application only works with resolution 1920 x 1080 for now!\n");
	printf("Please set up your resolution to 1920 x 1080 before continue!\n");
	printf("Hit Enter to continue\n");
	getchar();
	minimap.width = 268;
	minimap.height = 268;
	Screen_Grab::init(minimap.width, minimap.height);
	minimap.xo = 1650;
	minimap.yo = 810;

	(Rectt)GameScreen = { 0,0,1920,1080 };
}


int main(int argc, char **argv)
{
	SDL_Init(SDL_INIT_VIDEO);


	Introduction();

	champion::Get_Champion("lolchamps.jpg");   // Champion portrait will be saved in champion::portrait


	SDL_Surface *clip_buffer = SDL_CreateRGBSurfaceWithFormat(0, minimap.width, minimap.height, 24, SDL_PIXELFORMAT_BGR24);
	unsigned char *tmp_clip_buffer = (unsigned char*)malloc(sizeof(unsigned char) * minimap.width * minimap.height * 3);

	bool acknowledge = false;
	bool beeping = false;

	int previous_time = clock();

	FILE *file = fopen("test1.txt", "w");
	for (;;)
	{
		// Consume all window events first
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				exit(0);
			}
		}

		// Grab pixel buffer for minimap
		Screen_Grab::grab(tmp_clip_buffer, minimap.width, minimap.height, minimap.xo, minimap.yo);
		Pixels::copy((unsigned char*)clip_buffer->pixels, 0, 0, minimap.width, minimap.height, tmp_clip_buffer, minimap.width, minimap.height);
		

		// Find the pixel block which has the lowest pixel error, compared to the champion portrait
		long long error_min = MAXLONGLONG;
		int x_min = 0;
		int y_min = 0;
		for (int y = minimap.yo; y < 1060; y++)  //250
		{
			for (int x = minimap.xo; x < 1900; x++)  //250
			{
				long long diff = Pixels::compare((unsigned char*)champion::portrait->pixels, (unsigned char*)clip_buffer->pixels, x, y, champion::portrait_size.width, champion::portrait_size.height, minimap.xo, minimap.yo, minimap.width);
				if (diff < error_min)
				{
					x_min = x;
					y_min = y;
					error_min = diff;

				}
			}
		}

		printf("%llu\t%d\t%d\n", error_min, x_min, y_min);
		if ((error_min < champion::pixel_threshold) && (acknowledge == false) && (beeping == true) && (GetAsyncKeyState(VK_LSHIFT)))
		{
			ping(x_min + 8, y_min + 8); //+8 to make it at the center of the portrait
			acknowledge = true;
			previous_time = clock();
			beeping = false;
		}
		else if ((error_min < champion::pixel_threshold) && (acknowledge == false) && (clock() - previous_time > 7000))
		{
			Beep(500, 500);
			beeping = true;
			printf("BEEP!!!\n");
		}
		else if (error_min >= champion::pixel_threshold) acknowledge = false;
	}
	fclose(file);
	return 0;
}