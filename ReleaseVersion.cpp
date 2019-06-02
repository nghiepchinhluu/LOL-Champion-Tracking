#pragma warning (disable:4996)
#define WINVER 0x0500
#include <iostream>
#include <assert.h>
#include <Windows.h>
#include <string.h>
#include <math.h>
#include <time.h>
using namespace std;

//include SDL header
#include "SDL2-2.0.8\include\SDL.h"
#include "SDL2-2.0.8\include\SDL_image.h"

//load libraries
#pragma comment(lib,"SDL2-2.0.8\\lib\\x86\\SDL2.lib")
#pragma comment(lib,"SDL2-2.0.8\\lib\\x86\\SDL2main.lib")
#pragma comment(lib,"SDL2-2.0.8\\lib\\x86\\SDL2_image.lib")

#pragma comment(linker,"/subsystem:console")

// Variable declaration
int n_champs = 0;
char **champ_list;
int* limit;
int error_limit;
float ratio;
int previous_time;
int current_time;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Surface *original_image;
SDL_Surface *champ_image;
int screen_width = 680;
int screen_height = 488;
int clip_w = 12;
int clip_h = 12;
int minimap_size, minimap_x, minimap_y;

namespace Screen_Grab
{
	HDC screen;
	HDC clip;
	HBITMAP bitmap;
	BITMAPINFO bmi;

	void init(int w, int h)
	{
		screen = GetDC(NULL);
		clip = CreateCompatibleDC(screen);
		bitmap = CreateCompatibleBitmap(screen, w, h);
		SelectObject(clip, bitmap);

		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = w;
		bmi.bmiHeader.biHeight = h;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 24;
		bmi.bmiHeader.biCompression = 0;
		bmi.bmiHeader.biSizeImage = h * w * 3;
		bmi.bmiHeader.biXPelsPerMeter = 0;
		bmi.bmiHeader.biYPelsPerMeter = 0;
		bmi.bmiHeader.biClrUsed = 0;
		bmi.bmiHeader.biClrImportant = 0;
	}

	void grab(unsigned char*dest, int dest_w, int dest_h, int src_x, int src_y)
	{
		BitBlt(clip, 0, 0, dest_w, dest_h, screen, src_x, src_y, SRCCOPY);
		int r = GetDIBits(clip, bitmap, 0, dest_h, dest, &bmi, DIB_RGB_COLORS);
	}
}

namespace Rect
{
	void copy(unsigned char*dest, int dest_x, int dest_y, int dest_w, int dest_h, const unsigned char*src, int src_w, int src_h)
	{
		if (dest_y + src_h > dest_h) return;

		for (int y = 0; y < src_h; y++)
		{
			for (int x = 0; x < src_w; x++)
			{
				int dest_index = 3 * dest_w*(dest_y + y) + 3 * (x + dest_x);
				int src_index = 3 * src_w*(src_h - y - 1) + 3 * x;
				dest[dest_index + 0] = src[src_index + 0];
				dest[dest_index + 1] = src[src_index + 1];
				dest[dest_index + 2] = src[src_index + 2];
			}
		}
	}


	void fill(unsigned char*buffer, int buffer_width, int buffer_height, int rect_x, int rect_y, int rect_w, int rect_h, int r, int g, int b)
	{
		for (int y = 0; y < rect_h; y++)
		{
			for (int x = 0; x < rect_w; x++)
			{
				int index = 3 * (rect_y + y) * buffer_width + 3 * (x + rect_x);
				buffer[index] = r;
				buffer[index + 1] = g;
				buffer[index + 2] = b;
			}
		}

	}
}

namespace Champion
{
	void Init_Champion_List()
	{
		FILE *file;
		FILE *limitfile;
		file = fopen("championlist.txt", "r");
		limitfile = fopen("limit.txt", "r");
		fscanf(file, "%d\n", &n_champs);
		champ_list = (char**)malloc(sizeof(char*)*n_champs);
		limit = (int*)malloc(sizeof(int)*n_champs);
		for (int i = 0; i < n_champs; i++)
		{
			champ_list[i] = (char*)malloc(sizeof(char) * 20);
			fgets(champ_list[i], 20, file);
			int length = strlen(champ_list[i]);
			if (champ_list[i][length - 1] == '\n') champ_list[i][length - 1] = '\0';
			fscanf(limitfile, "%d", &limit[i]);
		}
		fclose(file);
		fclose(limitfile);
	}

	void Find_Champion(int index, int* position_x, int* position_y)
	{
		// Find champion's location in the image
		int left_border = 7;
		int top_border = 7;
		error_limit = limit[index];
		float t1 = (index % 14) * 24;
		float t2 = (index / 14) * 24;
		*position_x = (int)t1 + left_border;
		*position_y = (int)t2 + top_border;
	}
}

namespace Pixels
{

	bool Is_Map(int map_index, const unsigned char* map)
	{
		if ((map[map_index + 1] == 0) && (map[map_index] == 0) && (map[map_index + 2] == 0)) return true;
		if (map[map_index + 1] == 0) return false;
		float rg = (float)map[map_index + 2] / (float)map[map_index + 1];
		float bg = (float)map[map_index] / (float)map[map_index + 1];
		if (((rg >= 0.8) && (rg <= 1)) && ((bg >= 0.6) && (bg <= 0.8))) return true;   //grass
		if (((rg >= 0.6) && (rg <= 0.9)) && ((bg >= 0.6) && (bg <= 0.9)) && (map[map_index + 1] < 20)) return true; //wall
		return false;
	}

	int compare(unsigned char* portrait, const unsigned char* map, int x, int y, int map_pixels_per_line) //pixel by pixel
	{
		long long diff = 0;
		int map_index = ((y - 810) * map_pixels_per_line + (x - 1650)) * 3;
		if (Is_Map(map_index, map)) return 999999999;
		for (int i = y; i < y + clip_h; i++)
		{
			for (int j = x; j < x + clip_w; j++)
			{
				int map_index = ((i - 810) * map_pixels_per_line + (j - 1650)) * 3;
				int portrait_index = ((i - y)*clip_w + (j - x)) * 3;
				int b_diff = (map[map_index] - portrait[portrait_index])*(map[map_index] - portrait[portrait_index]);
				int g_diff = (map[map_index + 1] - portrait[portrait_index + 1])*(map[map_index + 1] - portrait[portrait_index + 1]);
				int r_diff = (map[map_index + 2] - portrait[portrait_index + 2])*(map[map_index + 2] - portrait[portrait_index + 2]);
				diff = diff + r_diff + g_diff + b_diff;
			}
		}
		diff /= (clip_w*clip_h * 3);
		return diff;
	}
}

namespace PromptUser
{
	/*void Get_ScreenSize()  // Initialize: Find champion in image file & resize the map (if resolution != 1920 1080) ---- not complete yet
	{
		int width, height;
		printf("Enter your screen resolution:");
		scanf("%d %d", &width, &height);
		getchar();
		ratio = 1;//1920 / (float)width;
		minimap_size = 268 / ratio;
		while (minimap_size % 4 != 0) minimap_size--;
		Screen_Grab::init(minimap_size, minimap_size);
		minimap_x = 1650 / ratio;
		minimap_y = 810 / ratio;
	}*/

	void Get_ScreenSize() //1920x1080 only
	{
		printf("This application only works with resolution 1920 x 1080 for now!\n");
		printf("Please set up your resolution to 1920 x 1080 before continue!\n");
		printf("Hit Enter to continue\n");
		getchar();
		minimap_size = 268;
		Screen_Grab::init(minimap_size, minimap_size);
		minimap_x = 1650;
		minimap_y = 810;
	}

	void Get_Champion()
	{
		printf("Now please select the champion you want to keep track of in the new window popped up\n");
		printf("Hit Enter to continue\n");
		getchar();
		window = SDL_CreateWindow(
			"WONDERFUL",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			screen_width, screen_height, SDL_WINDOW_SHOWN);

		renderer = SDL_CreateRenderer(window,
			-1, SDL_RENDERER_SOFTWARE);

		SDL_Surface *champ_select = SDL_CreateRGBSurfaceWithFormat(0, 680, 488, 24, SDL_PIXELFORMAT_BGR24);;
		SDL_Rect champ_select_size = { 0,0,680,488 };
		SDL_BlitScaled(original_image, NULL, champ_select, &champ_select_size);
		SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, champ_select);
		SDL_RenderCopyEx(renderer, texture, NULL, NULL, 0, NULL, SDL_FLIP_NONE);
		SDL_RenderPresent(renderer);

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
			if ((index >= 0) && (index < 140)) break;
		}
		int x = 0, y = 0;
		Champion::Find_Champion(index, &x, &y);
		printf("You chose %s\n", champ_list[index]);
		printf("Hit Enter to begin tracking\n");
		getchar();
		SDL_Rect champ_capture = { x,y,clip_w,clip_h };
		SDL_Rect tmp_champ_capture = { 0,0,clip_w,clip_h };
		SDL_BlitScaled(original_image, &champ_capture, champ_image, &tmp_champ_capture);
		SDL_DestroyWindow(window);
		printf("Tracking in progress!\n");
		printf("Now please set up your danger ping as your middle mouse!\n");
		printf("If the enemy jungle is detected, the application will beep\n");
		printf("Hit shift to acknowledge the position of enemny jungler and stop the beep.This will automatically ping on enemy jungler as well!\n");
	}
}


void ping(int dx, int dy)
{
	INPUT key;

	POINT oldpos;
	GetCursorPos(&oldpos);
	//ScreenToClient(hwnd, &oldpos);


	double fScreenWidth = ::GetSystemMetrics(SM_CXSCREEN) - 1;
	double fScreenHeight = ::GetSystemMetrics(SM_CYSCREEN) - 1;

	float x = (float)dx * 65536 / 1920;
	float y = (float)dy * 65536 / 1080;


	INPUT click[4] = { 0 };
	click[0].type = INPUT_MOUSE;

	click[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
	click[0].mi.dx = x;
	click[0].mi.dy = y;

	click[1] = click[0];
	click[1].mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
	SendInput(2, click, sizeof(INPUT));

	Sleep(20);

	click[2] = click[0];
	click[2].mi.dwFlags = MOUSEEVENTF_MIDDLEUP;

	click[3] = click[0];
	click[3].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
	click[3].mi.dx = (float)oldpos.x * 65536 / fScreenWidth;
	click[3].mi.dy = (float)oldpos.y * 65536 / fScreenHeight;
	SendInput(2, &click[2], sizeof(INPUT));
}

int main(int argc, char **argv)
{
	SDL_Init(SDL_INIT_VIDEO);

	original_image = IMG_Load("lolchamps.jpg");
	champ_image = SDL_CreateRGBSurfaceWithFormat(0, clip_w, clip_h, 24, SDL_PIXELFORMAT_BGR24);
	SDL_SetSurfaceBlendMode(champ_image, SDL_BLENDMODE_NONE);

	Champion::Init_Champion_List();
	PromptUser::Get_ScreenSize();

	PromptUser::Get_Champion();


	SDL_Surface *tmp_clip_buffer = SDL_CreateRGBSurfaceWithFormat(0, minimap_size, minimap_size, 24, SDL_PIXELFORMAT_BGR24);
	SDL_Surface *clip_buffer = SDL_CreateRGBSurfaceWithFormat(0, minimap_size, minimap_size, 24, SDL_PIXELFORMAT_BGR24);

	unsigned char *tmp1_clip_buffer = (unsigned char*)malloc(sizeof(unsigned char) * minimap_size * minimap_size * 3);


	bool acknowledge = false;
	bool beeping = false;
	previous_time = clock();

	for (;;)
	{
		//consume all window events first
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				exit(0);
			}
		}

		Screen_Grab::grab(tmp1_clip_buffer, minimap_size, minimap_size, minimap_x, minimap_y);
		Rect::copy((unsigned char*)tmp_clip_buffer->pixels, 0, 0, minimap_size, minimap_size, tmp1_clip_buffer, minimap_size, minimap_size);
		SDL_Rect src_size = { 0,0,minimap_size,minimap_size };
		SDL_BlitScaled(tmp_clip_buffer, &src_size, clip_buffer, &src_size); 

		long long min = MAXLONGLONG;
		int xo = 0;
		int yo = 0;
		for (int y = minimap_y; y < 1060; y++)  //250
		{
			for (int x = minimap_x; x < 1900; x++)  //250
			{
				long long diff = Pixels::compare((unsigned char*)champ_image->pixels, (unsigned char*)clip_buffer->pixels, x, y, 268);
				if (diff < min)
				{
					xo = x; // /ratio if multi-size
					yo = y; // /ratio if multi-size
					min = diff;
				}
			}
		}
		if ((min < error_limit) && (acknowledge == false) && (beeping==true) && (GetAsyncKeyState(VK_LSHIFT)))
		{
			ping(xo + 8, yo + 8); //+8 to make it at the center of the portrait
			acknowledge = true;
			previous_time = clock();
			beeping = false;
		}
		else if ((min < error_limit) && (acknowledge == false) && (clock() - previous_time > 7000))
		{
			Beep(500, 500);
			beeping = true;
		}
		else if (min >= error_limit) acknowledge = false;
	}
	return 0;
}