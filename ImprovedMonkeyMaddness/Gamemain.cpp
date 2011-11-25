//******************************************************************************
// monkeytx Madness proto-game
// DirectX Programming Gamemain File
// Liam Boyle
// CIS 255-57Z1
// 20 Nov 2011
// lboyle0004@kctcs.edu liampboyle@gmail.com
//******************************************************************************

#include "GameHeader.h"
using namespace std;

const string APPTITLE = "monkeytxMadness"; //title of application
const int SCREENW = 1024; //wanted screen width
const int SCREENH = 728; //wanted screen height
	/* note: LaFrustraction res = 1280x800, Prissy res = 1024x768*/

LPDIRECT3DSURFACE9 backgroundSurface = NULL;

SPRITE monkey, banana;

LPDIRECT3DTEXTURE9 monkeytx = NULL;
LPDIRECT3DTEXTURE9 bananatx = NULL;

long mx;
long my;
POINT curPos;

int frame = 0, columns, width, height;
int startframe, endframe, starttime = 0, delay;

//Game initialization function
bool Game_Init(HWND window)
{
	//initialize Direct3D
	if(!Direct3D_Init(window, SCREENW, SCREENH, false))
	{
		MessageBox(0, "Error initializing Direct3D", "error", 0);
		return false;
	}

	if(!DirectInput_Init(window))
	{
		MessageBox(0, "Error initializing DirectInput", "error", 0);
		return false;
	}

	backgroundSurface = LoadSurface("monkeyBackground.png");
	if (!backgroundSurface)
	{
		MessageBox(window, "Error loading file", "Error", 0);
		return false;
	}

	monkeytx = LoadTexture("monkeySpriteSheet.png");
	if (!monkeytx)
	{
		MessageBox(window, "Error loading file", "Error", 0);
		return false;
	}

	bananatx = LoadTexture("FlyingBanana.png");
	if (!bananatx)
	{
		MessageBox(window, "Error loading file", "Error", 0);
		return false;
	}

	//set properties for sprites
	monkey.x=0;
	monkey.y=SCREENH-64;
	monkey.width=monkey.height=64;
	monkey.columns=10;
	monkey.startframe=0;
	monkey.endframe=4;
	monkey.direction=RIGHT;
	monkey.velx=2.0f;
	monkey.delay=45;

	banana.x=0;
	banana.y=0;
	banana.width=banana.height=32;
	banana.columns=4;
	banana.startframe=0;
	banana.endframe=3;
	banana.delay=60;

	return true;
}

//Game update function
void Game_Run(HWND window)
{
    //make sure the Direct3D device is valid
	if (!d3ddev) return;

	static float scale=1.0f;
	static float r=0;
	static float s=1.0f;

	//clear the scene
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0),
		1.0f, 0);

	//move and animate the monkey
	if(monkey.direction==RIGHT && (monkey.x>SCREENW-monkey.width))
	{
		monkey.direction=LEFT;
		monkey.startframe=5;
		monkey.endframe=9;
	}
	else if(monkey.direction==LEFT && (monkey.x<0.0f))
	{
		monkey.direction=RIGHT;
		monkey.startframe=0;
		monkey.endframe=4;
	}

	if (monkey.direction==RIGHT)
		monkey.x+=monkey.velx;
	else
		monkey.x-=monkey.velx;

	Sprite_Animate(monkey.frame, monkey.startframe, monkey.endframe, monkey.direction,
			monkey.starttime, monkey.delay);

	Sprite_Animate(banana.frame, banana.startframe, banana.endframe, banana.direction,
		banana.starttime, banana.delay);

	if(Collision(monkey, banana))
		gameover=true;

	//start rendering
	if (d3ddev->BeginScene())
	{
		//draw background
		RECT rect;
		rect.left = 0;
		rect.right = SCREENW;
		rect.top = 0;
		rect.bottom = SCREENH;
		d3ddev->StretchRect(backgroundSurface, NULL, backbuffer, &rect, D3DTEXF_NONE);

		//start drawing
		spriteObj->Begin(D3DXSPRITE_ALPHABLEND);

		//draw sprites
		D3DCOLOR color = D3DCOLOR_XRGB(255,255,255);
		Sprite_Transform_Draw(monkeytx, monkey.x, monkey.y, monkey.width, monkey.height, monkey.frame,
			monkey.columns);

		//update input devices
		DirectInput_Update();
		
		//move protagonist with the mouse
		//int mx = Mouse_X();
		//int my = Mouse_Y();

		//attempt to use absolute mouse coordinates
		dimouse->GetDeviceState(sizeof(mouse_state), (LPVOID) &mouse_state);
		GetCursorPos(&curPos);
		ScreenToClient(window, &curPos);
		mx = curPos.x;
		my = curPos.y;

		banana.x=mx;
		banana.y=my;
		
		//move protagonist with the keyboard
			//pass
	
		Sprite_Transform_Draw(bananatx, banana.x, banana.y, banana.width, banana.height, banana.frame,
			banana.columns);

		//stop drawing
		spriteObj->End();

		//stop rendering
		d3ddev->EndScene();
		d3ddev->Present(NULL, NULL, NULL, NULL);
	}

	//check for escape key (to exit program)
	if(Key_Down(DIK_ESCAPE))
		gameover=true;

}

//Game shutdown function
void Game_End()
{
    if (backgroundSurface) backgroundSurface->Release();
	if (monkeytx) monkeytx->Release();
	if (bananatx) bananatx->Release();
	DirectInput_Shutdown();
	Direct3D_Shutdown();
}