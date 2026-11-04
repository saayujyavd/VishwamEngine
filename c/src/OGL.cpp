/*
* Title : Vishwam
* Date : 13 - 09 -2026
* Author : Saayujya Deshpande
* Technology Used : Win32 SDK, OpenGL
* Programming Language : C++ Language
* References:
	1. Gokhale sir's live code
*/

// header files
#include <windows.h>
#include <cstdio>  // for file io
#include <cstdlib> // for exit()
#include <cmath>
#include <vector>
#include <string>
#include <ctime>
using namespace std;

// OGL related headers
#include <gl/GL.h>
#include <gl/GLU.h>

#include "../include/alut.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imfilebrowser.h"

#include "../include/OGL.h"
#include "../include/Object.h"
#include "../include/Texture.h"
	
// link with OGL import library
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "lib/openal32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "lib/alut.lib")

// macros
#define radians(ang) ang * 3.141592f / 180.0f

#define WIN_WIDTH  800
#define WIN_HEIGHT 600

// global function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
extern IMGUI_IMPL_API LRESULT
ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

// global variables
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL; // OGL Rendering context handle
BOOL bFullscreen = FALSE;
BOOL bActiveWindow = FALSE;
BOOL bEscapeKeyIsPressed = FALSE;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev;
FILE* gpfile = NULL;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpszCmdLine, int iCmdShow)
{
	// function prototypes
	int initialize(void);
	void render(void);
	void update(void);
	void uninitialize(void);
	void enableConsole(void);
	void ImGuiUI(void);
	void ImGuiFrame(void);
	void initImGui(void);
	void renderMenu(void);
	void updateMenu(void);

	// variable declarations
	WNDCLASSEX wndclass;
	HWND hwnd = NULL;
	MSG msg;
	TCHAR szAppName[] = TEXT("RTR7_SVD");
	BOOL bDone = FALSE; // for game loop

	// code
	enableConsole();
	printf("console logging enabled\n");

	// create log file
	gpfile = fopen("log.txt", "w");
	if (gpfile == NULL)
	{
		MessageBox(NULL, TEXT("err: fopen() failed"),
			TEXT("Error"), MB_ICONERROR | MB_OK);
		exit(0);
	}
	else
		fprintf(gpfile, "SVD: Program started!\n");

	// WNDCLASSEX initialization
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));

	// register above WNDCLASS
	RegisterClassEx(&wndclass);

	// centering
	int screenwidth = GetSystemMetrics(SM_CXSCREEN); // CX: x count
	int screenheight = GetSystemMetrics(SM_CYSCREEN); // CY: y count

	// create the window
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szAppName,
		TEXT("SAAYUJYA VISHWAKUMAR DESHPANDE"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN |
		WS_CLIPSIBLINGS | WS_VISIBLE,
		(screenwidth - WIN_WIDTH) / 2,
		(screenheight - WIN_HEIGHT) / 2,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);			/* In the process of window creation,
						 * Windows does what it has to do and
						 * calls WndProc() with 1st argument
						 * set to window handle and the 2nd
						 * set to WM_CREATE (the value 1).
						 * WndProc() processes WM_CREATE msg
						 * and returns controls back to Windows */

						 // set global window handle
	ghwnd = hwnd;

	int iResult = initialize();
	if (iResult != 0)
	{
		fprintf(gpfile, "err %d in WinMain(): initialize()\n",
			iResult);
		DestroyWindow(hwnd);
		hwnd = NULL;
	}
	else
	{
		fprintf(gpfile, "initialize() in WinMain(): success\n");
	}

	// show window
	ShowWindow(hwnd, iCmdShow); /* Windows SENDS WndProc() WM_SIZE&
								 * WM_SHOWWINDOW messages */

								 // update window to paint its background
	UpdateWindow(hwnd);	// Windows SENDS WM_PAINT msg. to WndProc()

	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	// game loop
	while (bDone == FALSE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				bDone = TRUE;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else if (bActiveWindow == TRUE)
		{
			if (bEscapeKeyIsPressed)
			{
				updateMenu();
				renderMenu();
			}
			else
			{
				update(); // update
				ImGuiFrame();
				ImGuiUI();
				render(); // render
			}
		}
	}
	return((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg,
	WPARAM wParam, LPARAM lParam)
{
	// function prototypes
	void resize(int, int);
	void toggleFullscreen(void);
	void keyboard(unsigned char, int, int);
	void keyUp(unsigned char, int, int);
	void mouse(int, int, int, int);
	void uninitialize(void);

	// code
	if (ImGui_ImplWin32_WndProcHandler(hwnd, iMsg,
		wParam, lParam))
	{
		return(true);
	}
	else
	{
		switch (iMsg)
		{
		case WM_CREATE:
			memset(&wpPrev, 0, sizeof(WINDOWPLACEMENT));
			wpPrev.length = sizeof(WINDOWPLACEMENT);
			break;

		case WM_SETFOCUS:
			bActiveWindow = TRUE;
			break;

		case WM_KILLFOCUS:
			bActiveWindow = FALSE;
			break;

		case WM_SIZE:
			resize(LOWORD(lParam), HIWORD(lParam));
			break;

		case WM_KEYDOWN:
			keyboard((unsigned char)wParam,
				LOWORD(lParam), HIWORD(lParam));
			break;

		case WM_KEYUP:
			keyUp((unsigned char)wParam,
				LOWORD(lParam), HIWORD(lParam));
			break;

		case WM_LBUTTONDOWN:
			mouse(MK_LBUTTON, WM_LBUTTONDOWN, LOWORD(lParam),
				HIWORD(lParam));
			break;

		case WM_LBUTTONUP:
			mouse(MK_LBUTTON, WM_LBUTTONUP, LOWORD(lParam),
				HIWORD(lParam));
			break;

		case WM_RBUTTONDOWN:
			mouse(MK_RBUTTON, WM_RBUTTONDOWN, LOWORD(lParam),
				HIWORD(lParam));
			break;

		case WM_RBUTTONUP:
			mouse(MK_RBUTTON, WM_RBUTTONUP, LOWORD(lParam),
				HIWORD(lParam));
			break;

		case WM_MBUTTONDOWN:
			mouse(MK_MBUTTON, WM_MBUTTONDOWN, LOWORD(lParam),
				HIWORD(lParam));
			break;

		case WM_MBUTTONUP:
			mouse(MK_MBUTTON, WM_MBUTTONUP, LOWORD(lParam),
				HIWORD(lParam));
			break;

		case WM_MOUSEMOVE:
			mouse(WM_MOUSEMOVE, WM_MOUSEMOVE, LOWORD(lParam),
				HIWORD(lParam));
			break;

		case WM_CLOSE:
			uninitialize();
			break;

		case WM_DESTROY:
			FreeConsole();
			PostQuitMessage(0);
			break;

		default:
			break;
		}
		return(DefWindowProc(hwnd, iMsg, wParam, lParam));
	}
}

// typedefs
typedef struct Point
{
	GLfloat x, y, z;

	Point() {}
	Point(GLfloat x, GLfloat y,
		GLfloat z) : x(x), y(y), z(z) {}

	Point& operator+=(Point& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return(*this);
	}
	Point& operator-=(Point& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return(*this);
	}
	Point operator+(Point& v)
	{
		return(Point(x + v.x,
			y + v.y, z + v.z));
	}
	Point operator+(GLfloat f)
	{
		return(Point(x + f,
			y + f, z + f));
	}
	Point operator-(Point& v)
	{
		return(Point(x - v.x,
			y - v.y, z - v.z));
	}
	Point operator-(GLfloat f)
	{
		return(Point(x - f,
			y - f, z - f));
	}
	Point operator*(Point& v)
	{
		return(Point(y * v.z - z * v.y,
			z * v.x - x * v.z, x * v.y
			- y * v.x));
	}
	Point operator*(GLfloat f)
	{
		return(Point(x * f, y * f, z * f));
	}
	GLfloat magnitude(void)
	{
		return(sqrtf(x * x + y * y +
			z * z));
	}
	GLfloat* vect(void)
	{
		GLfloat* v = new GLfloat[3];
		v[0] = x; v[1] = y; v[2] = z;
		return(v);
	}
	Point unitv(void)
	{
		GLfloat mag = magnitude();
		mag = mag <= 0.00001f ?
			0.1f : mag;
		return(Point(x / mag,
			y / mag, z / mag));
	}
} Point;
typedef Point vec3;

// global function prototypes
void cube(GLfloat, GLfloat, GLfloat, GLfloat);

// class definitions
class KeyHandler
{
	// variables
	unsigned char key;
	int keyX, keyY;

public:
	// function prototypes
	void keyHandler(void);

	// constructor definitions
	KeyHandler() {}
	KeyHandler(unsigned char k, int x, int y) :
		key(k), keyX(x), keyY(y) {}
};

typedef struct Quad
{
	Object base;
} Quad;

void quadDescription(Object* obj)
{
	glPushMatrix();
	glColor4fv(vect(&obj->color));
	glTranslatef(obj->posX, obj->posY, obj->posZ);
	glRotatef(obj->ang,
		obj->axisX, obj->axisY, obj->axisZ);
	glScalef(obj->sizeX, obj->sizeY, obj->sizeZ);

	glBegin(GL_QUADS);
	for (int i = 0; i < 4; ++i)
	{
		glVertex3f(
			obj->vertices[i * 3 + 0],
			obj->vertices[i * 3 + 1],
			obj->vertices[i * 3 + 2]);
	}
	glEnd();
	glPopMatrix();
}

Quad* newQuad(void)
{
	// variables
	Quad* quad = (Quad*)malloc(sizeof(Quad));
	GLfloat vertices[] = {
					1.0f, 0.0f, -1.0f,
					-1.0f, 0.0f, -1.0f,
					-1.0f, 0.0f, 1.0f,
					1.0f, 0.0f, 1.0f };

	// code
	if (quad != NULL)
	{
		objectInit(&quad->base,
			Object::HANDLE_COLLISION, vertices,
			(GLuint)(sizeof(vertices) / sizeof(vertices[0])));
		quad->base.description = quadDescription;
	}
	return(quad);
}

typedef struct Cube
{
	Object base;
} Cube;

void cubeDescription(Object* obj)
{
	void quad(Point, Point, Point, Point);

	GLfloat r = obj->color.r;
	GLfloat g = obj->color.g;
	GLfloat b = obj->color.b;
	GLfloat a = obj->color.a;

	glPushMatrix();
	glTranslatef(obj->posX, obj->posY, obj->posZ);
	glRotatef(obj->ang,
		obj->axisX, obj->axisY, obj->axisZ);
	glScalef(obj->sizeX, obj->sizeY, obj->sizeZ);
	
	// front (z = +0.25)
	glColor4f(r - 0.05f, g - 0.05f,
		b - 0.05f, a);
	quad(Point(obj->vertices[0], obj->vertices[1], obj->vertices[2]),
		Point(obj->vertices[3], obj->vertices[4], obj->vertices[5]),
		Point(obj->vertices[6], obj->vertices[7], obj->vertices[8]),
		Point(obj->vertices[9], obj->vertices[10], obj->vertices[11]));

	// back (z = -0.25)
	glColor4f(r, g, b, a);
	quad(Point(obj->vertices[15], obj->vertices[16], obj->vertices[17]),
		Point(obj->vertices[18], obj->vertices[19], obj->vertices[20]),
		Point(obj->vertices[21], obj->vertices[22], obj->vertices[23]),
		Point(obj->vertices[12], obj->vertices[13], obj->vertices[14]));

	// top
	glColor4f(r + 0.05f, g + 0.05f,
		b + 0.05f, a);
	quad(Point(obj->vertices[0], obj->vertices[1], obj->vertices[2]),
		Point(obj->vertices[15], obj->vertices[16], obj->vertices[17]),
		Point(obj->vertices[18], obj->vertices[19], obj->vertices[20]),
		Point(obj->vertices[3], obj->vertices[4], obj->vertices[5]));

	// left
	glColor4f(r - 0.03f, g - 0.03f,
		b - 0.03f, a);
	quad(Point(obj->vertices[3], obj->vertices[4], obj->vertices[5]),
		Point(obj->vertices[18], obj->vertices[19], obj->vertices[20]),
		Point(obj->vertices[21], obj->vertices[22], obj->vertices[23]),
		Point(obj->vertices[6], obj->vertices[7], obj->vertices[8]));

	// bottom
	glColor4f(r + 0.03f, g + 0.03f,
		b + 0.03f, a);
	quad(Point(obj->vertices[6], obj->vertices[7], obj->vertices[8]),
		Point(obj->vertices[21], obj->vertices[22], obj->vertices[23]),
		Point(obj->vertices[12], obj->vertices[13], obj->vertices[14]),
		Point(obj->vertices[9], obj->vertices[10], obj->vertices[11]));

	// right
	glColor4f(r - 0.01f, g - 0.01f,
		b - 0.01f, a);
	quad(Point(obj->vertices[9], obj->vertices[10], obj->vertices[11]),
		Point(obj->vertices[12], obj->vertices[13], obj->vertices[14]),
		Point(obj->vertices[15], obj->vertices[16], obj->vertices[17]),
		Point(obj->vertices[0], obj->vertices[1], obj->vertices[2]));

	glPopMatrix();
}

Cube* newCube(void)
{
	// variables
	Cube* cube = (Cube*)malloc(sizeof(Cube));
	GLfloat vertices[] = {
						0.25f, 0.25f, 0.25f,
						-0.25f, 0.25f, 0.25f,
						-0.25f, -0.25f, 0.25f,
						0.25f, -0.25f, 0.25f,
						0.25f, -0.25f, -0.25f,
						0.25f, 0.25f, -0.25f,
						-0.25f, 0.25f, -0.25f,
						-0.25f, -0.25f, -0.25f };

	// code
	if (cube != NULL)
	{
		objectInit(&cube->base,
			Object::HANDLE_COLLISION, vertices,
			(int)(sizeof(vertices) / sizeof(vertices[0])));
		cube->base.description = cubeDescription;
	}
	return(cube);
}

typedef struct TexQuad2D
{
	Object base;
} TexQuad2D;

void texQuad2Ddescription(Object* obj)
{
	glPushMatrix();
	glTranslatef(obj->posX, obj->posY, obj->posZ);
	glRotatef(obj->ang,
		obj->axisX, obj->axisY, obj->axisZ);
	glScalef(obj->sizeX, obj->sizeY, obj->sizeZ);

	texQuad2D(obj->texture, obj->texCoords,
		obj->quadCoords);
	glPopMatrix();
}

GLuint textures[1];
GLfloat texCoords[] = { 0.0f, 0.0f,
							1.0f, 0.0f,
							1.0f, 1.0f,
							0.0f, 1.0f };
GLfloat quadCoords[] = { 0.0f, 0.0f,
						1.0f, 0.0f,
						1.0f, 1.0f,
						0.0f, 1.0f };

TexQuad2D* newTexQuad2D(const char* filename)
{
	// variables
	TexQuad2D* quad = (TexQuad2D*)malloc(
		sizeof(TexQuad2D));
	GLfloat vertices[] = {
					1.0f, 0.0f, -1.0f,
					-1.0f, 0.0f, -1.0f,
					-1.0f, 0.0f, 1.0f,
					1.0f, 0.0f, 1.0f };

	// code
	if (quad != NULL)
	{
		objectInit(&quad->base,
			Object::NO_COLLISION, vertices,
			(int)(sizeof(vertices) / sizeof(vertices[0])));

		// init texCoords
		quad->base.texCoords = (GLfloat*)calloc(
			8, sizeof(GLfloat));
		quad->base.quadCoords = (GLfloat*)calloc(
			8, sizeof(GLfloat));

		// init texture coords
		memcpy(quad->base.texCoords, texCoords,
			8 * sizeof(GLfloat));
		memcpy(quad->base.quadCoords, quadCoords,
			8 * sizeof(GLfloat));

		quad->base.texture = initTex(filename);
		quad->base.description =
			texQuad2Ddescription;
	}
	return(quad);
}

typedef struct Cylinder
{
	Object base;
} Cylinder;

void cylinderDescription(Object* obj)
{
	glPushMatrix();
	glColor4fv(vect(&obj->color));
	glTranslatef(obj->posX, obj->posY, obj->posZ);
	glRotatef(obj->ang,
		obj->axisX, obj->axisY, obj->axisZ);
	glRotatef(270.0f, 1.0, 0.0f, 0.0f);
	glScalef(obj->sizeX, obj->sizeY, obj->sizeZ);
	gluCylinder(obj->quadric, obj->baseRad,
		obj->topRad, 1.0, 20, 1);
	glPopMatrix();
}

Cylinder* newCylinder(void)
{
	// variables
	Cylinder* cylinder = (Cylinder*)malloc(
		sizeof(Cylinder));
	GLfloat vertices[] = {
						0.25f, 0.25f, 0.25f,
						-0.25f, 0.25f, 0.25f,
						-0.25f, -0.25f, 0.25f,
						0.25f, -0.25f, 0.25f,
						0.25f, -0.25f, -0.25f,
						0.25f, 0.25f, -0.25f,
						-0.25f, 0.25f, -0.25f,
						-0.25f, -0.25f, -0.25f };

	// code
	if (cylinder != NULL)
	{
		objectInit(&cylinder->base,
			Object::HANDLE_COLLISION, vertices,
			(int)(sizeof(vertices) / sizeof(vertices[0])));

		cylinder->base.quadric =
			gluNewQuadric();
		cylinder->base.description
			= cylinderDescription;
	}
	return(cylinder);
}

BOOL bPlayerKeyDown = FALSE;
BOOL bFootstepsAud = FALSE;
BOOL bIsKeydown = FALSE;
BOOL bFreeLook = TRUE;
BOOL bRmbDown = FALSE;
BOOL bMouseMove = TRUE;
BOOL bHandleKeys = TRUE;

KeyHandler keyhandler;
ALuint alSource[] = { 0 };
ALuint alBuff[] = { 0 };

vec3 eye(-2.506f, 0.688f, 2.515f);
vec3 lookdir(0.705f, -0.17f, -0.688f);
vec3 up(0.0f, 1.0f, 0.0f);

GLfloat angCube = 0.0f;
GLfloat currtime = 0.0f;
GLfloat camSinAng = 0.0f;
GLfloat playerHeight = 0.5f;

void handleKeysAndMouse(BOOL flag)
{
	bMouseMove = flag;
	bHandleKeys = flag;
}

typedef struct Player
{
	Object base;
} Player;

void playerDescription(Object* player)
{
	// code
	if (bFreeLook == FALSE)
	{
		if (bFootstepsAud == FALSE &&
			bPlayerKeyDown)
		{
			alSourcePlay(alSource[0]);
			bFootstepsAud = TRUE;
		}
		if (bPlayerKeyDown)
		{
			camSinAng = currtime * 7.0f;
			eye.y = playerHeight + (0.1f
				* sinf(camSinAng));
		}
		// update player loc
		player->posX = eye.x;
		player->posY = playerHeight;
		player->posZ = eye.z;
	}
}

void updatePlayer(Object* player)
{
	if (bFreeLook == FALSE)
	{
		eye.x = player->posX;
		eye.z = player->posZ;
	}
}

Player* newPlayer(void)
{
	// variables
	Player* player = (Player*)malloc(sizeof(Player));
	GLfloat v[1];

	// code
	if (player != NULL)
	{
		objectInit(&player->base,
			Object::HANDLE_COLLISION, v, 0);
		player->base.description = playerDescription;
		player->base.updateObject = updatePlayer;
	}
	return(player);
}

ObjVect objects = objVect(3);
vector<string> objlist;
int objIdx = -1;

int winwidth = WIN_WIDTH;
int winheight = WIN_HEIGHT;

void toggleFullscreen(void)
{
	// variables
	MONITORINFO mi;

	// code
	// check whether fullscreen or not
	if (bFullscreen == FALSE)
	{
		// get current window style
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);

		// check whether style contains WS_OVERLAPPEDWINDOW
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			memset(&mi, 0, sizeof(MONITORINFO));
			mi.cbSize = sizeof(MONITORINFO);

			// get current window placement & monitor information
			if (GetWindowPlacement(ghwnd, &wpPrev) &&
				GetMonitorInfo(MonitorFromWindow(ghwnd,
					MONITORINFOF_PRIMARY), &mi))
			{
				// remove WS_OVERLAPPEDWINDOW from current style
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle &
					~WS_OVERLAPPEDWINDOW);

				// set window pos to obtained current monitor info dim
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left,
					mi.rcMonitor.top,
					mi.rcMonitor.right - mi.rcMonitor.left,
					mi.rcMonitor.bottom - mi.rcMonitor.top,
					SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}

		// hide the cursor
		ShowCursor(FALSE);
	}
	else
	{
		// again add WS_OVERLAPPEDWINDOW into the style
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle |
			WS_OVERLAPPEDWINDOW);

		// set window placement to what it was before fullscreen
		SetWindowPlacement(ghwnd, &wpPrev);

		// set window pos accordingly as before
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER |
			SWP_NOZORDER | SWP_FRAMECHANGED);

		// show the cursor
		ShowCursor(TRUE);
	}
}

void enableConsole(void)
{
	// code
	AllocConsole();

	FILE* fout;
	freopen_s(&fout, "CONOUT$", "w", stdout);

	FILE* ferr;
	freopen_s(&ferr, "CONOUT$", "w", stderr);

	FILE* fin;
	freopen_s(&fin, "CONOUT$", "r", stdin);
}

#define EL_CONSOLE 1
#define EL_FILE    2

void ErrorLog(char err[], unsigned int err_dir_flag)
{
	switch (err_dir_flag)
	{
	case EL_CONSOLE:
		printf("%s\n", err);
		break;

	case EL_FILE:
		fprintf(gpfile, "%s\n", err);
		break;

	case EL_CONSOLE | EL_FILE:
		printf("%s\n", err);
		fprintf(gpfile, "%s\n", err);
		break;

	default:
		printf("%s\n", err);
		break;
	}
}

int initialize(void)
{
	// function prototypes
	void resize(int, int);
	void printGLInfo(void);
	void initALAud(void);
	void initImGui(void);

	// variables
	static BOOL alAudInit = FALSE;
	int iPixelFormatIndex;
	PIXELFORMATDESCRIPTOR pfd;

	// code
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	// ask OS to give PF to match rendering requirements
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1; // must be 1
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL
		| PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA; // 32bit pixel format
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;

	// get the device context
	ghdc = GetDC(ghwnd);
	if (ghdc == NULL)
	{
		printf("initialize(): GetDC() failed", EL_FILE);
		return(-1);
	}

	// choose and select OS given pixel format
	// index is 1 based, not 0 based
	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0)
	{
		ErrorLog("initialize(): ChoosePixelFormat() failed",
			EL_FILE);
		return(-1);
	}
	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd)
		== FALSE)
	{
		ErrorLog("initialize(): SetPixelFormat() failed",
			EL_FILE);
		return(-1);
	}

	// use bridging API to create rendering context
	// according to chosen pixel format & DC
	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		ErrorLog("initialize(): wglCreateContext() failed",
			EL_FILE);
		return(-1);
	}

	// make that rendering context: current context
	// for further rendering
	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		ErrorLog("initialize(): wglMakeCurrent() failed",
			EL_FILE);
		return(-1);
	}

	// print OGL info
	printGLInfo();

	// enable depth
	// give smooth coloring
	glShadeModel(GL_SMOOTH);

	// will make all vals. 1.0 in depth buffer
	// in render function
	glClearDepth(1.0);

	// if all vals. are 1.0 in depth then
	// how to know which is ahead/behind?
	// for this we enable depth test
	glEnable(GL_DEPTH_TEST);

	// which test to enable?
	// LEQUAL: less than equal
	// those pixels which are ahead of
	// 1.0 will have depth
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,
		GL_NICEST);

	// enable alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// start rendering API
	// choose screen clearing color: blue
	// glClearColor() just chooses color
	glClearColor(0.53f, 0.81f, 1.0f, 1.0f);

	// warmup resize
	resize(WIN_WIDTH, WIN_HEIGHT);

	// init OAL audio & imgui
	initALAud();
	initImGui();

	return(0);
}

void printGLInfo(void)
{
	// code
	fprintf(gpfile, "OpenGL Vendor: %s\n",
		glGetString(GL_VENDOR));
	fprintf(gpfile, "OpenGL Renderer: %s\n",
		glGetString(GL_RENDERER));
	fprintf(gpfile, "OpenGL Version: %s\n",
		glGetString(GL_VERSION));
}

GLdouble zFar = 200.0;

void resize(int width, int height)
{
	// code
	// height must not be 0 if
	// width / height is done
	if (height <= 0)
		height = 1;
	winwidth = width;
	winheight = height;

	glViewport(0, 0,
		(GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// perspective transformation
	gluPerspective(45.0,
		(GLdouble)width / (GLdouble)height,
		0.1, zFar);
}

void render(void)
{
	// function prototypes
	void scene(void);
	void ImGuiRender(void);

	// variables
	vec3 lookat = lookdir + eye;

	// code
	// clears screen with the color
	// chosen in glClearColor()
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// specify look at
	gluLookAt(eye.x, eye.y, eye.z,
		lookat.x, lookat.y, lookat.z,
		up.x, up.y, up.z);

	// draw the scene
	scene();
	ImGuiRender();

	// double buffering
	SwapBuffers(ghdc);
}

void keyboard(unsigned char key, int x, int y)
{
	// function prototypes
	void freeLook(int, int, int, int);
	void menuKeyHandler(unsigned char key, int x, int y);

	ImGuiIO& io = ImGui::GetIO();

	// code
	if (io.WantCaptureKeyboard || io.WantTextInput)
		return;

	if (bIsKeydown == FALSE)
	{
		bIsKeydown = TRUE;
		keyhandler = KeyHandler(key, x, y);
	}
	switch (key)
	{
	case VK_ESCAPE:
		if (bEscapeKeyIsPressed)
			bEscapeKeyIsPressed = FALSE;
		else
			bEscapeKeyIsPressed = TRUE;
		break;

	case 'F':
	case 'f':
		if (bFullscreen == FALSE)
		{
			toggleFullscreen();
			bFullscreen = TRUE;
		}
		else
		{
			toggleFullscreen();
			bFullscreen = FALSE;
		}
		break;

		/*export{*/
	case 'O':
	case 'o':
		eye = vec3(-2.506f, 0.688f, 2.515f);
		lookdir = vec3(0.705f, -0.17f, -0.688f);
		break;

	case 'P':
	case 'p':
		if (bFreeLook == TRUE)
			bFreeLook = FALSE;
		else
		{
			// remove the person-cam effect
			freeLook(MK_RBUTTON, WM_RBUTTONUP,
				x, y);
			bFreeLook = TRUE;
		}
		break;

	default:
		break;
	}
	if (bEscapeKeyIsPressed)
	{
		menuKeyHandler(key, x, y);
	}
}

void keyUp(unsigned char key, int x, int y)
{
	if (bIsKeydown == TRUE)
		bIsKeydown = FALSE;
	
	switch (key)
	{
	case 'W':
	case 'w':
	case 'S':
	case 's':
	case 'A':
	case 'a':
	case 'D':
	case 'd':
		if (bFootstepsAud == TRUE)
		{
			alSourceStop(alSource[0]);
			bFootstepsAud = FALSE;
		}
		if(bPlayerKeyDown)
			bPlayerKeyDown = FALSE;
		break;

	default:
		break;
	}
}

void mouse(int button, int state, int x, int y)
{
	// function prototypes
	void freeLook(int, int, int, int);

	// code
	freeLook(button, state, x, y);
	if (bFreeLook == FALSE)
	{
		// deliberately keep RMB state
		// always down
		freeLook(MK_RBUTTON, WM_RBUTTONDOWN,
			x, y);
	}
}

void update(void)
{
	// variables
	static Player player;
	static BOOL bPlayerInit = FALSE;
	clock_t current_time = clock();
	GLfloat v[1];
	float time_in_milliseconds = (float)current_time;

	// code
	currtime = (GLfloat)time_in_milliseconds / 1000.0f;

	if (camSinAng >= 6.28f)
		camSinAng = 0.0f;

	if (bIsKeydown)
		keyhandler.keyHandler();

	if (bPlayerInit == FALSE)
	{
		objectInit(&player.base,
			Object::HANDLE_COLLISION, v, 0);
		player.base.description = playerDescription;
		player.base.updateObject = updatePlayer;
		bPlayerInit = TRUE;
	}
	
	if (bPlayerInit)
		handleCollisionsOf(&player.base);
}

void uninitialize(void)
{
	// function prototypes
	void uninitALAud(void);
	void uninitImGui(void);

	// code
	// delete the object's vector
	deleteObjVect(&objects);

	// uninitialize AL audio & imgui
	uninitImGui();
	uninitALAud();
	
	// if exiting in fullscreen, first restore
	// and then proceed
	if (bFullscreen == TRUE)
	{
		toggleFullscreen();
		bFullscreen = FALSE;
	}

	// first check the current context and if it is,
	// then unmake it as the current context
	if (wglGetCurrentContext() == ghrc)
		wglMakeCurrent(NULL, NULL);

	// now destroy the rendering context
	if (ghrc)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}

	// release the DC
	if (ghdc)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	// destroy window
	if (ghwnd)
	{
		DestroyWindow(ghwnd);
		ghwnd = NULL;
	}

	// close log file
	if (gpfile != NULL)
	{
		fprintf(gpfile, "SVD: Program terminated!\n");
		fclose(gpfile);
		gpfile = NULL;
	}
}

void KeyHandler::keyHandler(void)
{
	// variables
	GLfloat camspeed = 0.05f;

	// code
	if (bHandleKeys)
	{
		switch (key)
		{
		case 'W':
		case 'w':
			eye += lookdir.unitv() * camspeed;
			bPlayerKeyDown = TRUE;
			break;

		case 'S':
		case 's':
			eye -= lookdir.unitv() * camspeed;
			bPlayerKeyDown = TRUE;
			break;

		case 'A':
		case 'a':
			eye -= vec3(lookdir * up).unitv() *
				camspeed;
			bPlayerKeyDown = TRUE;
			break;

		case 'D':
		case 'd':
			eye += vec3(lookdir * up).unitv() *
				camspeed;
			bPlayerKeyDown = TRUE;
			break;

		default:
			bPlayerKeyDown = FALSE;
			break;
		}
	}
	else
	{
		keyUp(key, keyX, keyY);
	}
}

void scene(void)
{
	// function prototypes
	void grid(void);
	void imguiDraw(void (*)(void));
	void something(void);
	void ImGuiUpdate(void);

	// code
	grid();
	drawObjects(&objects);
	ImGuiUpdate();
}

void quad(GLfloat r, GLfloat g, GLfloat b)
{
	glColor4f(r, g, b, 1.0f);
	glBegin(GL_QUADS);

	glVertex3f(1.0f, 0.0f, -1.0f);
	glVertex3f(-1.0f, 0.0f, -1.0f);
	glVertex3f(-1.0f, 0.0f, 1.0f);
	glVertex3f(1.0f, 0.0f, 1.0f);

	glEnd();
	glPopMatrix();
}

void cube(GLfloat r, GLfloat g, GLfloat b,
	GLfloat a)
{
	// function prototypes
	void quad(Point, Point, Point, Point);

	// code
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.25f);

	glColor4f(r - 0.05f, g - 0.05f,
		b - 0.05f, a);
	quad(Point(0.25f, 0.25f, 0.0f),
		Point(-0.25f, 0.25f, 0.0f),
		Point(-0.25f, -0.25f, 0.0f),
		Point(0.25f, -0.25f, 0.0f));

	glColor4f(r, g, b, a);
	quad(Point(0.25f, 0.25f, -0.5f),
		Point(-0.25f, 0.25f, -0.5f),
		Point(-0.25f, -0.25f, -0.5f),
		Point(0.25f, -0.25f, -0.5f));

	glColor4f(r + 0.05f, g + 0.05f,
		b + 0.05f, a);
	quad(Point(0.25f, 0.25f, 0.0f),
		Point(0.25f, 0.25f, -0.5f),
		Point(-0.25f, 0.25f, -0.5f),
		Point(-0.25f, 0.25f, 0.0f));

	glColor4f(r - 0.03f, g - 0.03f,
		b - 0.03f, a);
	quad(Point(-0.25f, 0.25f, 0.0f),
		Point(-0.25f, 0.25f, -0.5f),
		Point(-0.25f, -0.25f, -0.5f),
		Point(-0.25f, -0.25f, 0.0f));

	glColor4f(r + 0.03f, g + 0.03f,
		b + 0.03f, a);
	quad(Point(-0.25f, -0.25f, 0.0f),
		Point(-0.25f, -0.25f, -0.5f),
		Point(0.25f, -0.25f, -0.5f),
		Point(0.25f, -0.25f, 0.0f));

	glColor4f(r - 0.01f, g - 0.01f,
		b - 0.01f, a);
	quad(Point(0.25f, -0.25f, 0.0f),
		Point(0.25f, -0.25f, -0.5f),
		Point(0.25f, 0.25f, -0.5f),
		Point(0.25f, 0.25f, 0.0f));
	glPopMatrix();
}

void quad(Point a, Point b, Point c,
	Point d)
{
	glBegin(GL_QUADS);
	glVertex3f(a.x, a.y, a.z);
	glVertex3f(b.x, b.y, b.z);
	glVertex3f(c.x, c.y, c.z);
	glVertex3f(d.x, d.y, d.z);
	glEnd();
}

void newCollisionCube(vec3 pos, vec3 size,
	GLfloat ang, vec3 axis)
{
	// variables
	Cube* cube = newCube();

	// code
	cube->base.ang = ang;
	cube->base.color.a = 0.0f;
	cube->base.axisX = axis.x;
	cube->base.axisY = axis.y;
	cube->base.axisZ = axis.z;
	cube->base.posX = pos.x;
	cube->base.posY = pos.y;
	cube->base.posZ = pos.z;
	cube->base.sizeX = size.x;
	cube->base.sizeY = size.y;
	cube->base.sizeZ = size.z;
	
	objects.pushBack(&objects, cube->base);
	free(cube);
}

void freeLook(int button, int state, int x, int y)
{
	// variables
	BOOL bCamSnapped = FALSE;
	POINT cursScreenPos, cursClientPos;
	RECT rect;
	const int edgeWrapThresh = 2;
	const int wrapLandMargin = 4;
	const GLfloat sensitivity = 0.1f;
	static GLfloat yaw_rad = 0.0f;
	static GLfloat pitch_rad = 0.0f;
	static POINT prevCursScreenPos = { 0, 0 };

	// code
	switch (button)
	{
	case MK_RBUTTON:
		if (state == WM_RBUTTONDOWN)
		{
			if (bFreeLook)
			{
				// set the baseline,
				// so the first move after pressing
				// produces a small delta
				GetCursorPos(&prevCursScreenPos);

				// sync angles with curr lookdir
				GLfloat lookY = lookdir.y;

				if (lookY > 1.0f)
					lookY = 1.0f;
				else if (lookY < -1.0f)
					lookY = -1.0f;

				// get the pitch & yaw angles from lookdir
				pitch_rad = asinf(lookY);
				yaw_rad = atan2f(lookdir.x, -lookdir.z);

				// keep mouse events coming even when
				// the cursor leaves the window
				SetCapture(ghwnd);
			}
			bRmbDown = TRUE;
		}
		else
		{
			// stop bRmbDown from getting stuck
			ReleaseCapture();
			bRmbDown = FALSE;
		}
		break;

	case WM_MOUSEMOVE:
		if (bRmbDown && (state == WM_MOUSEMOVE) &&
			bMouseMove == TRUE)
		{
			GetCursorPos(&cursScreenPos);

			int deltaX = cursScreenPos.x
				- prevCursScreenPos.x;
			int deltaY = cursScreenPos.y
				- prevCursScreenPos.y;

			if (deltaX == 0 && deltaY == 0)
				break;

			yaw_rad += radians(deltaX * sensitivity);
			pitch_rad += radians(-deltaY * sensitivity);

			if (pitch_rad > radians(89.0f))
				pitch_rad = radians(89.0f);
			else if (pitch_rad < radians(-89.0f))
				pitch_rad = radians(-89.0f);

			lookdir.x = cosf(pitch_rad)
				* sinf(yaw_rad);
			lookdir.y = sinf(pitch_rad);
			lookdir.z = -cosf(pitch_rad)
				* cosf(yaw_rad);
			lookdir = lookdir.unitv();

			cursClientPos = cursScreenPos;
			ScreenToClient(ghwnd, &cursClientPos);
			GetClientRect(ghwnd, &rect);

			if (cursScreenPos.x >= rect.right
				- edgeWrapThresh)
			{
				cursClientPos.x = rect.left
					+ wrapLandMargin;
				bCamSnapped = TRUE;
			}
			else if (cursClientPos.x <= rect.left
				+ edgeWrapThresh)
			{
				cursClientPos.x = rect.right
					- wrapLandMargin;
				bCamSnapped = TRUE;
			}

			if (bCamSnapped)
			{
				ClientToScreen(ghwnd, &cursClientPos);
				SetCursorPos(cursClientPos.x,
					cursClientPos.y);
				prevCursScreenPos = cursClientPos;
			}
			else
			{
				prevCursScreenPos = cursScreenPos;
			}
		}
		break;

	default:
		break;
	}
}

void grid(void)
{
	// code
	glColor3f(0.5f, 0.5f, 0.5f);
	glPushMatrix();

	glBegin(GL_LINES);
	for (int x = -30;
		x <= 30; ++x)
	{
		glVertex3f((GLfloat)x,
			0.0f, 30.0f);
		glVertex3f((GLfloat)x,
			0.0f, -30.0f);
		for (int z = -30;
			z <= 30; ++z)
		{
			glVertex3f((GLfloat)x,
				0.0f, (GLfloat)z);
			glVertex3f(-(GLfloat)x,
				0.0f, (GLfloat)z);
		}
	}
	glEnd();
	glPopMatrix();
}

void initALAud(void)
{
	// variables
	ALCdevice* alcDev = NULL;
	ALboolean bEAX = FALSE, alLoop;
	ALenum alFormat;
	ALsizei alSize, alFreq;
	ALvoid* alData = NULL;

	// code
	// initialize OpenAL
	// open default OAL device
	alcDev = alcOpenDevice(NULL);
	if (alcDev != NULL)
	{
		ALCcontext* alcContext =
			alcCreateContext(alcDev, NULL);
		if (alcContext != NULL)
			alcMakeContextCurrent(alcContext);
		else
			printf("No ALC context found\n");
	}
	else
	{
		printf("No ALC device found\n");
	}

	// check for EAX 2.0 support
	if (bEAX == FALSE)
		bEAX = alIsExtensionPresent("EAX2.0");

	// generate buffers
	// first clear error code by
	// a dummy alGetError() call
	alGetError();
	alGenBuffers(1, alBuff);
	if (alGetError() != AL_NO_ERROR)
		printf("err: alGenBuffers()\n");

	// load test.wav file if required
	alutLoadWAVFile("rsc/OutDoorFootsteps.wav", &alFormat,
		&alData, &alSize, &alFreq, &alLoop);
	if (alGetError() != AL_NO_ERROR)
	{
		printf("err: alutLoadWAVFile()\n");
		alDeleteBuffers(1, alBuff);
	}
	else
	{
		// copy test.wav data into AL buffer 0
		alBufferData(alBuff[0], alFormat, alData,
			alSize, alFreq);
		if (alGetError() != AL_NO_ERROR)
		{
			printf("err: alBufferData()\n");
			alDeleteBuffers(1, alBuff);
		}

		// unload test.wav file
		alutUnloadWAV(alFormat, alData, alSize, alFreq);
		if (alGetError() != AL_NO_ERROR)
		{
			printf("err: alutUnloadWAV()\n");
			alDeleteBuffers(1, alBuff);
		}
		else
		{
			alData = NULL;
		}

		// generate sources
		alGenSources(1, alSource);
		if (alGetError() != AL_NO_ERROR)
			printf("err: alGenSources()\n");

		// attach buffer 0 to source
		alSourcei(alSource[0], AL_BUFFER, alBuff[0]);
		if (alGetError() != AL_NO_ERROR)
			printf("err: alSourcei()\n");

		// specify if you want to play in loop
		alSourcei(alSource[0], AL_LOOPING, AL_TRUE);
	}
}

void uninitALAud(void)
{
	// variables
	ALCcontext* alContext = alcGetCurrentContext();
	ALCdevice* alDev = alcGetContextsDevice(alContext);
	ALsizei n = sizeof(alSource) / sizeof(ALuint);

	// code
	alDeleteSources(n, alSource);
	alDeleteBuffers(n, alBuff);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(alContext);
	alcCloseDevice(alDev);

	alContext = NULL;
	alDev = NULL;
}

// imfilebrowser initialization
ImGui::FileBrowser fileDialog;

void initImGui(void)
{
	// setup imgui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// apply styling theme
	ImGui::StyleColorsDark();

	// init platform/renderer backends
	ImGui_ImplWin32_Init(ghwnd);
	ImGui_ImplOpenGL3_Init("#version 460");

	// config imfilebrowser props
	fileDialog.SetTitle("Select a .jpg");
	fileDialog.SetTypeFilters({ ".jpg" });
}

void ImGuiFrame(void)
{
	// code
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

BOOL bImGuiExport = FALSE;
BOOL bImGuiDraw = FALSE;

GLfloat posX = 0.0f, posY = 0.0f, posZ = 0.0f;
GLfloat ang = 0.0f, axisX = 0.0f, axisY = 0.0f, axisZ = 0.0f;
GLfloat sizeX = 1.0f, sizeY = 1.0f, sizeZ = 1.0f;

void imguiDraw(void (*func)(void) = NULL)
{
	if (func != NULL)
	{
		bImGuiDraw = TRUE;
		glPushMatrix();
		glTranslatef(posX, posY, posZ);
		glRotatef(ang,
			axisX, axisY, axisZ);
		glScalef(sizeX, sizeY, sizeZ);
		func();
		glPopMatrix();
	}
	else
	{
		bImGuiDraw = FALSE;
	}
}

void ImGuiUI()
{
	// variables
	BOOL bObjSelected = FALSE;

	// code
	ImGui::Begin("ImGui Wnd");

	// define UI below
	ImGui::Text("FPS: %.1f",
		ImGui::GetIO().Framerate);

	if(ImGui::Button("Export code"))
		bImGuiExport = TRUE;
	if (ImGui::Button("Remove Latest Obj."))
	{
		if (objIdx > -1)
			--objIdx;
		if (objects.objs > 0)
			objects.popBack(&objects);
		if (objlist.size() > 0)
			objlist.pop_back();
	}

	ImGui::BeginChild("Objects", ImVec2(0, 200),
		true);

	for (int i = 0; i < objlist.size(); ++i)
	{
		if (ImGui::Selectable(objlist[i].c_str(),
			objIdx == i))
		{
			objIdx = i;
			vec3 objloc(objects.objects[objIdx].posX,
				objects.objects[objIdx].posY,
				objects.objects[objIdx].posZ);
			eye = objloc - vec3(1.5f, -0.5f, -1.5f);
			lookdir = objloc - eye;
		}
	}
	ImGui::EndChild();

	if (ImGui::Button("Quad"))
	{
		Quad* quad = newQuad();
		objects.pushBack(&objects, quad->base);
		free(quad);

		objlist.push_back(to_string(objlist.size())
			+ ") Quad");
	}
	if (ImGui::Button("Cube"))
	{
		Cube* cube = newCube();
		objects.pushBack(&objects, cube->base);
		free(cube);

		objlist.push_back(to_string(objlist.size())
			+ ") Cube");
	}
	if (ImGui::Button("Cylinder"))
	{
		Cylinder* cylinder = newCylinder();
		objects.pushBack(&objects, cylinder->base);
		free(cylinder);

		objlist.push_back(to_string(objlist.size())
			+ ") Cylinder");
	}
	if (ImGui::Button("Texture Quad"))
	{
		// open the imfilebrowser dialog
		fileDialog.Open();
	}
	if (fileDialog.HasSelected())
	{
		filesystem::path filePath =
			fileDialog.GetSelected();
		string filename = filePath.string();

		TexQuad2D* quad = newTexQuad2D(
			filename.c_str());
		objects.pushBack(&objects, quad->base);

		free(quad);
		fileDialog.ClearSelected();

		objlist.push_back(to_string(objlist.size())
			+ ") Texture quad");
	}

	if (objIdx >= 0)
	{
		Object* obj = &objects.objects[objIdx];
		static GLfloat col[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		
		ImGui::Text("Color:");
		ImGui::ColorPicker4("##picker", col);

		obj->color.r = col[0];
		obj->color.g = col[1];
		obj->color.b = col[2];
		obj->color.a = col[3];

		for (int i = 0; i < obj->vertexCount - 2; i += 3)
		{
			ImGui::PushID(i);
			if (ImGui::CollapsingHeader(("Vertex: " + to_string(i / 3)).c_str()))
			{
				ImGui::SliderFloat("X", &obj->vertices[i + 0], -100.0f, 100.0f);
				ImGui::SliderFloat("Y", &obj->vertices[i + 1], -100.0f, 100.0f);
				ImGui::SliderFloat("Z", &obj->vertices[i + 2], -100.0f, 100.0f);
			}
			ImGui::PopID();
		}

		if (ImGui::CollapsingHeader("Translate:"))
		{
			ImGui::SliderFloat("X", &obj->posX, -100.0f, 100.0f);
			ImGui::SliderFloat("Y", &obj->posY, -100.0f, 100.0f);
			ImGui::SliderFloat("Z", &obj->posZ, -100.0f, 100.0f);
		}

		if (ImGui::CollapsingHeader("Rotate:"))
		{
			ImGui::SliderFloat("ang", &obj->ang, 0.0f, 360.0f);
			ImGui::SliderFloat("X axis", &obj->axisX, 0.0f, 1.0f);
			ImGui::SliderFloat("Y axis", &obj->axisY, 0.0f, 1.0f);
			ImGui::SliderFloat("Z axis", &obj->axisZ, 0.0f, 1.0f);
		}

		if (ImGui::CollapsingHeader("Scale:"))
		{
			ImGui::SliderFloat("X", &obj->sizeX, -100.0f, 100.0f);
			ImGui::SliderFloat("Y", &obj->sizeY, -100.0f, 100.0f);
			ImGui::SliderFloat("Z", &obj->sizeZ, -100.0f, 100.0f);
		}

		float base = (float)obj->baseRad;
		float top = (float)obj->topRad;
		if (ImGui::CollapsingHeader("Radius:"))
		{
			if (ImGui::SliderFloat("Base", &base, 0.0f, 100.0f)) obj->baseRad = base;
			if (ImGui::SliderFloat("Top", &top, 0.0f, 100.0f)) obj->topRad = top;
		}

		bImGuiDraw = FALSE;
	}
	else if (bImGuiDraw)
	{
		if (ImGui::CollapsingHeader("Translate:"))
		{
			ImGui::SliderFloat("X", &posX, -100.0f, 100.0f);
			ImGui::SliderFloat("Y", &posY, -100.0f, 100.0f);
			ImGui::SliderFloat("Z", &posZ, -100.0f, 100.0f);
		}

		if (ImGui::CollapsingHeader("Rotate:"))
		{
			ImGui::SliderFloat("ang", &ang, 0.0f, 360.0f);
			ImGui::SliderFloat("X axis", &axisX, 0.0f, 1.0f);
			ImGui::SliderFloat("Y axis", &axisY, 0.0f, 1.0f);
			ImGui::SliderFloat("Z axis", &axisZ, 0.0f, 1.0f);
		}

		if (ImGui::CollapsingHeader("Scale:"))
		{
			ImGui::SliderFloat("X", &sizeX, -100.0f, 100.0f);
			ImGui::SliderFloat("Y", &sizeY, -100.0f, 100.0f);
			ImGui::SliderFloat("Z", &sizeZ, -100.0f, 100.0f);
		}
	}

	// end imgui UI
	ImGui::End();
}

void ImGuiRender(void)
{
	// code
	fileDialog.Display();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(
		ImGui::GetDrawData());
}

void ImGuiUpdate(void)
{
	// variables
	FILE* f_imgui = NULL;

	// code
	if (f_imgui == NULL && bImGuiExport == TRUE)
	{
		if ((f_imgui = fopen("imgui.txt", "a"))
			== NULL)
		{
			printf("err: opening imgui file\n");
			bImGuiExport = FALSE;
		}
		else
		{
			for (int i = 0; i < objects.objs; ++i)
			{
				Object* obj = &objects.objects[i];

				fprintf(f_imgui, "\nglPushMatrix();\n"
					"glColor4f(%.3ff, %.3ff, %.3ff, %.3ff);\n"
					"glTranslatef(%.3ff, %.3ff, %.3ff);\n"
					"glRotatef(%.3ff, %.3ff, %.3ff, %.3ff);\n"
					"glScalef(%.3ff, %.3ff, %.3ff);\n"
					"%s(vec3(%.3ff, %.3ff, %.3ff),\n"
					"vec3(%.3ff, %.3ff, %.3ff), %.3ff,\n"
					"vec3(%.3ff, %.3ff, %.3ff));\n"
					"glPopMatrix();\n", obj->color.r,
					obj->color.g, obj->color.b, obj->color.a,
					obj->posX,
					obj->posY, obj->posZ, obj->ang,
					obj->axisX, obj->axisY,obj->axisZ,
					obj->sizeX, obj->sizeY, obj->sizeZ,
					objlist[i].c_str(),
					posX, posY, posZ,
					sizeX, sizeY, sizeZ,
					ang, axisX, axisY, axisZ);
				bImGuiExport = FALSE;
			}
		}
		if (bImGuiDraw)
		{
			fprintf(f_imgui, "\nglPushMatrix();\n"
				"glTranslatef(%.3ff, %.3ff, %.3ff);\n"
				"glRotatef(%.3ff,"
				"%.3ff, %.3ff, %.3ff);\n"
				"glScalef(%.3ff, %.3ff, %.3ff);\n"
				"<func(vec3(%.3ff, %.3ff, %.3ff),\n"
				"vec3(%.3ff, %.3ff, %.3ff), %.3ff,\n"
				"vec3(%.3ff, %.3ff, %.3ff))>\n"
				"glPopMatrix();\n", posX,
				posY, posZ, ang,
				axisX, axisY, axisZ,
				sizeX, sizeY, sizeZ,
				posX, posY, posZ,
				sizeX, sizeY, sizeZ,
				ang, axisX, axisY, axisZ);
			bImGuiExport = FALSE;
		}
	}
	if (f_imgui)
	{
		fclose(f_imgui);
		f_imgui = NULL;
	}
}

void uninitImGui(void)
{
	// code
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void quad2D(void)
{
	glBegin(GL_QUADS);
	glVertex3f(-1.0f, -1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, 0.0f);
	glEnd();
}

GLuint menuChoice = 1;
GLuint choices = 2;
BOOL bChoiceLocked = FALSE;

void renderMenu(void)
{
	// code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// switch to 2D projection for menu
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

	// modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	// disable the depth test &
	// texture 2D
	glDisable(GL_DEPTH_TEST);

	// render the menu
	glColor3f(0.0f, 0.0f, 0.0f);
	quad2D();

	// show the menu textures
	glPushMatrix();
	glScalef(0.8f, 0.7f, 0.0f);
	glTranslatef(-0.5f, 0.7f, 0.0f);
	texQuad2D(initTex("rsc/GamePaused.jpg"),
		texCoords, quadCoords);
	glPopMatrix();

	glPushMatrix();
	glScalef(0.5f, 0.1f, 0.0f);
	glTranslatef(-0.5f, 0.0f, 0.0f);
	texQuad2D(initTex("rsc/ResumeGame.jpg"),
		texCoords, quadCoords);
	glPopMatrix();

	glPushMatrix();
	glScalef(0.3f, 0.1f, 0.0f);
	glTranslatef(-0.5f, -1.2f, 0.0f);
	texQuad2D(initTex("rsc/QuitGame.jpg"),
		texCoords, quadCoords);
	glPopMatrix();

	// selection quad
	glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
	glPushMatrix();
	glScalef(0.3f, 0.1f, 0.0f);
	switch (menuChoice)
	{
	case 1:
		glTranslatef(0.0f, 1.0f, 0.0f);
		break;

	case 2:
		glTranslatef(0.0f, -1.0f, 0.0f);
		break;
	}
	quad2D();
	glPopMatrix();

	// enable the depth test &
	glEnable(GL_DEPTH_TEST);

	// get back to 3D state
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	SwapBuffers(ghdc);
}

void updateMenu(void)
{
	if (menuChoice < 1)
		menuChoice = 1;
	else if (menuChoice > choices)
		menuChoice = choices;

	if (bChoiceLocked)
	{
		switch (menuChoice)
		{
		case 1:
			bEscapeKeyIsPressed = FALSE;
			break;

		case 2:
			if (MessageBox(ghwnd, TEXT("Sure you want to quit?"),
				TEXT("Question"), MB_ICONQUESTION | MB_YESNO)
				== IDYES)
			{
				PostMessage(ghwnd, WM_CLOSE, 0, 0);
			}
			break;

		default:
			break;
		}
		bChoiceLocked = FALSE;
	}
}

void menuKeyHandler(unsigned char key, int x, int y)
{
	switch (key)
	{
	case VK_UP:
		menuChoice -= 1;
		break;

	case VK_DOWN:
		menuChoice += 1;
		break;

	case VK_RETURN:
		bChoiceLocked = TRUE;
		break;

	default:
		break;
	}
}
