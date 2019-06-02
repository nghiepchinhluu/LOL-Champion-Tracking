#pragma once


namespace ChampSelectScreen
{
	SDL_Surface* data_champion_image_Surface;
	SDL_Texture* data_champion_image_Texture;

	void Init()
	{
		data_champion_image_Surface = IMG_Load("lolchamps.jpg");
		data_champion_image_Texture = SDL_CreateTextureFromSurface(renderer, data_champion_image_Surface);
	}

	void Process()
	{
		int MouseClick_y = 0;
		int MouseClick_x = 0;
		SDL_RenderCopyEx(renderer, data_champion_image_Texture, NULL, NULL, 0, NULL, SDL_FLIP_NONE);
		bool Selected = false;
		int index = -1;
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
					Selected = true;
				}
			}
		}
		if (Selected == false) return;
		MouseClick_x -= 3;
		MouseClick_y -= 3;
		index = (MouseClick_y / 48) * 14 + (MouseClick_x / 48);
		int Champion_x = 0; int Champion_y = 0;
		champion::name = champion::Get_Champion_Name(index);
		champion::pixel_threshold = champion::Get_Pixel_Threshold(index);
		champion::Find_Champion_Position(index, &Champion_x, &Champion_y);

		SDL_Rect champ_capture = { Champion_x,Champion_y,champion::portrait_size.width,champion::portrait_size.height };
		SDL_Rect tmp_champ_capture = { 0,0,champion::portrait_size.width,champion::portrait_size.height };
		SDL_SetSurfaceBlendMode(champion::portrait, SDL_BLENDMODE_NONE);
		SDL_BlitScaled(data_champion_image_Surface, &champ_capture, champion::portrait, &tmp_champ_capture);


		Write(&SetupScreen::ChampName, champion::name, font, SetupScreen::color, renderer);

		Screen_State = 1;
	}
}

