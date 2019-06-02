#pragma once

namespace ResolutionScreen
{
	SDL_Surface* Image_Surface;
	SDL_Texture* Image_Texture;
	SDL_Texture* Back;
	SDL_Rect Back_position;
	bool MouseInText;
	char Text[] = "Back";

	void Init()
	{
		Image_Surface = IMG_Load("ThiswilllaterResolutionSetup.png");
		Image_Texture = SDL_CreateTextureFromSurface(renderer, Image_Surface);
		SDL_Color color = { 0,0,0,255 };
		Write(&Back,Text, font, color, renderer);
		Back_position = { 0, screen_height - 50, 80, 50 };
		MouseInText = false;
	}

	void Process()
	{
		int MouseClick_x = 0, MouseClick_y = 0;
		POINT Mouse;
		SDL_RenderCopyEx(renderer, Image_Texture, NULL, NULL, 0, NULL, SDL_FLIP_NONE);
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
					if ((MouseClick_x >= 0) && (MouseClick_x <= 80) && (MouseClick_y >= screen_height - 50) && (MouseClick_y <= screen_height))
					{
						Mix_PlayChannel(-1, Enter_Wav, 0);
						Screen_State = 1;
					}
				}
			}
		}

		GetCursorPos(&Mouse);
		ScreenToClient(hwnd, &Mouse);
		// Back
		if ((Mouse.x >= 0) && (Mouse.x <= 80) && (Mouse.y >= (screen_height - 50)) && (Mouse.y <= screen_height))
		{
			SDL_Rect tmp = { 0,screen_height - 50,80, 50 };
			SDL_SetRenderDrawColor(renderer, 0, 150, 0, 255);
			SDL_RenderFillRect(renderer, &tmp);
			if (MouseInText == false) Mix_PlayChannel(-1, Mousein_Wav, 0);
			MouseInText = true;
		}
		else MouseInText = false;
		SDL_RenderCopyEx(renderer, Back, NULL, &Back_position, 0, NULL, SDL_FLIP_NONE);
	}
}