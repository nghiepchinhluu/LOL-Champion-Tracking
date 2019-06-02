#pragma once


namespace SetupScreen
{
	int previous_change_time;
	SDL_Texture* ResolutionText1;
	SDL_Texture* ResolutionText2;

	SDL_Texture* AutopingText1;
	SDL_Texture* AutopingText2;
	SDL_Texture* AutopingText3;

	SDL_Texture* ChampselectText1;
	SDL_Texture* ChampselectText2;

	SDL_Texture* Back;

	SDL_Texture* ChampName;

	SDL_Rect ResolutionText1_position;
	SDL_Rect ResolutionText2_position;
	SDL_Rect AutopingText1_position;
	SDL_Rect AutopingText2_position;
	SDL_Rect AutopingText3_position;
	SDL_Rect ChampselectText1_position;
	SDL_Rect ChampselectText2_position;
	SDL_Rect Back_position;
	SDL_Rect ChampName_position;


	char Text1[] = "Set resolution to";
	char Text5[] = "1920 x 1080";

	char Text2[] = "Activate auto ping";
	char Text3[] = "By set up Danger ping";
	char Text6[] = "as your middle mouse";

	char Text4[] = "Champion";
	char Text7[] = "Select";

	char Text8[] = "Back";

	SDL_Color color;
	bool MouseInText;

	void Init()
	{
		color = { 255,255,255,255 };
		Write(&ResolutionText1,Text1, font, color, renderer);
		Write(&ResolutionText2,Text5, font, color, renderer);

		Write(&AutopingText1,Text2, font, color, renderer);
		Write(&AutopingText2,Text3, font, color, renderer);
		Write(&AutopingText3,Text6, font, color, renderer);

		Write(&ChampselectText1,Text4, font, color, renderer);
		Write(&ChampselectText2,Text7, font, color, renderer);

		Write(&Back,Text8, font, color, renderer);

		ResolutionText1_position = { 50,40,250,40 };
		ResolutionText2_position = { 70,80,210,40 };

		AutopingText1_position = { 390,40,250, 40 };
		AutopingText2_position = { 390,80,250, 40 };
		AutopingText3_position = { 390,120,250, 40 };

		ChampselectText1_position = { 240,240,200,50 };
		ChampselectText2_position = { 270,290,140,50 };

		Back_position = { 0, screen_height - 50, 80, 50 };

		MouseInText = false;
	}

	void Process()
	{
		int MouseClick_x = 0, MouseClick_y = 0;
		POINT Mouse;
		// Process
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				exit(0);
			}
			else if (event.type == SDL_MOUSEBUTTONDOWN)		//Handle options
			{
				if (event.button.button == SDL_BUTTON_LEFT)
				{
					MouseClick_x = event.button.x;
					MouseClick_y = event.button.y;
					if ((MouseClick_x >= 50) && (MouseClick_x <= 300) && (MouseClick_y >= 40) && (MouseClick_y <= 120))
					{
						Mix_PlayChannel(-1, Enter_Wav, 0);
						Screen_State = 2;
					}
					if ((MouseClick_x >= 390) && (MouseClick_x <= 640) && (MouseClick_y >= 40) && (MouseClick_y <= 160))
					{
						Mix_PlayChannel(-1, Enter_Wav, 0);
						Screen_State = 3;
					}
					if ((MouseClick_x >= 240) && (MouseClick_x <= 440) && (MouseClick_y >= 240) && (MouseClick_y <= 340))
					{
						Mix_PlayChannel(-1, Enter_Wav, 0);
						Screen_State = 4;
					}
					if ((MouseClick_x >= 0) && (MouseClick_x <= 80) && (MouseClick_y >= screen_height - 50) && (MouseClick_y <= screen_height))
					{
						Mix_PlayChannel(-1, Enter_Wav, 0);
						Screen_State = 0;
					}
				}
			}
		}

		if (Screen_State != 1) return;

		GetCursorPos(&Mouse);
		ScreenToClient(hwnd, &Mouse);

		// Resolution
		SDL_SetRenderDrawColor(renderer, 0, 150, 0, 255);
		if ((Mouse.x >= 50) && (Mouse.x <= 300) && (Mouse.y >= 40) && (Mouse.y <= 120))
		{
			SDL_Rect tmp = { 50,40,250,80 };
			SDL_RenderFillRect(renderer, &tmp);
			if (MouseInText == false) Mix_PlayChannel(-1, Mousein_Wav, 0);
			MouseInText = true;
		}
		else
		// Autoping
		if ((Mouse.x >= 390) && (Mouse.x <= 640) && (Mouse.y >= 40) && (Mouse.y <= 160))
		{
			SDL_Rect tmp = { 390,40,250, 120 };
			SDL_RenderFillRect(renderer, &tmp);
			if (MouseInText == false) Mix_PlayChannel(-1, Mousein_Wav, 0);
			MouseInText = true;
		}
		else
		// Champ select
		if ((Mouse.x >= 240) && (Mouse.x <= 440) && (Mouse.y >= 240) && (Mouse.y <= 340))
		{
			SDL_Rect tmp = { 240,240,200, 100 };
			SDL_RenderFillRect(renderer, &tmp);
			if (MouseInText == false) Mix_PlayChannel(-1, Mousein_Wav, 0);
			MouseInText = true;
		}
		else
		// Back
		if ((Mouse.x >= 0) && (Mouse.x <= 80) && (Mouse.y >= (screen_height - 50)) && (Mouse.y <= screen_height))
		{
			SDL_Rect tmp = { 0,screen_height - 50,80, 50 };
			SDL_RenderFillRect(renderer, &tmp);
			if (MouseInText == false) Mix_PlayChannel(-1, Mousein_Wav, 0);
			MouseInText = true;
		}
		else MouseInText = false;
		if (champion::name != NULL)
		{
			int length = strlen(champion::name);
			SDL_SetRenderDrawColor(renderer, 100, 0, 0, 255);
			SDL_RenderFillRect(renderer, &ChampName_position);
		}

		Background::Show(&Background::BackgroundList[Current_Background], renderer);

		// Render Texts
		SDL_RenderCopyEx(renderer, ResolutionText1, NULL, &ResolutionText1_position, 0, NULL, SDL_FLIP_NONE);
		SDL_RenderCopyEx(renderer, ResolutionText2, NULL, &ResolutionText2_position, 0, NULL, SDL_FLIP_NONE);
		SDL_RenderCopyEx(renderer, AutopingText1, NULL, &AutopingText1_position, 0, NULL, SDL_FLIP_NONE);
		SDL_RenderCopyEx(renderer, AutopingText2, NULL, &AutopingText2_position, 0, NULL, SDL_FLIP_NONE);
		SDL_RenderCopyEx(renderer, AutopingText3, NULL, &AutopingText3_position, 0, NULL, SDL_FLIP_NONE);
		SDL_RenderCopyEx(renderer, ChampselectText1, NULL, &ChampselectText1_position, 0, NULL, SDL_FLIP_NONE);
		SDL_RenderCopyEx(renderer, ChampselectText2, NULL, &ChampselectText2_position, 0, NULL, SDL_FLIP_NONE);
		SDL_RenderCopyEx(renderer, Back, NULL, &Back_position, 0, NULL, SDL_FLIP_NONE);
		if (champion::name != NULL)
		{
			int length = strlen(champion::name);
			ChampName_position = { screen_width / 2 - (length * 10), 350, length * 20,50 };
			SDL_RenderCopyEx(renderer, ChampName, NULL, &ChampName_position, 0, NULL, SDL_FLIP_NONE);
		}
	}
}