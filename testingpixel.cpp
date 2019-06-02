#pragma warning (disable:4996)
#include <iostream>
#include <assert.h>
#include <Windows.h>
#include <string.h>
#include <math.h>
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

SDL_Window *window = NULL;
SDL_Window *window2 = NULL;
SDL_Renderer *renderer = NULL;
SDL_Renderer *renderer2 = NULL;
int screen_width = 800;
int screen_height = 600;
int clip_w = 20;
int clip_h = 20;


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

void Print_rect_from_image(const char* filename, int src_x, int src_y, int width, int height, int screendest_x, int screendest_y)
{
	SDL_Surface *sprite_surface = IMG_Load(filename);
	SDL_Texture *sprite_texture = SDL_CreateTextureFromSurface(renderer, sprite_surface);
	SDL_FreeSurface(sprite_surface);
	SDL_SetTextureAlphaMod(sprite_texture, 255);
	SDL_Rect src;
	src.x = src_x;
	src.y = src_y;
	src.w = width;
	src.h = height;

	SDL_Rect dest;
	dest.x = screendest_x;
	dest.y = screendest_y;
	dest.w = width;
	dest.h = height;

	SDL_RenderCopyEx(renderer, sprite_texture, &src, &dest, 0, NULL, SDL_FLIP_NONE);
}


void Init_Champion_List()
{
	FILE *file;
	file = fopen("championlist.txt", "r");
	fscanf(file, "%d\n", &n_champs);
	champ_list = (char**)malloc(sizeof(char*)*n_champs);

	for (int i = 0; i < n_champs; i++)
	{
		champ_list[i] = (char*)malloc(sizeof(char) * 20);
		fgets(champ_list[i], 20, file);
		int length = strlen(champ_list[i]);
		if (champ_list[i][length - 1] == '\n') champ_list[i][length - 1] = '\0';
	}
}

void Find_Champion(char* champion, int* position_x, int* position_y)
{
	// Find champion's location in the image
	int left_border = 2;
	int top_border = 2;
	int index = 0;
	for (int i = 0; i < n_champs; i++)
	{
		if (strcmp(champion, champ_list[i]) == 0)
		{
			index = i;
			break;
		}
	}
	float t1 = (index % 14)*21.25;
	float t2 = (index / 14) * 21;
	*position_x = (int)t1 + left_border;
	*position_y = (int)t2 + top_border;
}

int compare(unsigned char* str1, const unsigned char* str2)
{
	long long diff = 0;
	for (int i = 0; i < clip_w*clip_h; i++)
	{
		int index = i * 3;
		int r_diff = (str1[i] - str2[i])*(str1[i] - str2[i]);
		int g_diff = (str1[i + 1] - str2[i + 1])*(str1[i + 1] - str2[i + 1]);
		int b_diff = (str1[i + 2] - str2[i + 2])*(str1[i + 2] - str2[i + 2]);
		diff = diff + r_diff + g_diff + b_diff;
	}
	diff /= (clip_w*clip_h * 3);
	return diff;
}

void print(unsigned char* a)
{
	FILE *f;
	f = fopen("lala.txt","w");
	for (int i = 0; i < 571*586; i++)
	{
		int index = i * 3;
		fprintf(f,"%d\t%d\t%d\t%d\n", a[index + 2], a[index + 1], a[index], (a[index + 2]+ a[index + 1]+ a[index])/3 );
	}
	fclose(f);
}



int main(int argc, char **argv)
{
	SDL_Init(SDL_INIT_VIDEO);

	window = SDL_CreateWindow(
		"WONDERFUL",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		screen_width, screen_height, SDL_WINDOW_SHOWN);

	renderer = SDL_CreateRenderer(window,
		-1, SDL_RENDERER_SOFTWARE);

/*	window2 = SDL_CreateWindow(
		"WONDERFUL2",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		screen_width, screen_height, SDL_WINDOW_SHOWN);
		
	renderer2 = SDL_CreateRenderer(window2,
		-1, SDL_RENDERER_SOFTWARE);*/


	Screen_Grab::init(clip_w, clip_h);

	SDL_Surface *clip_buffer = SDL_CreateRGBSurfaceWithFormat(0, clip_w, clip_h, 24, SDL_PIXELFORMAT_BGR24);
	SDL_Surface *draw_buffer = SDL_CreateRGBSurfaceWithFormat(0, screen_width, screen_height, 24, SDL_PIXELFORMAT_BGR24);
	SDL_Surface *screen = SDL_GetWindowSurface(window);
	SDL_Surface *screen2 = SDL_GetWindowSurface(window2);

	SDL_Surface *original_image = IMG_Load("map.jpg");
	print((unsigned char*)original_image->pixels);
	return 0;
}