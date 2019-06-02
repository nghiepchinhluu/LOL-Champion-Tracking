#pragma once

namespace Background
{
	struct Background
	{
		//YOPU CANNO)T UPT FU NTION INSIDE STRUCTS
		//PUT DATA
		SDL_Texture* Texture;
		int Width;
		int Height;
		int WidthperFrame;
		int MsperFrame;
		SDL_Rect FrameRect;
	};

	Background *BackgroundList;
	int BackgroundList_Size;
}