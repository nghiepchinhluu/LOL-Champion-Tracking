#pragma once

#define WINVER 0x0500
#include <Windows.h>

void ping(int dx, int dy)
{
	INPUT key;

	POINT oldpos;
	GetCursorPos(&oldpos);
	//ScreenToClient(hwnd, &oldpos);

	double fScreenWidth = ::GetSystemMetrics(SM_CXSCREEN) - 1;
	double fScreenHeight = ::GetSystemMetrics(SM_CYSCREEN) - 1;

	float x = (float)dx * 65536 / 1920;
	float y = (float)dy * 65536 / 1080;


	INPUT click[4] = { 0 };
	click[0].type = INPUT_MOUSE;

	click[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
	click[0].mi.dx = x;
	click[0].mi.dy = y;

	click[1] = click[0];
	click[1].mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
	SendInput(2, click, sizeof(INPUT));

	Sleep(20);

	click[2] = click[0];
	click[2].mi.dwFlags = MOUSEEVENTF_MIDDLEUP;

	click[3] = click[0];
	click[3].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
	click[3].mi.dx = (float)oldpos.x * 65536 / fScreenWidth;
	click[3].mi.dy = (float)oldpos.y * 65536 / fScreenHeight;
	SendInput(2, &click[2], sizeof(INPUT));
}
