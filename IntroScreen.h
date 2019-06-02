#pragma once

namespace IntroScreen
{
	SDL_Texture* greetingText1;
	SDL_Texture* greetingText2;
	SDL_Texture* SwitchtoSetup;
	SDL_Texture* StartTracking;
	SDL_Rect greetingText1_position;
	SDL_Rect greetingText2_position;
	SDL_Rect SwitchtoSetup_position;
	SDL_Rect StartTracking_position;
	bool MouseInText;

	void Init()
	{
		char Text1[] = "Hello Summoner";
		char Text2[] = "Welcome to the champion-tracking application!";
		char Text3[] = "Setup";
		char Text4[] = "Start tracking!";

		SDL_Color color = { 255,255,255,255 };
	//	Write(greetingText1,Text1, font, color, renderer);
		SDL_Surface *textSurface = TTF_RenderText_Solid(font, Text1, color);
		greetingText1 = SDL_CreateTextureFromSurface(renderer, textSurface);

		Write(&greetingText2,Text2, font, color, renderer);
		Write(&SwitchtoSetup,Text3, font, color, renderer);
		Write(&StartTracking,Text4, font, color, renderer);

		greetingText1_position = { screen_width / 2 - 170,10,340,100 };
		greetingText2_position = { screen_width / 2 - 300,100,600,50 };
		SwitchtoSetup_position = { screen_width / 2 - 50,250,100,50 };
		StartTracking_position = { screen_width / 2 - 100,350,200,50 };

		MouseInText = false;
	}

	void Process()
	{
		// Setup
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
			if (event.type == SDL_MOUSEBUTTONDOWN)		//Handle options
			{
				if (event.button.button == SDL_BUTTON_LEFT)
				{
					MouseClick_x = event.button.x;
					MouseClick_y = event.button.y;
					if ((MouseClick_x >= (screen_width / 2 - 50)) && (MouseClick_x <= (screen_width / 2 + 50)) && (MouseClick_y >= 250) && (MouseClick_y <= 320))
					{
						Mix_PlayChannel(-1, Enter_Wav, 0);
						Screen_State = 1;
					}
					if ((MouseClick_x >= (screen_width / 2 - 100)) && (MouseClick_x <= (screen_width / 2 + 100)) && (MouseClick_y >= 350) && (MouseClick_y <= 450))
					{
						Mix_PlayChannel(-1, Enter_Wav, 0);
						Screen_State = 5;
					}
				}
			}
		}
		if (Screen_State != 0) return;

		GetCursorPos(&Mouse);
		ScreenToClient(hwnd, &Mouse);

		// SwitchtoSetup
		SDL_SetRenderDrawColor(renderer, 0, 150, 0, 100);
		if ((Mouse.x >= (screen_width / 2 - 50)) && (Mouse.x <= (screen_width / 2 + 50)) && (Mouse.y >= 250) && (Mouse.y <= 320))
		{
			SDL_Rect tmp = { (screen_width / 2 - 50), 250, 100, 50 };
			SDL_RenderFillRect(renderer, &tmp);
			if (MouseInText == false) Mix_PlayChannel(-1, Mousein_Wav, 0);
			MouseInText = true;
		}
		else
			// StartTracking
			if ((Mouse.x >= (screen_width / 2 - 100)) && (Mouse.x <= (screen_width / 2 + 100)) && (Mouse.y >= 350) && (Mouse.y <= 450))
			{
				SDL_Rect tmp = { (screen_width / 2 - 100),350,200, 50 };
				SDL_RenderFillRect(renderer, &tmp);
				if (MouseInText == false) Mix_PlayChannel(-1, Mousein_Wav, 0);
				MouseInText = true;
			}
			else MouseInText = false;

		Background::Show(&Background::BackgroundList[Current_Background], renderer);

		SDL_RenderCopyEx(renderer, greetingText1, NULL, &greetingText1_position, 0, NULL, SDL_FLIP_NONE);
		SDL_RenderCopyEx(renderer, greetingText2, NULL, &greetingText2_position, 0, NULL, SDL_FLIP_NONE);
		SDL_RenderCopyEx(renderer, SwitchtoSetup, NULL, &SwitchtoSetup_position, 0, NULL, SDL_FLIP_NONE);
		SDL_RenderCopyEx(renderer, StartTracking, NULL, &StartTracking_position, 0, NULL, SDL_FLIP_NONE);
	}
}