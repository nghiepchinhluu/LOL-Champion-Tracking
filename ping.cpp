#pragma warning (disable:4996)
#define WINVER 0x0500
#include <windows.h>
#include <WinUser.h>
#include <time.h>
#include <stdio.h>

int main()
{
	// This structure will be used to create the keyboard
	// input event.


	POINT oldpos;

	RECT clip = { 0,0,1920,1080 };
	ClipCursor(&clip);
	HWND hwnd = FindWindow(NULL, "League of Legends (TM) Client");

	FILE *f = fopen("test.txt", "w");

	int previous_time = clock();
	for (;;)
	{
		GetCursorPos(&oldpos);
		//ScreenToClient(hwnd, &oldpos);

		if (clock() - previous_time > 3000)
		{
			fprintf(f, "%d %d\n", oldpos.x, oldpos.y);

			double fScreenWidth = ::GetSystemMetrics(SM_CXSCREEN) - 1;
			double fScreenHeight = ::GetSystemMetrics(SM_CYSCREEN) - 1;

			float x = 1700.0 * 65536 / 1920;
			float y = 850.0 * 65536 / 1080;
		
			INPUT click[4] = { 0 };
			click[0].type = INPUT_MOUSE;

			click[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
			click[0].mi.dx = x;
			click[0].mi.dy = y;
			//SendInput(1, &click, sizeof(INPUT));

			click[1] = click[0];
			click[1].mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
			SendInput(2, click, sizeof(INPUT));

			Sleep(20);

			click[2] = click[0];
			click[2].mi.dwFlags = MOUSEEVENTF_MIDDLEUP;

			click[3] = click[0];
			click[3].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
			click[3].mi.dx = (float)oldpos.x * 65536 / 1920;
			click[3].mi.dy = (float)oldpos.y * 65536 / 1080;
			SendInput(2, &click[2], sizeof(INPUT));
	
			previous_time = clock();
		}
	}

	fclose(f);
	// Exit normally
	return 0;
}