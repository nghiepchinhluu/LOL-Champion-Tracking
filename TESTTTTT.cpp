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

	//champion::Get_Champion("lolchamps.jpg");   // Champion portrait will be saved in champion::portrait
	champion::data_champion_image = IMG_Load("lolchamps.jpg");

	SDL_Surface *clip_buffer = SDL_CreateRGBSurfaceWithFormat(0, minimap.width, minimap.height, 24, SDL_PIXELFORMAT_BGR24);
	unsigned char *tmp_clip_buffer = (unsigned char*)malloc(sizeof(unsigned char) * minimap.width * minimap.height * 3);

		// Find the pixel block which has the lowest pixel error, compared to the champion portrait
	FILE *f = fopen("championlist.txt", "r");
	char* tmp = (char*)malloc(sizeof(char) * 20);
	for (int i = 0; i < 140; i++)
	{
		fgets(tmp, 20, f);
		if (tmp[strlen(tmp) - 1] = '\n') tmp[strlen(tmp) - 1] = '\0';
		FILE *file = fopen(tmp, "w");

		int x = 0, y = 0;
		champion::Find_Champion_Position(i, &x, &y);
		SDL_Rect champ_capture = { x,y,champion::portrait_size.width,champion::portrait_size.height };
		SDL_Rect tmp_champ_capture = { 0,0,champion::portrait_size.width,champion::portrait_size.height };
		SDL_SetSurfaceBlendMode(champion::portrait, SDL_BLENDMODE_NONE);
		SDL_BlitScaled(champion::data_champion_image, &champ_capture, champion::portrait, &tmp_champ_capture);

		int previous_time = clock();
		for (;;)
		{
			Screen_Grab::grab(tmp_clip_buffer, minimap.width, minimap.height, minimap.xo, minimap.yo);
			Pixels::copy((unsigned char*)clip_buffer->pixels, 0, 0, minimap.width, minimap.height, tmp_clip_buffer, minimap.width, minimap.height);

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
			fprintf(file, "%llu\n", error_min);
			if (clock() - previous_time > 3000) break;
		}
		fclose(file);
	}
	return 0;
}