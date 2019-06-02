#pragma once
#include "Champion_Data.h"

#include <iostream>

namespace champion
{
	void Find_Champion_Position(int index, int* position_x, int* position_y);
	char* Get_Champion_Name(int index);
	int Get_Pixel_Threshold(int index);
}

namespace champion
{
	void Find_Champion_Position(int index, int* position_x, int* position_y)   // Find champion's location in the image
	{
		int left_border = 7;
		int top_border = 7;
		float t1 = (index % 14) * 24;
		float t2 = (index / 14) * 24;
		*position_x = (int)t1 + left_border;
		*position_y = (int)t2 + top_border;
	}

	char* Get_Champion_Name(int index)
	{
		FILE *file;
		file = fopen("championlist.txt", "r");
		char* result = (char*)malloc(sizeof(char) * 20);
		for (int i = 0; i < index; i++)
		{
			fgets(result, 20, file);
		}
		fgets(result, 20, file);
		if (result[strlen(result) - 1] == '\n') result[strlen(result) - 1] = '\0';
		fclose(file);
		return result;
	}

	int Get_Pixel_Threshold(int index)
	{
		FILE *file;
		file = fopen("limit.txt", "r");
		int result = 0;
		for (int i = 0; i < index; i++)
		{
			fscanf(file, "%d", &result);
		}
		fscanf(file, "%d", &result);
		fclose(file);
		return result;
	}
}
