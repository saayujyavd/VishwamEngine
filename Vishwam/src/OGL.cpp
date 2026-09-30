/*
* Title : Blue screen
* Date : 13 - 09 -2026
* Author : Saayujya Deshpande
* Technology Used : Win32 SDK
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
using namespace std;

// OGL related headers
#include <gl/GL.h>
#include <al/ALUT.h>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>

#include "../include/OGL.h"
#include "../include/SDUT.h"

// link with OGL import library
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "openal32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "alut.lib")
#pragma comment(lib, "lib/sdut.lib")

// macros
#define radians(ang) ang * 3.141592f / 180.0f

#define WIN_WIDTH  800
#define WIN_HEIGHT 600

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
typedef Point Color;
typedef Point vec3;

// global function prototypes
void cube(Color&);

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

class Object
{
	// variables
private:
	inline static vector<Object*>* objs = nullptr;
public:
	Color color = Color(0.0f, 0.0f, 0.0f);
	GLfloat posX = 0.0f, posY = 0.0f, posZ = 0.0f;
	GLfloat sizeX = 1.0f, sizeY = 1.0f, sizeZ = 1.0f;
	GLfloat ang = 0.0f;
	GLfloat axisX = 0.0f, axisY = 0.0f, axisZ = 0.0f;

	// function definitions
private:
	bool collided(Object& obj)
	{
		// code
		GLfloat halfSizeX = sizeX / 4.0f;
		GLfloat halfSizeY = sizeY / 4.0f;
		GLfloat halfSizeZ = sizeZ / 4.0f;

		GLfloat leftA = posX - sizeX / 4.0f;
		GLfloat rightA = posX + sizeX / 4.0f;
		GLfloat bottomA = posY - sizeY / 4.0f;
		GLfloat topA = posY + sizeY / 4.0f;
		GLfloat rearA = posZ - sizeZ / 4.0f;
		GLfloat frontA = posZ + sizeZ / 4.0f;

		GLfloat leftB = obj.posX - obj.sizeX / 4.0f;
		GLfloat rightB = obj.posX + obj.sizeX / 4.0f;
		GLfloat bottomB = obj.posY - obj.sizeY / 4.0f;
		GLfloat topB = obj.posY + obj.sizeY / 4.0f;
		GLfloat rearB = obj.posZ - obj.sizeZ / 4.0f;
		GLfloat frontB = obj.posZ + obj.sizeZ / 4.0f;

		if ((leftA <= rightB && rightA >= leftB)
			&& (bottomA <= topB && topA >= bottomB)
			&& (rearA <= frontB && frontA >= rearB))
		{
			if (leftA >= leftB)
				posX += rightB - leftA + (obj.sizeX / 4.0f);
			else
				posX -= rightA - leftB + (obj.sizeX / 4.0f);

			if (bottomA >= bottomB)
				posY += topB - bottomA + (obj.sizeY / 4.0f);
			else
				posY -= topA - bottomB + (obj.sizeY / 4.0f);

			if (rearA >= rearB)
				posZ += frontB - rearA + (obj.sizeZ / 4.0f);
			else
				posZ -= frontA - rearB + (obj.sizeZ / 4.0f);

			return(true);
		}
		else return(false);
	}

protected:
	virtual void draw(void) = 0;

public:
	static void drawObjects(vector<Object*>& objects)
	{
		objs = &objects;
		for (int i = 0; i < objects.size(); ++i)
			objects[i]->draw();
	}

	static void checkCollisions(void)
	{
		if (objs == nullptr) return;
		for (int i = 0; i < objs->size(); ++i)
		{
			Object* obj = (*objs)[i];
			for (int j = i + 1; j < objs->size(); ++j)
			{
				if (obj->collided(*(*objs)[j]))
					printf("collision\n");
			}
		}
	}
};

class Quad : public Object
{
	// function definitions
	void draw(void) override
	{
		glPushMatrix();
		glColor3fv(color.vect());
		glTranslatef(posX, posY, posZ);
		glRotatef(ang, axisX, axisY, axisZ);
		glScalef(sizeX, sizeY, sizeZ);

		glBegin(GL_QUADS);
		glVertex3f(1.0f, 0.0f, -1.0f);
		glVertex3f(-1.0f, 0.0f, -1.0f);
		glVertex3f(-1.0f, 0.0f, 1.0f);
		glVertex3f(1.0f, 0.0f, 1.0f);

		glEnd();
		glPopMatrix();
	}
};

class Cube : public Object
{
	// function definitions
	void draw(void) override
	{
		glPushMatrix();
		glTranslatef(posX, posY, posZ);
		glRotatef(ang, axisX, axisY, axisZ);
		glScalef(sizeX, sizeY, sizeZ);
		cube(Color(color.x, color.y, color.z));
		glPopMatrix();
	}
};

BOOL bFootstepsAud = FALSE;
BOOL bIsKeydown = FALSE;
BOOL bFreeLook = TRUE;
BOOL bRmbDown = FALSE;

KeyHandler keyhandler;
ALuint alSource[] = { 0 };

int winwidth = WIN_WIDTH;
int winheight = WIN_HEIGHT;

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
		else
		{
			if (bActiveWindow == TRUE)
			{
				if (bEscapeKeyIsPressed == TRUE)
					bDone = TRUE;

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
			mouse(SDUT_LEFT_BUTTON, SDUT_DOWN, LOWORD(lParam),
				HIWORD(lParam));
			break;

		case WM_LBUTTONUP:
			mouse(SDUT_LEFT_BUTTON, SDUT_UP, LOWORD(lParam),
				HIWORD(lParam));
			break;

		case WM_RBUTTONDOWN:
			mouse(SDUT_RIGHT_BUTTON, SDUT_DOWN, LOWORD(lParam),
				HIWORD(lParam));
			break;

		case WM_RBUTTONUP:
			mouse(SDUT_RIGHT_BUTTON, SDUT_UP, LOWORD(lParam),
				HIWORD(lParam));
			break;

		case WM_MBUTTONDOWN:
			mouse(SDUT_MIDDLE_BUTTON, SDUT_DOWN, LOWORD(lParam),
				HIWORD(lParam));
			break;

		case WM_MBUTTONUP:
			mouse(SDUT_MIDDLE_BUTTON, SDUT_UP, LOWORD(lParam),
				HIWORD(lParam));
			break;

		case WM_MOUSEMOVE:
			mouse(SDUT_MOUSE, SDUT_MOVE, LOWORD(lParam),
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
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

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

GLdouble zFar = 100.0;

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

vec3 eye(-2.506f, 0.688f, 2.515f);
vec3 lookdir(0.705f, -0.17f, -0.688f);
vec3 up(0.0f, 1.0f, 0.0f);
GLfloat angCube = 0.0f;

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

	// code
	if (bIsKeydown == FALSE)
	{
		bIsKeydown = TRUE;
		keyhandler = KeyHandler(key, x, y);
	}
	switch (key)
	{
		/*}*/
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
			freeLook(SDUT_RIGHT_BUTTON, SDUT_UP,
				x, y);
			bFreeLook = TRUE;
		}
		break;

	default:
		break;
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
		freeLook(SDUT_RIGHT_BUTTON, SDUT_DOWN,
			x, y);
	}
}

GLfloat currtime = 0.0f;
GLfloat camSinAng = 0.0f;

void update(void)
{
	// code
	currtime = sdutGet(SDUT_CURRENT_TIME)
		/ 1000.0f;

	if (camSinAng >= 6.28f)
		camSinAng = 0.0f;

	if (bIsKeydown)
		keyhandler.keyHandler();
}

void uninitialize(void)
{
	// function prototypes
	void uninitALAud(void);
	void uninitImGui(void);

	// code
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
	// function prototypes
	void person(void);

	// variables
	GLfloat camspeed = 0.05f;

	// code
	switch (key)
	{
	case 'W':
	case 'w':
		eye += lookdir.unitv() * camspeed;
		person();
		break;

	case 'S':
	case 's':
		eye -= lookdir.unitv() * camspeed;
		person();
		break;

	case 'A':
	case 'a':
		eye -= vec3(lookdir * up).unitv() *
			camspeed;
		person();
		break;

	case 'D':
	case 'd':
		eye += vec3(lookdir * up).unitv() *
			camspeed;
		person();
		break;

	default:
		break;
	}
}

void scene(void)
{
	// function prototypes
	void grid(void);
	void ImGuiUpdate(void);

	// code
	grid();
	ImGuiUpdate();
}

void quad(Color& col)
{
	glColor3fv(col.vect());
	glBegin(GL_QUADS);
	glVertex3f(1.0f, 0.0f, -1.0f);
	glVertex3f(-1.0f, 0.0f, -1.0f);
	glVertex3f(-1.0f, 0.0f, 1.0f);
	glVertex3f(1.0f, 0.0f, 1.0f);
	glEnd();
}

void cube(Color& col)
{
	// function prototypes
	void quad(Point, Point, Point, Point);

	// code
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.25f);

	glColor3fv(Color(col - 0.05f).vect());
	quad(Point(0.25f, 0.25f, 0.0f),
		Point(-0.25f, 0.25f, 0.0f),
		Point(-0.25f, -0.25f, 0.0f),
		Point(0.25f, -0.25f, 0.0f));

	glColor3fv(col.vect());
	quad(Point(0.25f, 0.25f, -0.5f),
		Point(-0.25f, 0.25f, -0.5f),
		Point(-0.25f, -0.25f, -0.5f),
		Point(0.25f, -0.25f, -0.5f));

	glColor3fv(Color(col + 0.05f).vect());
	quad(Point(0.25f, 0.25f, 0.0f),
		Point(0.25f, 0.25f, -0.5f),
		Point(-0.25f, 0.25f, -0.5f),
		Point(-0.25f, 0.25f, 0.0f));

	glColor3fv(Color(col - 0.03f).vect());
	quad(Point(-0.25f, 0.25f, 0.0f),
		Point(-0.25f, 0.25f, -0.5f),
		Point(-0.25f, -0.25f, -0.5f),
		Point(-0.25f, -0.25f, 0.0f));

	glColor3fv(Color(col + 0.03f).vect());
	quad(Point(-0.25f, -0.25f, 0.0f),
		Point(-0.25f, -0.25f, -0.5f),
		Point(0.25f, -0.25f, -0.5f),
		Point(0.25f, -0.25f, 0.0f));

	glColor3fv(Color(col - 0.01f).vect());
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

void person(void)
{
	// code
	if (bFreeLook == FALSE)
	{
		if (bFootstepsAud == FALSE)
		{
			alSourcePlay(alSource[0]);
			bFootstepsAud = TRUE;
		}
		eye.y = 0.5 + (0.1f
			* sinf(camSinAng));
		camSinAng += 0.1f;
	}
}

void freeLook(int button, int state, int x, int y)
{
	// variables
	POINT cursorPos;
	RECT rect;
	const GLfloat sensitivity = 0.1f;
	static GLfloat yaw_rad = 0.0f;
	static GLfloat pitch_rad = 0.0f;
	static BOOL bCamSnapped = FALSE;
	static int prevOGLx = 0;
	static int prevOGLy = 0;

	// code
	switch (button)
	{
	case SDUT_RIGHT_BUTTON:
		if (state == SDUT_DOWN)
		{
			if (bFreeLook)
			{
				prevOGLx = x;
				prevOGLy = y;
			}
			bRmbDown = TRUE;
		}
		else
		{
			bRmbDown = FALSE;
		}
		break;

	case SDUT_MOUSE:
		if (bRmbDown && (state == SDUT_MOVE))
		{
			yaw_rad += radians((x - prevOGLx)
				* sensitivity);
			pitch_rad += radians((prevOGLy - y)
				* sensitivity);

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

			GetCursorPos(&cursorPos);
			ScreenToClient(ghwnd, &cursorPos);
			GetClientRect(ghwnd, &rect);

			if (cursorPos.x >= rect.right - 1)
			{
				cursorPos.x = 0;
				bCamSnapped = TRUE;
			}
			else if (cursorPos.x <= rect.left)
			{
				cursorPos.x = rect.right - 1;
				bCamSnapped = TRUE;
			}

			prevOGLx = cursorPos.x;
			prevOGLy = cursorPos.y;

			if (bCamSnapped)
			{
				bCamSnapped = FALSE;
				ClientToScreen(ghwnd, &cursorPos);
				SetCursorPos(cursorPos.x, cursorPos.y);
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
	for (int x = -winwidth / 2;
		x <= winwidth / 2; ++x)
	{
		glVertex3f((GLfloat)x,
			0.0f, (GLfloat)zFar / 2.0f);
		glVertex3f((GLfloat)x,
			0.0f, -(GLfloat)zFar / 2.0f);
		for (int z = -zFar / 2.0f;
			z <= zFar / 2.0f; ++z)
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
	ALuint alBuff[] = { 0 };
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
	alutLoadWAVFile("Vishwam/rsc/OutDoorFootsteps.wav", &alFormat,
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

	// code
	alcMakeContextCurrent(NULL);
	alcDestroyContext(alContext);
	alcCloseDevice(alDev);

	alContext = NULL;
	alDev = NULL;
}

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
}

void ImGuiFrame(void)
{
	// code
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

BOOL bImGuiExport = FALSE;
BOOL bImGuiNewCube = FALSE;

vector<Object*> objects;
vector<string> objlist;
int objIdx = -1;

void ImGuiUI(void)
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
	if (ImGui::Button("Undo"))
	{
		if (objIdx > -1)
			--objIdx;
		if (objects.size() > 0)
			objects.pop_back();
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
			vec3 objloc(objects[objIdx]->posX,
				objects[objIdx]->posY,
				objects[objIdx]->posZ);
			eye = objloc - vec3(1.5f, -0.5f, -1.5f);
			lookdir = objloc - eye;
		}
	}
	ImGui::EndChild();

	if (ImGui::Button("Quad"))
	{
		objects.push_back(new Quad());
		objlist.push_back(to_string(objlist.size())
			+ ") Quad");
	}
	if (ImGui::Button("Cube"))
	{
		objects.push_back(new Cube());
		objlist.push_back(to_string(objlist.size())
			+ ") Cube");
	}
	
	if (objIdx >= 0)
	{
		Object* obj = objects[objIdx];

		ImGui::Text("Color:");
		ImGui::SliderFloat("R", &obj->color.x, 0.0f, 1.0f);
		ImGui::SliderFloat("G", &obj->color.y, 0.0f, 1.0f);
		ImGui::SliderFloat("B", &obj->color.z, 0.0f, 1.0f);

		ImGui::Text("Translate:");
		ImGui::SliderFloat("X1", &obj->posX, -100.0f, 100.0f);
		ImGui::SliderFloat("Y1", &obj->posY, -100.0f, 100.0f);
		ImGui::SliderFloat("Z1", &obj->posZ, -100.0f, 100.0f);

		ImGui::Text("Rotate:");
		ImGui::SliderFloat("ang", &obj->ang, 0.0f, 360.0f);
		ImGui::SliderFloat("X axis", &obj->axisX, 0.0f, 1.0f);
		ImGui::SliderFloat("Y axis", &obj->axisY, 0.0f, 1.0f);
		ImGui::SliderFloat("Z axis", &obj->axisZ, 0.0f, 1.0f);

		ImGui::Text("Scale:");
		ImGui::SliderFloat("X2", &obj->sizeX, -100.0f, 100.0f);
		ImGui::SliderFloat("Y2", &obj->sizeY, -100.0f, 100.0f);
		ImGui::SliderFloat("Z2", &obj->sizeZ, -100.0f, 100.0f);
	}

	// end imgui UI
	ImGui::End();
}

void ImGuiRender(void)
{
	// code
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(
		ImGui::GetDrawData());
}

void ImGuiUpdate(void)
{
	// variables
	FILE* f_imgui = NULL;

	// code
	Object::drawObjects(objects);
	Object::checkCollisions();

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
			for (int i = 0; i < objects.size(); ++i)
			{
				Object* obj = objects[i];

				fprintf(f_imgui, "\nglPushMatrix();\n"
					"glColor3f(%.3ff, %.3ff, %.3ff);\n"
					"glTranslatef(%.3ff, %.3ff, %.3ff);\n"
					"glRotatef(%.3ff, %.3ff, %.3ff, %.3ff);\n"
					"glScalef(%.3ff, %.3ff, %.3ff);\n"
					"%s;\n"
					"glPopMatrix();\n", obj->color.x,
					obj->color.y, obj->color.z, obj->posX,
					obj->posY, obj->posZ, obj->ang,
					obj->axisX, obj->axisY,obj->axisZ,
					obj->sizeX, obj->sizeY, obj->sizeZ,
					objlist[i].c_str());
				bImGuiExport = FALSE;
			}
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
