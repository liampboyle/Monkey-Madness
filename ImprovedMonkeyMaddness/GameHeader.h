//******************************************************************************
// Monkey Madness proto-game
// DirectX Programming Header File
// Liam Boyle
// CIS 255-57Z1
// 20 Nov 2011
// lboyle0004@kctcs.edu liampboyle@gmail.com
//******************************************************************************

#pragma once

#ifndef GAME_HEADER_H
#define GAME_HEADER_H

// include, using statements, and pre-processor directives
#define WIN32_EXTRA_LEAN
#define DIRECTINPUT_VERSION 0x0800

// #include <cstdlib>		// standard lib just for general principle
#include <iostream>		// access cin, cout
#include <iomanip>		// access manipulators for data formatting
#include <windows.h>	// access components for windows programming
//#include <time.h>		// access time and clock functions
#include <d3d9.h>		// directX
#include <d3dx9.h>		// directX extensions
#include <dinput.h>		// access direct input
//#include <xinput.h>		// xbox controller support
#include <ctime>		// time functions for random number seeds

using namespace std;

// Libraries
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dinput8.lib")

//Named constant declarations
extern const string APPTITLE;
extern const int SCREENW;
extern const int SCREENH;
//extern HWND window;
//extern HDC device;
extern bool gameover;
extern const double PI;
extern const double PI_under_180;
extern const double PI_over_180;

//macro to detect key presses
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

enum direction {LEFT, RIGHT};

//Direct3D objects
extern LPDIRECT3D9 d3d;
extern LPDIRECT3DDEVICE9 d3ddev;
extern LPDIRECT3DSURFACE9 backbuffer;
extern LPD3DXSPRITE spriteObj;

//Direct3D functions
bool Direct3D_Init(HWND hwnd, int width, int height, bool fullscreen);
void Direct3D_Shutdown();
LPDIRECT3DSURFACE9 LoadSurface(string filename);
void DrawSurface(LPDIRECT3DSURFACE9 dest, float x, float y, LPDIRECT3DSURFACE9 source);
D3DXVECTOR2 GetBitmapSize(string filename);
LPDIRECT3DTEXTURE9 LoadTexture(string filename, D3DCOLOR transcolor=D3DCOLOR_ARGB(255,255,255,255));
void Sprite_Draw_Frame(LPDIRECT3DTEXTURE9 texture=NULL,
					   int destx=0,
					   int desty=0,
					   int framew=64,
					   int frameh=64,
					   int framenum=0,
					   int columns=1);
void Sprite_Animate(int &frame, int startframe, int endframe, int direction,
					int &starttime, int delay);
void Sprite_Transform_Draw(LPDIRECT3DTEXTURE9 image, int x, int y, int width, int height,
						   int frame=0,
						   int columns=1,
						   float rotation=0.0f,
						   float scaling=1.0f,
						   D3DCOLOR color=D3DCOLOR_XRGB(255,255,255));
double toRadians(double degrees);
double toDegrees(double radians);

//DirectInput objects, devices, and states
extern LPDIRECTINPUT8 dinput;
extern LPDIRECTINPUTDEVICE8	dimouse;
extern LPDIRECTINPUTDEVICE8 dikeyboard;
extern DIMOUSESTATE mouse_state;

//DirectInput functions
bool DirectInput_Init(HWND);
void DirectInput_Update();
void DirectInput_Shutdown();
int Key_Down(int);
int Mouse_Button(int);
int Mouse_X();
int Mouse_Y();

struct SPRITE /*game sprite structure*/
{
    float x,y;
	int frame, columns;
	int width, height; //width and height of image
	float scaling, rotation;
	int startframe, endframe;
	int starttime, delay;
	int direction; //enum values for left and right
	float velx, vely; //movement speed
	D3DCOLOR color;

    SPRITE() //constructor with default values
    {
        frame = 0;
        columns = 1;
		width = height = 0;
		scaling = 1.0f;
		rotation = 0.0f;
		startframe = endframe = 0;
		direction = RIGHT;
		starttime = delay = 0;
		velx = vely = 0.0f;
		color = D3DCOLOR_XRGB(255, 255, 255);
    }
};

int Collision(SPRITE sprite1, SPRITE sprite2);

//game functions
	//Game initialization function
		bool Game_Init(HWND window);
	//Game update function
		void Game_Run(HWND hwnd);
	//Game shutdown function
		void Game_End();

#endif