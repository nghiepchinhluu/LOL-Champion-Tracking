#pragma once

namespace Background
{
	//FUNCTINOS GO HERE
	void Init(Background *background, const char* picname, const char* dataname, SDL_Renderer* renderer)
	{
		// Create Texture
		SDL_Surface *Surface = IMG_Load(picname);
		background->Texture = SDL_CreateTextureFromSurface(renderer, Surface);
		SDL_FreeSurface(Surface);

		SDL_SetTextureBlendMode(background->Texture, SDL_BLENDMODE_BLEND);
		SDL_SetTextureAlphaMod(background->Texture, 70);

		// Get data from txt file
		FILE *file = fopen(dataname, "r");
		fscanf(file, "%d\n", &background->Width);
		fscanf(file, "%d\n", &background->Height);
		fscanf(file, "%d\n", &background->WidthperFrame);
		fscanf(file, "%d\n", &background->MsperFrame);   
		fclose(file);
		// Setup starting frame
		background->FrameRect = { 0,0,background->WidthperFrame,background->Height };
	}

	void InitBackgroundList()
	{
		BackgroundList_Size = 4;
		BackgroundList = (Background*)malloc(sizeof(Background)*BackgroundList_Size);
		for (int i = 0; i < BackgroundList_Size; i++)
		{
			char num = (i + 48);
			char* BackgroundImageFile = (char*)malloc(sizeof(char) * 9);
			strcpy(BackgroundImageFile, "GIF");
			append(BackgroundImageFile, num);
			strcat(BackgroundImageFile, ".png");

			char* BackgroundDataFile = (char*)malloc(sizeof(char) * 9);
			strcpy(BackgroundDataFile, "GIF");
			append(BackgroundDataFile, num);
			strcat(BackgroundDataFile, ".txt");

			Init(&BackgroundList[i], BackgroundImageFile, BackgroundDataFile, renderer);
		}
	}

	void Show(Background *background, SDL_Renderer* renderer)
	{
		SDL_RenderCopyEx(renderer, background->Texture, &(background->FrameRect), NULL, 0, NULL, SDL_FLIP_NONE);
	}

	int ShuffleBackground(int Current_Background) // Choose Background
	{
		int Chosen_Background = rand() % BackgroundList_Size;
		while (Chosen_Background == Current_Background)
			Chosen_Background = rand() % BackgroundList_Size;
		return Chosen_Background;
	}

	void Update(Background* background)
	{
		background->FrameRect.x = (background->FrameRect.x + background->WidthperFrame) % background->Width;
	}
}