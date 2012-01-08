//******************************************************************************
// Monkey Madness proto-game
// DirectX Programming Gamemain File
// Liam Boyle  Background and Logo by Lou Ann Boyle
// CIS 255-57Z1
// 20 Nov 2011
// lboyle0004@kctcs.edu liampboyle@gmail.com
//******************************************************************************

#include "GameHeader.h"
using namespace std;

const string APPTITLE = "Monkey Madness"; //title of application
const int SCREENW = 1024; //wanted screen width
const int SCREENH = 728; //wanted screen height
	/* note: LaFrustraction res = 1280x800, Prissy res = 1024x768*/

LPDIRECT3DSURFACE9 backgroundSurface = NULL;

SPRITE monkey, banana, tiger;

LPDIRECT3DTEXTURE9 monkeytx = NULL;
LPDIRECT3DTEXTURE9 bananatx = NULL;
LPDIRECT3DTEXTURE9 tigertx = NULL;

LPD3DXFONT fontArial24 = NULL;

long mx;
long my;
POINT curPos;

int frame = 0, columns, width, height;
int startframe, endframe, starttime = 0, delay;
int score = 0;
int monkeyLives = 3;
int level = 0;
bool showBanana = false;

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

	backgroundSurface = LoadSurface("MonkeyBackground1.png");
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

	tigertx = LoadTexture("RandomTiger.png");
	if (!tigertx)
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
	monkey.delay=45;

	banana.x=0;
	banana.y=0;
	banana.width=banana.height=32;
	banana.columns=4;
	banana.startframe=0;
	banana.endframe=3;
	banana.delay=60;

	tiger.randReset();
	tiger.width=tiger.height=128;
	tiger.columns=1;
	tiger.startframe=0;
	tiger.endframe=0;
	tiger.delay=45;

	//font creation
	fontArial24 = MakeFont("Arial", 24);

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

	//set sprite speeds
	tiger.vely = 2.0f + (0.05f*level);
	monkey.velx = 4.0f + (0.05f*level);
	banana.vely = 6.0f + (0.05f*level);

	Sprite_Animate(tiger.frame, tiger.startframe, tiger.endframe, tiger.direction,
		tiger.starttime, tiger.delay);

	//detect game conditions
	if(Collision(monkey, tiger))
	{
		monkeyLives--;
		tiger.randReset();
	}

	if(Collision(tiger, banana))
	{
		score += 100;
		level++;
		showBanana = false;
		tiger.randReset();
	}

	if(monkeyLives <= 0)
		gameover = true;

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
		Sprite_Transform_Draw(tigertx, tiger.x, tiger.y, tiger.width, tiger.height, tiger.frame,
			tiger.columns);

		//move tiger downscreen
		tiger.y += tiger.vely;

		//see if tiger hit bottom
		if (tiger.y > SCREENH-tiger.height)
		{
			tiger.randReset();
		}

		//update input devices
		DirectInput_Update();

		//move the monkey with the keyboard fire banana
		if(Key_Down(DIK_LEFT)) 
		{
			monkey.direction = LEFT;
			monkey.startframe = 5;
			monkey.endframe = 10;
			monkey.x -= monkey.velx;
			if (monkey.x<=0) monkey.x=0;

			Sprite_Animate(monkey.frame, monkey.startframe, monkey.endframe, monkey.direction,
				monkey.starttime, monkey.delay);
		}

		if(Key_Down(DIK_RIGHT)) 
		{
			monkey.direction = RIGHT;
			monkey.startframe = 0;
			monkey.endframe = 4;
			monkey.x += monkey.velx;
			if (monkey.x >= SCREENW-64) monkey.x=SCREENW-64;

			Sprite_Animate(monkey.frame, monkey.startframe, monkey.endframe, monkey.direction,
				monkey.starttime, monkey.delay);
		}

		if(Key_Down(DIK_SPACE))
		{
			showBanana = true;
			banana.x=monkey.x;
			banana.y=monkey.y;
		}

		// move the banana up screen
		if (showBanana)
		{
			Sprite_Animate(banana.frame, banana.startframe, banana.endframe, banana.direction,
				banana.starttime, banana.delay);
			banana.y -= banana.vely;
			Sprite_Transform_Draw(bananatx, banana.x, banana.y, banana.width, banana.height,
				banana.frame, banana.columns);

			//check if banana hit top of screen
			if(banana.y<=0) showBanana=false;
		}

		//display score and lives
		D3DCOLOR white = D3DCOLOR_XRGB(255,255,255);
		char scoreBuffer[33];
		_itoa_s (score, scoreBuffer, 10);
		char livesBuffer[33];
		_itoa_s (monkeyLives, livesBuffer, 10);
		//static_cast<string>(scoreBuffer);
		char scoreDisplay[9] = "Score:  "; //need to concatenate score long value to string
		FontPrint(fontArial24, 800, 10, scoreDisplay, white);
		FontPrint(fontArial24, 875, 10, scoreBuffer, white);
		char livesDisplay[9] = "Lives:  "; //need to concatenate lives int value to string
		FontPrint(fontArial24, 800, 50, livesDisplay, white);
		FontPrint(fontArial24, 875, 50, livesBuffer, white);

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
	if (fontArial24) fontArial24->Release();
	if (tigertx) tigertx->Release();
	if (monkeytx) monkeytx->Release();
	if (bananatx) bananatx->Release();
	DirectInput_Shutdown();
	Direct3D_Shutdown();
}