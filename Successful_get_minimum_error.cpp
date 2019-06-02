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
int clip_w = 12;
int clip_h = 12;


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
	file = fopen("championlist.txt","r");
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
	int left_border = 6;
	int top_border = 6;
	int index = 0;
	for (int i = 0; i < n_champs; i++)
	{
		if (strcmp(champion, champ_list[i]) == 0)
		{
			index = i;
			break;
		}
	}
	float t1 = (index % 14)*24;
	float t2 = (index / 14)*24;
	*position_x = (int)t1 + left_border;
	*position_y = (int)t2 + top_border;
}

bool isgrass(int map_index, const unsigned char* map)
{
	if (map[map_index + 1] == 0) return false;
	float rg = (float)map[map_index + 2] / (float)map[map_index + 1];
	float bg = (float)map[map_index] / (float)map[map_index + 1];
	if ((rg >= 0.93) && (rg <= 0.97)) return true;
	if ((bg >= 0.72) && (bg <= 0.76)) return true;
	return false;
}

int compare(unsigned char* portrait,const unsigned char* map, int x, int y, int map_pixels_per_line) //pixel by pixel
{
	long long diff = 0;
	int map_index = ((y - 810) * map_pixels_per_line + (x - 1650)) * 3;
	if (isgrass(map_index, map)) return 999999999;
	for (int i = y; i < y + clip_h; i++)
	{
		for (int j = x; j < x + clip_w; j++)
		{
			int map_index = ((i-810) * map_pixels_per_line + (j-1650)) * 3;			
			int portrait_index = ((i - y)*clip_w + (j - x)) * 3;
			int b_diff = (map[map_index] - portrait[portrait_index])*(map[map_index] - portrait[portrait_index]);
			int g_diff = (map[map_index + 1] - portrait[portrait_index + 1])*(map[map_index + 1] - portrait[portrait_index + 1]);
			int r_diff = (map[map_index+2] - portrait[portrait_index+2])*(map[map_index+2] - portrait[portrait_index+2]);
			diff = diff + r_diff + g_diff + b_diff;
		}
	}
	diff /= (clip_w*clip_h * 3);
	return diff;
}


/*
int compare(unsigned char* portrait, const unsigned char* map, int x, int y, int map_pixels_per_line) // important points
{
	long long diff = 0;
	for (int i = y; i < y + clip_h; i+=11)
	{
		for (int j = x; j < x + clip_w; j+=11)
		{
			int map_index = ((i - 810) * map_pixels_per_line + (j - 1650)) * 3;
			int portrait_index = ((i - y)*clip_w + (j - x)) * 3;
			int b_diff = (map[map_index] - portrait[portrait_index])*(map[map_index] - portrait[portrait_index]);
			int g_diff = (map[map_index + 1] - portrait[portrait_index + 1])*(map[map_index + 1] - portrait[portrait_index + 1]);
			int r_diff = (map[map_index + 2] - portrait[portrait_index + 2])*(map[map_index + 2] - portrait[portrait_index + 2]);
			diff = diff + r_diff + g_diff + b_diff;
		}
	}
	diff /= (4 * 3);
	return diff;
}*/

/*
int compare(unsigned char* portrait, const unsigned char* map, int x, int y, int map_pixels_per_line)  // average
{
	long long diff = 0;
	long long b_portrait = 0;
	long long g_portrait = 0;
	long long r_portrait = 0;
	long long b_map = 0;
	long long g_map = 0;
	long long r_map = 0;
	for (int i = y; i < y + clip_h; i++)
	{
		for (int j = x; j < x + clip_w; j++)
		{
			int map_index = ((i - 810) * map_pixels_per_line + (j - 1650)) * 3;
			int portrait_index = ((i - y)*clip_w + (j - x)) * 3;
			b_portrait += portrait[portrait_index];
			g_portrait += portrait[portrait_index+1];
			r_portrait += portrait[portrait_index+2];

			b_map += map[map_index];
			g_map += map[map_index+1];
			r_map += map[map_index+2];
		}
	}
	b_portrait /= (clip_w*clip_h);
	g_portrait /= (clip_w*clip_h);
	r_portrait /= (clip_w*clip_h);
	b_map /= (clip_w*clip_h);
	g_map /= (clip_w*clip_h);
	r_map /= (clip_w*clip_h);
	diff = (b_portrait - b_map)*(b_portrait - b_map) + (g_portrait - g_map)*(g_portrait - g_map)*(g_portrait - g_map)*(g_portrait - g_map) + (r_portrait - r_map)*(r_portrait - r_map);
	return diff;
}*/


void print(const unsigned char* str)
{
	FILE* f;
	f = fopen("test1.txt", "w");
	for (int i = 0; i < 268*268; i++)
	{
		int index = i * 3;
		fprintf(f, "%d %d %d %d %d\n", i % (268)+1650, i / 268+810, str[index],str[index+1],str[index+2]);

		SDL_Rect tmp = { i % (268),i / 268,1,1 };
		SDL_SetRenderDrawColor(renderer, str[index+2], str[index+1], str[index], 255);
		SDL_RenderFillRect(renderer, &tmp);
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

	window2 = SDL_CreateWindow(
		"WONDERFUL2",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		screen_width, screen_height, SDL_WINDOW_SHOWN);

	renderer2 = SDL_CreateRenderer(window2,
		-1, SDL_RENDERER_SOFTWARE);
		
	
	Screen_Grab::init(268,268);

	SDL_Surface *clip_buffer = SDL_CreateRGBSurfaceWithFormat(0, clip_w, clip_h, 24, SDL_PIXELFORMAT_BGR24);
	SDL_Surface *draw_buffer = SDL_CreateRGBSurfaceWithFormat(0, screen_width, screen_height, 24, SDL_PIXELFORMAT_BGR24);
	SDL_Surface *screen = SDL_GetWindowSurface(window);
	SDL_Surface *screen2 = SDL_GetWindowSurface(window2);

	SDL_Surface *original_image = IMG_Load("lolchamps.jpg");
	SDL_Surface *champ_image = SDL_CreateRGBSurfaceWithFormat(0, clip_w, clip_h, 24, SDL_PIXELFORMAT_BGR24);

	SDL_SetSurfaceBlendMode(draw_buffer, SDL_BLENDMODE_NONE);
	SDL_SetSurfaceBlendMode(champ_image, SDL_BLENDMODE_NONE);


	unsigned char *tmp_clip_buffer = (unsigned char*)malloc(sizeof(unsigned char) * 268 * 268 * 3);
	unsigned char *tmp1_clip_buffer = (unsigned char*)malloc(sizeof(unsigned char) * 268 * 268 * 3);

	Init_Champion_List();
	printf("Enter champion name:");
	char* champion = (char*)malloc(sizeof(char) * 20);
	fgets(champion, 20, stdin);
	int length = strlen(champion);
	if (champion[length - 1] == '\n') champion[length - 1] = '\0';
	int x = 0, y = 0;
	Find_Champion(champion, &x, &y);
	printf("%d %d\n", x, y);

	SDL_Rect champ_capture = { x,y,clip_w,clip_h };
	SDL_Rect tmp_champ_capture = { 0,0,clip_w,clip_h };
	SDL_BlitScaled(original_image, &champ_capture, champ_image, &tmp_champ_capture);

	/*
	SDL_Rect champ_capture = {5,5,clip_w,clip_h };
	SDL_Rect tmp_champ_capture = { 0,0,clip_w,clip_h };
	SDL_BlitScaled(original_image, &champ_capture, champ_image, &tmp_champ_capture);
	*/

	//For testing purpose

	SDL_BlitScaled(champ_image, &tmp_champ_capture, screen2, NULL);
	SDL_UpdateWindowSurface(window2);
	
	
	
	//for (;;)
	//{
		//consume all window events first
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				exit(0);
			}
		}

		FILE *f;
		f = fopen("test2.txt", "w");
		Screen_Grab::grab(tmp1_clip_buffer, 268, 268, 1650, 810);
		Rect::copy(tmp_clip_buffer, 0, 0, 268, 268, tmp1_clip_buffer, 268, 268);
		SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
		SDL_RenderClear(renderer);
		print(tmp_clip_buffer);
		SDL_RenderPresent(renderer);
		for (int y = 815; y < 1060; y++)  //250
		{ 
			for (int x = 1655; x < 1900; x++)  //250
			{
				long long diff = compare((unsigned char*)champ_image->pixels, tmp_clip_buffer,x,y,268);
				fprintf(f,"%d\t%d\t%lli\n",x,y, diff);
			}
		}
		fclose(f);
		getchar();
	return 0;
}