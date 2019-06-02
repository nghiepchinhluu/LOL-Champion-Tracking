#pragma warning (disable:4996)

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
Mix_Music *music;
Mix_Chunk *Mousein_Wav;
Mix_Chunk *Enter_Wav;
Mix_Chunk *Warning_Wav;
TTF_Font *font;
HWND hwnd;

int screen_width = 680;
int screen_height = 480;
int Screen_State = 0;
int Current_Background;
int previous_change_time;
int current_time;


void append(char* s, char c)
{
	int len = strlen(s);
	s[len] = c;
	s[len + 1] = '\0';
}


void InitLibrary()
{
	srand(time(0));
	// Kill Console
	HWND windowHandle = GetConsoleWindow();
	ShowWindow(windowHandle, SW_HIDE);

	hwnd = FindWindowA(NULL, "WONDERFUL");
	if (hwnd == NULL) printf("FAIL");


	// Init Mixer
	Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
	music = Mix_LoadMUS("League of Legends.mp3");
	Mix_PlayMusic(music, -1);
	if (music == NULL)
	{
		printf("Failed loading music!");
		getchar();
	}

	Mousein_Wav = Mix_LoadWAV("Mousein.wav");
	if (Mousein_Wav == NULL)
	{
		printf("Failed loading wav!");
		getchar();
	}

	Enter_Wav = Mix_LoadWAV("Enter.ogg");
	if (Enter_Wav == NULL)
	{
		printf("Failed loading wav!");
		getchar();
	}


	Warning_Wav = Mix_LoadWAV("Warning.wav");
	if (Enter_Wav == NULL)
	{
		printf("Failed loading wav!");
		getchar();
	}
	// Init Font
	TTF_Init();
	font = TTF_OpenFont("OpenSans-Bold.ttf", 100);
	if (font == NULL)
	{
		printf("Failed loading font!");
		getchar();
	}
}

void Write(SDL_Texture **textTexture, char* text, TTF_Font *font, SDL_Color color, SDL_Renderer* renderer)
{
	SDL_Surface *textSurface = TTF_RenderText_Solid(font, text, color);
	*textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	//SDL_FreeSurface(textSurface); //This makes program crash on release mode ??
}




#include "Champion_Data.h"
#include "Champion_Core.h"
#include "Screen_Grab.h"
#include "Ping.h"
#include "Pixels(new).h"
#include "Background_Data.h"
#include "Background_Core.h"
#include "IntroScreen.h"
#include "SetupScreen.h"
#include "AutopingScreen.h"
#include "ResolutionScreen.h"
#include "TrackingScreen.h"
#include "ChampSelectScreen.h"



int main(int argc, char **argv)
{
	SDL_Init(SDL_INIT_VIDEO);


	window = SDL_CreateWindow(
		"WONDERFUL",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		screen_width, screen_height, SDL_WINDOW_SHOWN);

	renderer = SDL_CreateRenderer(window,
		-1, SDL_RENDERER_SOFTWARE);

	InitLibrary();
	Background::InitBackgroundList();
	IntroScreen::Init();
	SetupScreen::Init();
	ResolutionScreen::Init();
	ChampSelectScreen::Init();
	AutopingScreen::Init();
	TrackingScreen::Init();

	Current_Background = Background::ShuffleBackground(-1);
	previous_change_time= clock();
	current_time = previous_change_time;
	
	bool TrackingStatus = false;

	for (;;)
	{
		current_time = clock();
		if (current_time - previous_change_time > Background::BackgroundList[Current_Background].MsperFrame)
		{
			Background::Update(&Background::BackgroundList[Current_Background]);
			if (Background::BackgroundList[Current_Background].FrameRect.x == 0) Current_Background = Background::ShuffleBackground(Current_Background);
			previous_change_time = current_time;
		}
	
		if (Screen_State==0) IntroScreen::Process();
		else if (Screen_State==1) SetupScreen::Process();
		else if (Screen_State == 2) ResolutionScreen::Process();
		else if (Screen_State == 3) AutopingScreen::Process();
		else if (Screen_State==4) ChampSelectScreen::Process();
		else if (Screen_State == 5)
		{
			if (TrackingStatus == false) Mix_PauseMusic();
			TrackingStatus = true;
			TrackingScreen::Process();
		}

		if (Screen_State != 5)
		{
			TrackingScreen::acknowledge = false;
			TrackingScreen::beeping = false;
			if (TrackingStatus) Mix_ResumeMusic();
			TrackingStatus = false;
		}

		SDL_RenderPresent(renderer);
	}
	return 0;
}