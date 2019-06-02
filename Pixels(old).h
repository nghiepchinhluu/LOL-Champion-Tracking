#pragma once

namespace Pixels
{
	void copy(unsigned char*dest, int dest_x, int dest_y, int dest_w, int dest_h, const unsigned char*src, int src_w, int src_h)
	{
		if (dest_y + src_h > dest_h) return;

		for (int y = 0; y < src_h; y++)
		{
			for (int x = 0; x < src_w; x++)
			{
				int dest_index = 3 * dest_w*(dest_y + y) + 3 * (x + dest_x);
				int src_index = 3 * src_w*(src_h - y - 1) + 3 * x;
				dest[dest_index + 0] = src[src_index + 0];
				dest[dest_index + 1] = src[src_index + 1];
				dest[dest_index + 2] = src[src_index + 2];
			}
		}
	}

	bool Is_Map(int map_index, const unsigned char* map)
	{
		if ((map[map_index + 1] == 0) && (map[map_index] == 0) && (map[map_index + 2] == 0)) return true;
		if (map[map_index + 1] == 0) return false;
		float rg = (float)map[map_index + 2] / (float)map[map_index + 1];
		float bg = (float)map[map_index] / (float)map[map_index + 1];
		if (((rg >= 0.8) && (rg <= 1)) && ((bg >= 0.6) && (bg <= 0.8))) return true;   //grass
		if (((rg >= 0.6) && (rg <= 0.9)) && ((bg >= 0.6) && (bg <= 0.9)) && (map[map_index + 1] < 20)) return true; //wall
		return false;
	}

	int compare(unsigned char* portrait, const unsigned char* map, int compare_xstart, int compare_ystart, int width, int height, int map_xstart, int map_ystart, int map_pixels_per_line) //pixel by pixel
	{
		long long diff = 0;
		int map_index = ((compare_ystart - map_ystart) * map_pixels_per_line + (compare_xstart - map_xstart)) * 3;
		if (Is_Map(map_index, map)) return 999999999;
		for (int i = compare_ystart; i < compare_ystart + height; i++)
		{
			for (int j = compare_xstart; j < compare_xstart + width; j++)
			{
				map_index = ((i - map_ystart) * map_pixels_per_line + (j - map_xstart)) * 3;
				int portrait_index = ((i - compare_ystart)* champion::portrait_size.width + (j - compare_xstart)) * 3;
				int b_diff = (map[map_index] - portrait[portrait_index])*(map[map_index] - portrait[portrait_index]);
				int g_diff = (map[map_index + 1] - portrait[portrait_index + 1])*(map[map_index + 1] - portrait[portrait_index + 1]);
				int r_diff = (map[map_index + 2] - portrait[portrait_index + 2])*(map[map_index + 2] - portrait[portrait_index + 2]);
				diff = diff + r_diff + g_diff + b_diff;
			}
		}
		diff /= (champion::portrait_size.width*champion::portrait_size.height * 3);
		return diff;
	}
}
