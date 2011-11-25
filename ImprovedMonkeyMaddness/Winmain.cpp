//******************************************************************************
// Monkey Madness proto-game
// DirectX Programming Winmain File
// Liam Boyle
// CIS 255-57Z1
// 20 Nov 2011
// lboyle0004@kctcs.edu liampboyle@gmail.com
//******************************************************************************

#include "GameHeader.h"
using namespace std;
bool gameover = false;

//Windows event handling function
LRESULT WINAPI WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) 
    {
        case WM_DESTROY:
            gameover = true;
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

//Main Windows entry function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    //initialize window properties
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = APPTITLE.c_str();
	wc.hIconSm = NULL;
	RegisterClassEx(&wc);

	//create a new window
	HWND window = CreateWindow(APPTITLE.c_str(), APPTITLE.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		SCREENW, SCREENH,
		NULL, NULL, hInstance, NULL);
	if(window == 0) return 0;

	//display the window
	ShowWindow(window, nCmdShow);
	UpdateWindow(window);

    //initialize the game
    if (!Game_Init(window)) return 0;

    // main message loop
	MSG message;
    while (!gameover)
    {
        //process Windows events
        if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) 
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
        
        //process game loop
        Game_Run(window);
    }

    //shutdown
    Game_End();
	return message.wParam;
}