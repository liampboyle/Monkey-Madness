//******************************************************************************
// Monkey Madness proto-game
// DirectX Programming DirectX main File
// Liam Boyle
// CIS 255-57Z1
// 20 Nov 2011
// lboyle0004@kctcs.edu liampboyle@gmail.com
//******************************************************************************

#include "GameHeader.h"
#include <iostream>
using namespace std;

//named constants for functions
const double PI=3.1415926535;
const double PI_under_180=180.0f/PI;
const double PI_over_180=PI/180.0f;

//Direct3D variables
LPDIRECT3D9 d3d = NULL; 
LPDIRECT3DDEVICE9 d3ddev = NULL; 
LPDIRECT3DSURFACE9 backbuffer = NULL;
LPD3DXSPRITE spriteObj = NULL;

//DirectInput variables
LPDIRECTINPUT8 dinput = NULL;
LPDIRECTINPUTDEVICE8 dimouse = NULL;
LPDIRECTINPUTDEVICE8 dikeyboard = NULL;
DIMOUSESTATE mouse_state;
char keys[256];
//XINPUT_GAMEPAD controllers[4];

//Direct3D initialization
bool Direct3D_Init(HWND window, int width, int height, bool fullscreen)
{
    //initialize Direct3D
    d3d = Direct3DCreate9(D3D_SDK_VERSION);
    if (!d3d) return false;

    //set Direct3D presentation parameters
    D3DPRESENT_PARAMETERS d3dpp; 
    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.Windowed = (!fullscreen);
    d3dpp.SwapEffect = D3DSWAPEFFECT_COPY;
    d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferCount = 1;
    d3dpp.BackBufferWidth = width;
    d3dpp.BackBufferHeight = height;
    d3dpp.hDeviceWindow = window;

    //create Direct3D device
    d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3ddev);

    if (!d3ddev) return false;

    //get a pointer to the back buffer surface
    d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);

	//create sprite object
	D3DXCreateSprite(d3ddev, &spriteObj);

    return 1;
}

//Direct3D shutdown
void Direct3D_Shutdown()
{
	if (spriteObj) spriteObj->Release();
    if (d3ddev) d3ddev->Release();
    if (d3d) d3d->Release();
}

//Function for degree to radian conversion
double toRadians(double degrees)
{
	return degrees*PI_over_180;
}

//Function for radian to degree conversion
double toDegrees(double radians)
{
	return radians*PI_under_180;
}

//Fuction to load texture
LPDIRECT3DTEXTURE9 LoadTexture(string filename, D3DCOLOR transcolor)
{
	LPDIRECT3DTEXTURE9 texture = NULL;

	//get width and height from bitmap file
	D3DXIMAGE_INFO info;
	HRESULT result = D3DXGetImageInfoFromFile(filename.c_str(), &info);
	if(result != D3D_OK) return NULL;

	//create the new texture by loading a bitmap image file
	D3DXCreateTextureFromFileEx(
		d3ddev,
		filename.c_str(),
		info.Width,
		info.Height,		
		1,					//mip-map levels (1 for no chain)
		D3DPOOL_DEFAULT,	//the type of surface
		D3DFMT_UNKNOWN,		//surface format (default)
		D3DPOOL_DEFAULT,	//memory class for texture
		D3DX_DEFAULT,		//image filter
		D3DX_DEFAULT,		//mip filter
		transcolor,			//color key for transparency
		&info,				//image file info (from loaded file)
		NULL,				//color palette
		&texture);			//destination texture

	//make sure image was loaded correctly
	if(result != D3D_OK) return NULL;

	return texture;
}

D3DXVECTOR2 GetBitmapSize(string filename)
{
	D3DXIMAGE_INFO info;
	D3DXVECTOR2 size = D3DXVECTOR2(0.0f, 0.0f);
	HRESULT result = D3DXGetImageInfoFromFile(filename.c_str(), &info);
	if(result == D3D_OK)
		size = D3DXVECTOR2((float)info.Width, (float)info.Height);
	else
		size = D3DXVECTOR2((float)info.Width, (float)info.Height);

	return size;
}

//Draws a surface to the screen using StretchRect
void DrawSurface(LPDIRECT3DSURFACE9 dest, float x, float y, LPDIRECT3DSURFACE9 source)
{
    //get width/height from source surface
    D3DSURFACE_DESC desc;
    source->GetDesc(&desc);

    //create rects for drawing
    RECT source_rect = {0, 0, (long)desc.Width, (long)desc.Height };
    RECT dest_rect = { (long)x, (long)y, (long)x+desc.Width, (long)y+desc.Height};
    
    //draw the source surface onto the dest
    d3ddev->StretchRect(source, &source_rect, dest, &dest_rect, D3DTEXF_NONE);

}

//Loads a bitmap file into a surface
LPDIRECT3DSURFACE9 LoadSurface(string filename)
{
    LPDIRECT3DSURFACE9 image = NULL;
    
    //get width and height from bitmap file
    D3DXIMAGE_INFO info;
    HRESULT result = D3DXGetImageInfoFromFile(filename.c_str(), &info);
    if (result != D3D_OK)
        return NULL;

    //create surface
    result = d3ddev->CreateOffscreenPlainSurface(
        info.Width,         //width of the surface
        info.Height,        //height of the surface
        D3DFMT_X8R8G8B8,    //surface format
        D3DPOOL_DEFAULT,    //memory pool to use
        &image,             //pointer to the surface
        NULL);              //reserved (always NULL)

    if (result != D3D_OK) return NULL;

    //load surface from file into newly created surface
    result = D3DXLoadSurfaceFromFile(
        image,                  //destination surface
        NULL,                   //destination palette
        NULL,                   //destination rectangle
        filename.c_str(),       //source filename
        NULL,                   //source rectangle
        D3DX_DEFAULT,           //controls how image is filtered
        D3DCOLOR_XRGB(0,0,0),   //for transparency (0 for none)
        NULL);                  //source image info (usually NULL)

    //make sure file was loaded okay
    if (result != D3D_OK) return NULL;

    return image;
}

void Sprite_Draw_Frame(LPDIRECT3DTEXTURE9 texture, int destx, int desty, int framew, int frameh,
					   int framenum, int columns)
{
	//perform check for NULL
	if(!texture) return;

	D3DXVECTOR3 position ((float)destx, (float)desty, 0);
	D3DCOLOR white = D3DCOLOR_XRGB(255,255,255);

	RECT rect;
	rect.left = (framenum % columns) * framew;
	rect.top = (framenum/columns)*frameh;
	rect.right = rect.left+framew;
	rect.bottom = rect.top+frameh;

	spriteObj->Draw(texture, &rect, NULL, &position, white);
}

void Sprite_Animate(int &frame, int startframe, int endframe,
					int direction, int &starttime, int delay)
{
	if((int)GetTickCount()>starttime+delay)
	{
		starttime = GetTickCount();
		if (direction==RIGHT) frame+=direction;
		else frame += 1;

		if (frame>endframe) frame=startframe;
		if (frame<startframe) frame=endframe;
	}
}

void Sprite_Transform_Draw(LPDIRECT3DTEXTURE9 image, int x, int y, int width, int height,
						   int frame, int columns, float rotation, float scaling,
						   D3DCOLOR color)
{
	//create a scale vector
	D3DXVECTOR2 scale(scaling, scaling);

	//create a translate vector
	D3DXVECTOR2 trans(x, y);

	//set center by dividing width and height by two
	D3DXVECTOR2 center((float)(width*scaling)/2, (float)(height*scaling)/2);

	//create 2D transformation matrix
	D3DXMATRIX mat;
	D3DXMatrixTransformation2D(&mat, NULL, 0, &scale, &center, rotation, &trans);

	//tell sprite object to use transformation
	spriteObj->SetTransform(&mat);

	//calculate frame location in source image
	int fx = (frame%columns)*width;
	int fy = (frame/columns)*height;
	RECT srcRect={fx,fy,fx+width,fy+height};

	//draw the sprite frame
	spriteObj->Draw(image, &srcRect, NULL, NULL, color);
}

//Collision Detection Function
int Collision(SPRITE sprite1, SPRITE sprite2)
{
	RECT rect1;
	rect1.left = (long)sprite1.x;
	rect1.top = (long)sprite1.y;
	rect1.right = (long)sprite1.x+sprite1.width*sprite1.scaling;
	rect1.bottom = (long)sprite1.y+sprite1.height*sprite1.scaling;

	RECT rect2;
	rect2.left = (long)sprite2.x;
	rect2.top = (long)sprite2.y;
	rect2.right = (long)sprite2.x+sprite2.width*sprite2.scaling;
	rect2.bottom = (long)sprite2.y+sprite2.height*sprite2.scaling;

	RECT dest; //ignored
	return IntersectRect(&dest, &rect1, &rect2);
}

//Distance Based Collision Detection
bool CollisionD(SPRITE sprite1, SPRITE sprite2)
{
	double radius1, radius2;

	//calculate radiius 1
	if(sprite1.width > sprite1.height)
		radius1 = (sprite1.width*sprite1.scaling)/2.0;
	else 
		radius1 = (sprite1.height*sprite1.scaling)/2.0;

	//center point 1
	double x1=sprite1.x+radius1;
	double y1=sprite1.y+radius1;
	D3DXVECTOR2 vector1(x1, y1);

	//calculate radius 2
	if(sprite2.width > sprite2.height)
		radius2 = (sprite2.width*sprite2.scaling)/2.0;
	else 
		radius2 = (sprite2.height*sprite2.scaling)/2.0;

	//center point 2
	double x2=sprite2.x+radius2;
	double y2=sprite2.y+radius2;
	D3DXVECTOR2 vector2(x2, y2);

	//calculate distance
	double deltax = vector1.x - vector2.x;
	double deltay = vector2.y - vector1.y;
	double dist = sqrt((deltax*deltax)+(deltay+deltay));

	//return distance comparison
	return (dist<radius1+radius2);
}

// DirectInput initialization
bool DirectInput_Init(HWND hwnd)
{
    //initialize DirectInput object
    HRESULT result = DirectInput8Create(
        GetModuleHandle(NULL), 
        DIRECTINPUT_VERSION, 
        IID_IDirectInput8,
        (void**)&dinput,
        NULL);

    //initialize the keyboard
    dinput->CreateDevice(GUID_SysKeyboard, &dikeyboard, NULL);
    dikeyboard->SetDataFormat(&c_dfDIKeyboard);
    dikeyboard->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
    dikeyboard->Acquire();

    //initialize the mouse
    dinput->CreateDevice(GUID_SysMouse, &dimouse, NULL);
    dimouse->SetDataFormat(&c_dfDIMouse);
    dimouse->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
    dimouse->Acquire();
    d3ddev->ShowCursor(false);

    return true;
}

//DirectInput update
void DirectInput_Update()
{
    //update mouse
    dimouse->GetDeviceState(sizeof(mouse_state), (LPVOID)&mouse_state);

    //update keyboard
    dikeyboard->GetDeviceState(sizeof(keys), (LPVOID)&keys);
}

//Return mouse x movement
int Mouse_X()
{
    return mouse_state.lX;
}

//Return mouse y movement
int Mouse_Y()
{
    return mouse_state.lY;
}

//Return mouse button state
int Mouse_Button(int button)
{
    return mouse_state.rgbButtons[button] & 0x80;
}

//Return key press state
int Key_Down(int key)
{
    return (keys[key] & 0x80);
}

//Font functions
LPD3DXFONT MakeFont(string name, int size)
{
	LPD3DXFONT font = NULL;
	D3DXFONT_DESC desc = 
	{
		size, 0, 0, 0, false, DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_PITCH, ""
	};
	strcpy(desc.FaceName, name.c_str());
	D3DXCreateFontIndirect(d3ddev, &desc, &font);
	return font;
}

void FontPrint(LPD3DXFONT font, int x, int y, string text, D3DCOLOR color)
{
	//figure out the text boundary
	RECT rect={x, y, 0, 0};
	font->DrawText(NULL, text.c_str(), text.length(), &rect, DT_CALCRECT, color);

	//print the text
	font->DrawText(NULL, text.c_str(), text.length(), &rect, DT_LEFT, color);
}

//DirectInput shutdown
void DirectInput_Shutdown()
{
    if (dikeyboard) 
    {
        dikeyboard->Unacquire();
        dikeyboard->Release();
        dikeyboard = NULL;
    }
    if (dimouse) 
    {
        dimouse->Unacquire();
        dimouse->Release();
        dimouse = NULL;
    }
}
