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
TTF_Font *font;

int screen_width = 640;
int screen_height = 480;


void Init()
{
	// Kill Console
	HWND windowHandle = GetConsoleWindow();
	ShowWindow(windowHandle, SW_HIDE);

	// Init Mixer
	Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
	music = Mix_LoadMUS("League of Legends.mp3");
	Mix_PlayMusic(music, -1);
	if (music == NULL)
	{
		printf("Failed loading music!");
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
	//TTF_SetFontOutline(font, 2);
}


SDL_Texture* Write(char* text, TTF_Font *font, SDL_Color color, SDL_Renderer* renderer)
{
	SDL_Color bgcolor = { 100,100,100,255 };
	SDL_Surface *textSurface = TTF_RenderText_Shaded(font, text, color,bgcolor);
	SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_FreeSurface(textSurface);
	return textTexture;
}

namespace Background
{
	struct Background
	{
		//YOPU CANNO)T UPT FU NTION INSIDE STRUCTS
		//PUT DATA
		SDL_Texture* Texture;
		int Width;
		int Height;
		int NumberofFrame;
		int WidthperFrame;
		int MsperFrame;
		SDL_Rect FrameRect;
	};
}

namespace Background
{
	//FUNCTINOS GO HERE
	Background Init(const char* picname, const char* dataname, SDL_Renderer* renderer)
	{
		Background background;
		// Create Texture
		SDL_Surface *Surface = IMG_Load(picname);
		background.Texture = SDL_CreateTextureFromSurface(renderer, Surface);
		SDL_FreeSurface(Surface);

		SDL_SetTextureBlendMode(background.Texture, SDL_BLENDMODE_BLEND);
		SDL_SetTextureAlphaMod(background.Texture, 70);
		
		// Get data from txt file
		FILE *file = fopen(dataname,"r");
		fscanf(file, "%d\n" , &background.Width);
		fscanf(file, "%d\n", &background.Height);
		fscanf(file, "%d\n", &background.WidthperFrame);
		fscanf(file, "%d\n", &background.NumberofFrame);
		fscanf(file, "%d\n", &background.MsperFrame);

		// Setup starting frame
		background.FrameRect = { 0,0,background.WidthperFrame,background.Height };

		return background;
	}

	void Show(Background background, SDL_Renderer* renderer)
	{
		SDL_RenderCopyEx(renderer, background.Texture, &background.FrameRect, NULL, 0, NULL, SDL_FLIP_NONE);
	}

	void Update(Background* background)
	{
		(*background).FrameRect.x = ( (*background).FrameRect.x + (*background).WidthperFrame ) % (*background).Width;
	}
}



namespace IntroScreen
{
	Background::Background BG;
	int previous_change_time;
	SDL_Texture* greetingText1;
	SDL_Texture* greetingText2;
	SDL_Rect greetingText1_position;
	SDL_Rect greetingText2_position;

	void Init()
	{
		// Setup Background
		BG = Background::Init("Intro.jpg", "Intro.txt",renderer);
		previous_change_time = clock();

		char Text1[] = "Hello Summoner";
		char Text2[] = "Welcome to the champion-tracking application!";

		SDL_Color color = { 255,255,255,255 };
		greetingText1=Write(Text1, font, color, renderer);
		greetingText2=Write(Text2, font, color, renderer);

		greetingText1_position = { screen_width / 2 - 100,20,200,70 };
		greetingText2_position = { screen_width / 2 - 300,100,600,50 };
	}


	void Process()
	{
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

			if (clock() - previous_change_time > BG.MsperFrame)
			{
				Background::Update(&BG);
				printf("%d\n", BG.FrameRect.x);
				previous_change_time = clock();
			}

			Background::Show(BG,renderer);
			SDL_RenderCopyEx(renderer, greetingText1, NULL, &greetingText1_position, 0, NULL, SDL_FLIP_NONE);
			SDL_RenderCopyEx(renderer, greetingText2, NULL, &greetingText2_position, 0, NULL, SDL_FLIP_NONE);

			SDL_RenderPresent(renderer);
		}
	}
}

namespace SetupScreen
{
	Background::Background BG;
	int previous_change_time;
	SDL_Texture* ResolutionText1;
	SDL_Texture* ResolutionText2;
	SDL_Texture* ClickHere;
	SDL_Rect ResolutionButton;

	SDL_Texture* AutopingText1;
	SDL_Texture* AutopingText2;
	SDL_Texture* AutopingText3;
	SDL_Texture* AutopingClickHere;
	SDL_Rect AutopingButton;

	SDL_Texture* ChampselectText1;
	SDL_Texture* ChampselectText2;
	SDL_Texture* ChampselectClickHere;
	SDL_Rect ChampselectButton;
	
	SDL_Rect ResolutionText1_position;
	SDL_Rect ResolutionText2_position;
	SDL_Rect ResolutionClickHere_position;
	SDL_Rect AutopingText1_position;
	SDL_Rect AutopingText2_position;
	SDL_Rect AutopingText3_position;	
	SDL_Rect AutopingClickHere_position;
	SDL_Rect ChampselectText1_position;
	SDL_Rect ChampselectText2_position;
	SDL_Rect ChampselectClickHere_position;

	void Init()
	{
		// Setup Background
		BG = Background::Init("Instruction.png", "Instruction.txt", renderer);
		previous_change_time = clock();

		char Text1[] = "Set resolution to";
		char Text5[] = "1920 x 1080";

		char Text2[] = "Activate auto ping";
		char Text3[] = "By set up Danger ping";
		char Text6[] = "as your middle mouse";

		char Text4[] = "Champion";
		char Text7[] = "Select";

		SDL_Color color = { 255,255,255,255 };
		ResolutionText1 = Write(Text1, font, color, renderer);
		ResolutionText2 = Write(Text5, font, color, renderer);
	
		AutopingText1 = Write(Text2, font, color, renderer);
		AutopingText2 = Write(Text3, font, color, renderer);
		AutopingText3 = Write(Text6, font, color, renderer);
		
	
		ChampselectText1 = Write(Text4, font, color, renderer);
		ChampselectText2 = Write(Text7, font, color, renderer);
	
		ResolutionText1_position = { 30,40,250,40 };
		ResolutionText2_position = { 30,80,250,40 };

		AutopingText1_position = { 370,40,250, 40 };
		AutopingText2_position = { 370,80,250, 40 };
		AutopingText3_position = { 370,120,250, 40 };

		ChampselectText1_position = { 220,280,200,50 };
		ChampselectText2_position = { 220,330,200,50 };
	
	}

	void Process()
	{
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

			if (clock() - previous_change_time > BG.MsperFrame)
			{
				Background::Update(&BG);
				previous_change_time = clock();
			}


			Background::Show(BG, renderer);


			SDL_RenderCopyEx(renderer, ResolutionText1, NULL, &ResolutionText1_position, 0, NULL, SDL_FLIP_NONE);
			SDL_RenderCopyEx(renderer, ResolutionText2, NULL, &ResolutionText2_position, 0, NULL, SDL_FLIP_NONE);
			
			SDL_RenderCopyEx(renderer, AutopingText1, NULL, &AutopingText1_position, 0, NULL, SDL_FLIP_NONE);
			SDL_RenderCopyEx(renderer, AutopingText2, NULL, &AutopingText2_position, 0, NULL, SDL_FLIP_NONE);
			SDL_RenderCopyEx(renderer, AutopingText3, NULL, &AutopingText3_position, 0, NULL, SDL_FLIP_NONE);
			
			SDL_RenderCopyEx(renderer, ChampselectText1, NULL, &ChampselectText1_position, 0, NULL, SDL_FLIP_NONE);
			SDL_RenderCopyEx(renderer, ChampselectText2, NULL, &ChampselectText2_position, 0, NULL, SDL_FLIP_NONE);
			
			SDL_RenderPresent(renderer);
		}
	}
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
	IntroScreen::Init();
	SetupScreen::Init();

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
		//IntroScreen::Process();
		SetupScreen::Process();
	}
	return 0;
}