#pragma once

namespace TrackingScreen
{
	int previous_change_time;
	SDL_Texture* TrackingInProgress;
	SDL_Texture* Back;
	SDL_Texture* Dot;
	SDL_Texture* HitShift;

	SDL_Rect TrackingInProgress_position;
	SDL_Rect Back_position;
	SDL_Rect Dot_position;
	SDL_Rect HitShift_position;

	int Dot_Current;
	int Dot_Last;

	SDL_Color color;
	char Text[] = "Tracking in progress";
	char Text1[] = ".";
	char Text2[] = "Back";
	char Text3[] = "When champion is detected, hit shift to autoping";
	int DotNum;
	bool MouseInText;

	struct Rectt
	{
		int xo;
		int yo;
		int width;
		int height;
	};

	Rectt minimap;

	SDL_Surface *clip_buffer;
	unsigned char *tmp_clip_buffer;

	bool acknowledge, beeping;

	int current_ping, last_ping;


	void Init()
	{
		color = { 255,255,255,255 };

		Write(&TrackingInProgress, Text, font, color, renderer);
		TrackingInProgress_position = { 20, 20, 300, 50 };

		Write(&Back, Text2, font, color, renderer);
		Back_position = { 0, screen_height - 50, 80, 50 };

		Write(&Dot, Text1, font, color, renderer);

		Write(&HitShift, Text3, font, color, renderer);
		HitShift_position = { 20, 100 , 600 ,50 };
		


		MouseInText = false;

		Dot_Current = clock();
		Dot_Last = Dot_Current;
		DotNum = 0;

		minimap.width = 232;
		minimap.height = 232;
		Screen_Grab::init(minimap.width, minimap.height);
		minimap.xo = 1665;
		minimap.yo = 825;


		clip_buffer = SDL_CreateRGBSurfaceWithFormat(0, minimap.width, minimap.height, 24, SDL_PIXELFORMAT_BGR24);
		tmp_clip_buffer = (unsigned char*)malloc(sizeof(unsigned char) * minimap.width * minimap.height * 3);

		current_ping = clock();
		last_ping = current_ping;
	}

	void ScreenShow()
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
					if ((MouseClick_x >= 0) && (MouseClick_x <= 80) && (MouseClick_y >= screen_height - 50) && (MouseClick_y <= screen_height))
					{
						Mix_PlayChannel(-1, Enter_Wav, 0);
						Screen_State = 0;
					}
				}
			}
		}

		if (Screen_State != 5) return;

		GetCursorPos(&Mouse);
		ScreenToClient(hwnd, &Mouse);

		// Back
		if ((Mouse.x >= 0) && (Mouse.x <= 80) && (Mouse.y >= (screen_height - 50)) && (Mouse.y <= screen_height))
		{
			SDL_Rect tmp = { 0,screen_height - 50,80, 50 };
			SDL_RenderFillRect(renderer, &tmp);
			if (MouseInText == false) Mix_PlayChannel(-1, Mousein_Wav, 0);
			MouseInText = true;
		}
		else MouseInText = false;

		Background::Show(&Background::BackgroundList[Current_Background], renderer);

		// Render Texts
		SDL_RenderCopyEx(renderer, Back, NULL, &Back_position, 0, NULL, SDL_FLIP_NONE);
		SDL_RenderCopyEx(renderer, TrackingInProgress, NULL, &TrackingInProgress_position, 0, NULL, SDL_FLIP_NONE);
		SDL_RenderCopyEx(renderer, HitShift, NULL, &HitShift_position, 0, NULL, SDL_FLIP_NONE);

		Dot_Current = clock();
		if (Dot_Current - Dot_Last > 1000)
		{
			DotNum++;
			DotNum %= 3;
			Dot_Last = Dot_Current;
		}
		for (int i = 0; i <= DotNum; i++)
		{
			Dot_position = { 320 + (i * 10),20,10,50 };
			SDL_RenderCopyEx(renderer, Dot, NULL, &Dot_position, 0, NULL, SDL_FLIP_NONE);
		}
	}

	void HandleData()
	{
		// Grab pixel buffer for minimap
		Screen_Grab::grab(tmp_clip_buffer, minimap.width, minimap.height, minimap.xo, minimap.yo);
		Pixels::copy((unsigned char*)clip_buffer->pixels, 0, 0, minimap.width, minimap.height, tmp_clip_buffer, minimap.width, minimap.height);

		COLORREF color = RGB(255, 0, 0);
		HDC screen = GetDC(NULL);
		// Find the pixel block which has the lowest pixel error, compared to the champion portrait
		long long error_min = MAXLONGLONG;
		int x_min = 0;
		int y_min = 0;
		for (int y = minimap.yo; y < minimap.yo+240 ; y++)  //240
		{
			current_time = clock();
			if (current_time - previous_change_time > Background::BackgroundList[Current_Background].MsperFrame)
			{
				Background::Update(&Background::BackgroundList[Current_Background]);
				if (Background::BackgroundList[Current_Background].FrameRect.x == 0) Current_Background = Background::ShuffleBackground(Current_Background);
				previous_change_time = current_time;
			}
			ScreenShow();
			SDL_RenderPresent(renderer);
			for (int x = minimap.xo; x < minimap.xo+240; x++)  //240
			{
				long long diff = Pixels::compare((unsigned char*)champion::portrait->pixels, (unsigned char*)clip_buffer->pixels, x, y, champion::portrait_size.width, champion::portrait_size.height, minimap.xo, minimap.yo, minimap.width);
				if (diff < error_min)
				{
					x_min = x;
					y_min = y;
					error_min = diff;
				}
			}
		}

		//printf("%llu\t%d\t%d\n", error_min, x_min, y_min);

		current_ping = clock();
		if ((error_min < champion::pixel_threshold) && (acknowledge == false) && (beeping == true) && (GetAsyncKeyState(VK_LSHIFT)))
		{
			ping(x_min + 8, y_min + 8); //+8 to make it at the center of the portrait
			acknowledge = true;
			last_ping = current_ping;
			beeping = false;
		}
		else if ((error_min < champion::pixel_threshold) && (acknowledge == false) && (current_ping - last_ping > 7000))
		{
			Mix_PlayChannel(-1, Warning_Wav, 0); // Change this to the beep sound
			beeping = true;
			//printf("BEEP!!!\n");
			for (int j = 0; j < 120; j++)
			for (int i = 0; i < 70; i++)
			SetPixel(screen, 900 + j, 100 + i, color);
			ReleaseDC(NULL, screen);
		}
		else if (error_min >= champion::pixel_threshold) acknowledge = false;
	}


	void Process()
	{

		HandleData();
	}

}