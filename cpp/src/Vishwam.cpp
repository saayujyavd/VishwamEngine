/*
* Title : Vishwam Core
* Date : 13 - 09 -2026
* Author : Saayujya Deshpande
* Technology Used : Win32 SDK
* Programming Language : C++ Language
* References:
	1. Gokhale sir's live code
*/

#include "../include/alut.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imfilebrowser.h"

#include "../include/Vishwam.h"
#include "../include/OGL.h"
#include "../include/Texture.h"
#include "../include/Light.h"

// link with OGL import library
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "lib/openal32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "lib/alut.lib")

// macros
#define WIN_WIDTH  800
#define WIN_HEIGHT 600

#define radians(ang) ang * 3.141592f / 180.0f

// global function prototypes
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
extern IMGUI_IMPL_API LRESULT
ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);
GLfloat randFloat(GLfloat, GLfloat);

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
GLfloat texCoords[] = { 0.0f, 0.0f, 0.0f,
						1.0f, 0.0f, 0.0f,
						1.0f, 1.0f, 0.0f,
						0.0f, 1.0f, 0.0f };
GLfloat quadCoords[] = { 0.0f, 0.0f, 0.0f,
						1.0f, 0.0f, 0.0f,
						1.0f, 1.0f, 0.0f,
						0.0f, 1.0f, 0.0f };

// class definitions
// pause menu (toggled with ESC)
class PauseMenu
{
public:
	enum Choice { RESUME = 1, QUIT = 2 };

private:
	// variables
	inline static int choice = RESUME;
	inline static BOOL bChoiceLocked = FALSE;
	inline static BOOL bLoaded = FALSE;
	inline static array<GLuint, 3> textures = { 0, 0, 0 };
	inline static const char* const files[3] = {
		"rsc/GamePaused.jpg",
		"rsc/ResumeGame.jpg",
		"rsc/QuitGame.jpg" };

	// function definitions
	static void quad2D(void)
	{
		glBegin(GL_QUADS);
		glVertex3f(-1.0f, -1.0f, 0.0f);
		glVertex3f(1.0f, -1.0f, 0.0f);
		glVertex3f(1.0f, 1.0f, 0.0f);
		glVertex3f(-1.0f, 1.0f, 0.0f);
		glEnd();
	}

	// load the textures only once
	static void load(void)
	{
		if (bLoaded)
			return;

		for (size_t i = 0; i < textures.size(); ++i)
			textures[i] = initTex(files[i]);
		bLoaded = TRUE;
	}

public:
	static void keyHandler(unsigned char key)
	{
		switch (key)
		{
		case VK_UP:
			if (choice > RESUME)
				--choice;
			break;

		case VK_DOWN:
			if (choice < QUIT)
				++choice;
			break;

		case VK_RETURN:
			bChoiceLocked = TRUE;
			break;

		default:
			break;
		}
	}

	static void update(void)
	{
		if (!bChoiceLocked)
			return;

		switch (choice)
		{
		case RESUME:
			bEscapeKeyIsPressed = FALSE;
			break;

		case QUIT:
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

	static void render(void)
	{
		// code
		load();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// switch to 2D projection for menu
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glDisable(GL_DEPTH_TEST);

		// background
		glColor3f(0.0f, 0.0f, 0.0f);
		quad2D();

		// title
		glPushMatrix();
		glScalef(0.8f, 0.7f, 0.0f);
		glTranslatef(-0.5f, 0.7f, 0.0f);
		texQuad2D(textures[0], texCoords, quadCoords);
		glPopMatrix();

		// resume
		glPushMatrix();
		glScalef(0.5f, 0.1f, 0.0f);
		glTranslatef(-0.5f, 0.0f, 0.0f);
		texQuad2D(textures[1], texCoords, quadCoords);
		glPopMatrix();

		// quit
		glPushMatrix();
		glScalef(0.3f, 0.1f, 0.0f);
		glTranslatef(-0.5f, -1.2f, 0.0f);
		texQuad2D(textures[2], texCoords, quadCoords);
		glPopMatrix();

		// selection quad
		glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
		glPushMatrix();
		glScalef(0.3f, 0.1f, 0.0f);
		glTranslatef(0.0f, choice == RESUME ? 1.0f : -1.0f, 0.0f);
		quad2D();
		glPopMatrix();

		glEnable(GL_DEPTH_TEST);

		// get back to 3D state
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);

		SwapBuffers(ghdc);
	}

	static void release(void)
	{
		if (!bLoaded)
			return;

		glDeleteTextures((GLsizei)textures.size(),
			textures.data());
		textures.fill(0);
		bLoaded = FALSE;
	}
};

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
		uninitialize();

		hwnd = NULL;
		return(iResult);
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
				PauseMenu::update();
				PauseMenu::render();
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
	void mouse(UINT, int, int);
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
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MOUSEMOVE:
			mouse(iMsg, LOWORD(lParam), HIWORD(lParam));
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


// global function prototypes
void cube(GLfloat, GLfloat, GLfloat, GLfloat);
void quad(Point, Point, Point, Point);
void quad(Point, Point, Point, Point,
	Point, Point, Point, Point,
	Color, Color, Color, Color);

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

Point::Point() {}
Point::Point(GLfloat x, GLfloat y,
	GLfloat z) : x(x), y(y), z(z) {}

Point& Point::operator+=(Point& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return(*this);
}
Point& Point::operator-=(Point& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return(*this);
}
Point Point::operator+(Point& v)
{
	return(Point(x + v.x,
		y + v.y, z + v.z));
}
Point Point::operator+(GLfloat f)
{
	return(Point(x + f,
		y + f, z + f));
}
Point Point::operator-(Point& v)
{
	return(Point(x - v.x,
		y - v.y, z - v.z));
}
Point Point::operator-(GLfloat f)
{
	return(Point(x - f,
		y - f, z - f));
}
Point Point::operator*(Point& v)
{
	return(Point(y * v.z - z * v.y,
		z * v.x - x * v.z, x * v.y
		- y * v.x));
}
Point Point::operator*(GLfloat f)
{
	return(Point(x * f, y * f, z * f));
}
GLfloat Point::magnitude(void)
{
	return(sqrtf(x * x + y * y +
		z * z));
}
GLfloat Point::dot(Point& v)
{
	return(x * v.x + y * v.y + z * v.z);
}
array<GLfloat, 3> Point::vect(void)
{
	array<GLfloat, 3> arr
		= { x, y, z };
	return(arr);
}
Point Point::unitv(void)
{
	GLfloat mag = magnitude();
	mag = mag <= 0.00001f ?
		0.1f : mag;
	return(Point(x / mag,
		y / mag, z / mag));
}
Point Point::randP(GLfloat min, GLfloat max)
{
	return(Point(randFloat(min, max),
		randFloat(min, max),
		randFloat(min, max)));
}

bool Vishwam::handleCollision(Vishwam& obj)
{
	Vishwam* objA = this;
	Vishwam* objB = &obj;
	Vishwam::Property prop = Vishwam::NO_COLLISION;

	if (objA->property == Vishwam::HANDLE_COLLISION &&
		objB->property == Vishwam::HANDLE_COLLISION)
	{
		prop = Vishwam::HANDLE_COLLISION;
	}
	else if ((objA->property == Vishwam::TEST_COLLISION
		&& objB->property == Vishwam::HANDLE_COLLISION)
		|| (objB->property == Vishwam::TEST_COLLISION
			&& objA->property == Vishwam::HANDLE_COLLISION))
	{
		prop = Vishwam::TEST_COLLISION;
	}
	else if (objA->property == Vishwam::TEST_COLLISION
		&& objB->property == Vishwam::TEST_COLLISION)
	{
		prop = Vishwam::TEST_COLLISION;
	}
	else
	{
		return(false);
	}

	GLfloat leftA = objA->posX - objA->sizeX / 4.0f;
	GLfloat rightA = objA->posX + objA->sizeX / 4.0f;
	GLfloat bottomA = objA->posY - objA->sizeY / 4.0f;
	GLfloat topA = objA->posY + objA->sizeY / 4.0f;
	GLfloat rearA = objA->posZ - objA->sizeZ / 4.0f;
	GLfloat frontA = objA->posZ + objA->sizeZ / 4.0f;

	GLfloat leftB = objB->posX - objB->sizeX / 4.0f;
	GLfloat rightB = objB->posX + objB->sizeX / 4.0f;
	GLfloat bottomB = objB->posY - objB->sizeY / 4.0f;
	GLfloat topB = objB->posY + objB->sizeY / 4.0f;
	GLfloat rearB = objB->posZ - objB->sizeZ / 4.0f;
	GLfloat frontB = objB->posZ + objB->sizeZ / 4.0f;

	if (objB->bIsBounding)
	{
		GLfloat pushX = 0, pushZ = 0;

		if (leftA < leftB)
			pushX = leftB - leftA;
		else if (rightA > rightB)
			pushX = rightB - rightA;

		if (rearA < rearB)
			pushZ = rearB - rearA;
		else if (frontA > frontB)
			pushZ = frontB - frontA;

		if (pushX == 0 && pushZ == 0)
			return false;

		if (prop == Vishwam::TEST_COLLISION)
			return true;

		objA->posX += pushX;
		objA->posZ += pushZ;
		return(true);
	}
	else
	{
		if ((leftA < rightB && rightA > leftB) &&
			(bottomA < topB && topA > bottomB) &&
			(rearA < frontB && frontA > rearB))
		{
			if (prop == Vishwam::TEST_COLLISION)
				return(true);

			GLfloat overlapX = fmin(rightA, rightB)
				- fmax(leftA, leftB);
			GLfloat overlapY = fmin(topA, topB)
				- fmax(bottomA, bottomB);
			GLfloat overlapZ = fmin(frontA, frontB)
				- fmax(rearA, rearB);

			if (overlapX <= overlapY &&
				overlapX <= overlapZ)
			{
				if (objA->posX >= objB->posX)
					objA->posX += overlapX;
				else
					objA->posX -= overlapX;
			}
			else if (overlapY <= overlapX
				&& overlapY <= overlapZ)
			{
				if (objA->posY >= objB->posY)
					objA->posY += overlapY;
				else
					objA->posY -= overlapY;
			}
			else
			{
				if (objA->posZ >= objB->posZ)
					objA->posZ += overlapZ;
				else
					objA->posZ -= overlapZ;
			}
			return(true);
		}
	}
	return(false);
}

void Vishwam::updateObject(void) {}

Vishwam::Vishwam(Property prop) : property(prop)
{
	if (property == TEST_COLLISION ||
		property == HANDLE_COLLISION)
	{
		left = posX - sizeX / 4.0f;
		right = posX + sizeX / 4.0f;
		bottom = posY - sizeY / 4.0f;
		top = posY + sizeY / 4.0f;
		rear = posZ - sizeZ / 4.0f;
		front = posZ + sizeZ / 4.0f;
	}
}

Vishwam::Vishwam(Vishwam& copy)
{
	bIsBounding = copy.bIsBounding;
	property = copy.property;
	color = copy.color;
	alpha = copy.alpha;
	ang = copy.ang;
	vertices = copy.vertices;
	posX = copy.posX; posY = copy.posY; posZ = copy.posZ;
	sizeX = copy.sizeX; sizeY = copy.sizeY; sizeZ = copy.sizeZ;
	axisX = copy.axisX; axisY = copy.axisY; axisZ = copy.axisZ;
	left = copy.left; right = copy.right;
	bottom = copy.bottom; top = copy.top;
	rear = copy.rear; front = copy.front;
}

void Vishwam::drawObjects(vector<Vishwam*>& objects)
{
	objs = &objects;
	for (int i = 0; i < objects.size(); ++i)
	{
		objects[i]->draw();
		objects[i]->updateObject();
	}
}

void Vishwam::handleCollisionsOf(Vishwam& object)
{
	if (objs == nullptr)
		return;

	object.draw();
	for (int i = 0; i < objs->size(); ++i)
		object.handleCollision(*(*objs)[i]);
	object.updateObject();
}

void Vishwam::popBack(vector<Vishwam*>& objects)
{
	if (objects.size() > 0)
	{
		delete objects.back();
		objects.pop_back();
	}
}

void Vishwam::emptyObjVect(vector<Vishwam*>& objects)
{
	for (int i = 0; i < objects.size(); ++i)
		delete objects[i];
	objects.clear();
}

Vishwam::~Vishwam() {}

vector<Vishwam*> objects;
vector<Vishwam*> childObjs;
vector<string> objlist;
int objIdx = -1;

class Quad : public Vishwam
{
	// function definitions
	void draw(void) override
	{
		glPushMatrix();
		glColor4f(color.x, color.y, color.z, alpha);
		glTranslatef(posX, posY, posZ);
		glRotatef(ang, axisX, axisY, axisZ);
		glScalef(sizeX, sizeY, sizeZ);

		glBegin(GL_QUADS);
		for (int i = 0; i < 4; ++i)
		{
			glVertex3f(vertices[i * 3],
				vertices[i * 3 + 1],
				vertices[i * 3 + 2]);
		}
		glEnd();
		glPopMatrix();
	}

public:
	Quad()
	{
		vertices = { 1.0f, 0.0f, -1.0f,
					-1.0f, 0.0f, -1.0f,
					-1.0f, 0.0f, 1.0f,
					1.0f, 0.0f, 1.0f };
	}

	Vishwam* clone() override
	{
		return(new Quad());
	}
};

class Cube : public Vishwam
{
	// function definitions
	void draw(void) override
	{
		// vertex i as a Point
		auto v = [this](int i)
			{
				return(Point(vertices[i * 3 + 0],
					vertices[i * 3 + 1],
					vertices[i * 3 + 2]));
			};

		GLfloat r = color.x;
		GLfloat g = color.y;
		GLfloat b = color.z;
		GLfloat a = alpha;

		glPushMatrix();
		glTranslatef(posX, posY, posZ);
		glRotatef(ang, axisX, axisY, axisZ);
		glScalef(sizeX, sizeY, sizeZ);
		glColor4f(r, g, b, a);

		// front
		glNormal3f(0.0f, 0.0f, 1.0f);
		quad(v(0), v(1), v(2), v(3));

		// back
		glNormal3f(0.0f, 0.0f, -1.0f);
		quad(v(5), v(6), v(7), v(4));

		// top
		glNormal3f(0.0f, 1.0f, 0.0f);
		quad(v(0), v(5), v(6), v(1));

		// left
		glNormal3f(-1.0f, 0.0f, 0.0f);
		quad(v(1), v(6), v(7), v(2));

		// bottom
		glNormal3f(0.0f, -1.0f, 0.0f);
		quad(v(2), v(7), v(4), v(3));

		// right
		glNormal3f(1.0f, 0.0f, 0.0f);
		quad(v(3), v(4), v(5), v(0));

		glPopMatrix();
	}

public:
	Cube()
	{
		vertices = { 0.25f, 0.25f, 0.25f,
					-0.25f, 0.25f, 0.25f,
					-0.25f, -0.25f, 0.25f,
					0.25f, -0.25f, 0.25f,
					0.25f, -0.25f, -0.25f,
					0.25f, 0.25f, -0.25f,
					-0.25f, 0.25f, -0.25f,
					-0.25f, -0.25f, -0.25f };
	}

	Vishwam* clone() override
	{
		return(new Cube());
	}
};

class Cylinder : public Vishwam
{
	GLUquadric* quadric = nullptr;
public:
	GLdouble baseRad = 0.5, topRad = 0.5;

private:
	void draw(void) override
	{
		glPushMatrix();
		glColor4f(color.x, color.y, color.z, alpha);
		glTranslatef(posX, posY, posZ);
		glRotatef(ang, axisX, axisY, axisZ);
		glRotatef(270.0f, 1.0, 0.0f, 0.0f);
		glScalef(sizeX, sizeY, sizeZ);
		gluCylinder(quadric, baseRad, topRad, 1.0, 20, 1);
		glPopMatrix();
	}

public:
	Cylinder()
	{
		quadric = gluNewQuadric();
	}

	Vishwam* clone() override
	{
		return(new Cylinder());
	}

	~Cylinder()
	{
		if (quadric != nullptr)
			gluDeleteQuadric(quadric);
	}
};

class Light : public Vishwam
{
public:
	GLfloat pos[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat col[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat shininess = 20.0f, specular = 0.5f;
	GLfloat ambient = 0.3f;

private:
	void draw(void) override
	{
		pos[0] = posX; pos[1] = posY; pos[2] = posZ;
		col[0] = color.x; col[1] = color.y;
		col[2] = color.z; col[3] = alpha;
		light(pos, col, shininess, specular, ambient);
	}

public:
	Light()
	{
		initLight();
	}

	Vishwam* clone() override
	{
		return(new Light());
	}

	~Light()
	{
		uninitLight();
	}
};

class TexQuad2D : public Vishwam
{
	vector<char> name;
	GLuint texture = 0;
	GLfloat texCoords[12];
	GLfloat quadCoords[12];

	void draw(void) override
	{
		glPushMatrix();
		glTranslatef(posX, posY, posZ);
		glRotatef(ang, axisX, axisY, axisZ);
		glScalef(sizeX, sizeY, sizeZ);
		texQuad2D(texture, texCoords, vertices.data());
		glPopMatrix();
	}

public:
	TexQuad2D(const char* filename)
	{
		memcpy(texCoords, this->texCoords, 12 * sizeof(GLfloat));
		memcpy(quadCoords, this->quadCoords, 12 * sizeof(GLfloat));

		vertices.assign(quadCoords, quadCoords + 12);
		name.assign(filename, filename + strlen(filename) + 1);
		texture = initTex(name.data());
	}

	Vishwam* clone() override
	{
		return(new TexQuad2D(name.data()));
	}

	~TexQuad2D()
	{
		if (texture != 0)
			glDeleteTextures(1, &texture);
	}
};

BOOL bFootstepsAud = FALSE;
BOOL bIsKeydown = FALSE;
BOOL bFreeLook = TRUE;
BOOL bRmbDown = FALSE;
BOOL bPlayerKeyDown = FALSE;
BOOL bMouseMove = TRUE;
BOOL bHandleKeys = TRUE;

GLfloat currtime = 0.0f;
GLfloat camSinAng = 0.0f;
GLfloat playerHeight = 0.5f;

vec3 eye(-2.506f, 0.688f, 2.515f);
vec3 lookdir(0.705f, -0.17f, -0.688f);
vec3 up(0.0f, 1.0f, 0.0f);

KeyHandler keyhandler;
ALuint alSource[] = { 0 };
ALuint alBuff[] = { 0 };

class Player : public Vishwam
{
	void draw(void) override
	{
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
			posX = eye.x;
			posY = playerHeight;
			posZ = eye.z;
		}
	}

	void updateObject(void) override
	{
		if (bFreeLook == FALSE)
		{
			eye.x = posX;
			eye.z = posZ;
		}
	}

public:
	Vishwam* clone() override
	{
		return(new Player());
	}
};

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
	freopen_s(&fin, "CONIN$", "r", stdin);
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
		ErrorLog("initialize(): GetDC() failed", EL_FILE);
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

	// call register objects
	registerObjects();

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

// GLfloat angCube = 0.0f;

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
	void freeLook(UINT, int, int);

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
			freeLook(WM_RBUTTONUP, x, y);
			bFreeLook = TRUE;
		}
		break;

	default:
		break;
	}

	if (bEscapeKeyIsPressed)
		PauseMenu::keyHandler(key);
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
		if (bPlayerKeyDown)
			bPlayerKeyDown = FALSE;
		break;

	default:
		break;
	}
}

void mouse(UINT msg, int x, int y)
{
	// function prototypes
	void freeLook(UINT, int, int);

	// code
	freeLook(msg, x, y);
	if (bFreeLook == FALSE)
	{
		// deliberately keep RMB state
		// always down
		freeLook(WM_RBUTTONDOWN, x, y);
	}
}

void handleKeysAndMouse(BOOL flag)
{
	bMouseMove = flag;
	bHandleKeys = flag;
}

void update(void)
{
	// function prototypes
	void ImGuiUpdate(void);

	// variables
	static Player player;
	float time_in_milliseconds = (float)clock();

	// code
	currtime = (GLfloat)time_in_milliseconds / 1000.0f;

	if (camSinAng >= 6.28f)
		camSinAng = 0.0f;

	if (bIsKeydown)
		keyhandler.keyHandler();

	Vishwam::handleCollisionsOf(player);
	ImGuiUpdate();
}

void uninitialize(void)
{
	// function prototypes
	void uninitALAud(void);
	void uninitImGui(void);

	// code
	// delete the object's vector
	Vishwam::emptyObjVect(objects);

	// free the pause menu textures
	PauseMenu::release();

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

	// code
	Vishwam::drawObjects(objects);
	grid();
}

GLfloat randFloat(GLfloat min, GLfloat max)
{
	GLfloat scale = (GLfloat)rand() / (GLfloat)RAND_MAX;
	return(min + scale * (max - min));
}

void quad(Color& col)
{
	glColor3fv(col.vect().data());
	glBegin(GL_QUADS);
	glVertex3f(1.0f, 0.0f, -1.0f);
	glVertex3f(-1.0f, 0.0f, -1.0f);
	glVertex3f(-1.0f, 0.0f, 1.0f);
	glVertex3f(1.0f, 0.0f, 1.0f);
	glEnd();
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
	glBegin(GL_TRIANGLES);
	glVertex3f(a.x, a.y, a.z);
	glVertex3f(b.x, b.y, b.z);
	glVertex3f(c.x, c.y, c.z);
	glVertex3f(a.x, a.y, a.z);
	glVertex3f(d.x, d.y, d.z);
	glVertex3f(c.x, c.y, c.z);
	glEnd();
}

void newCollisionCube(vec3 pos, vec3 size,
	GLfloat ang, vec3 axis)
{
	// variables
	Cube* cube = new Cube();

	// code
	cube->ang = ang;
	cube->alpha = 0.0f;
	cube->axisX = axis.x;
	cube->axisY = axis.y;
	cube->axisZ = axis.z;
	cube->posX = pos.x;
	cube->posY = pos.y;
	cube->posZ = pos.z;
	cube->sizeX = size.x;
	cube->sizeY = size.y;
	cube->sizeZ = size.z;

	objects.push_back(cube);
}

void freeLook(UINT msg, int x, int y)
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
	switch (msg)
	{
	case WM_RBUTTONDOWN:
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
	break;

	case WM_RBUTTONUP:
	{
		// stop bRmbDown from getting stuck
		ReleaseCapture();
		bRmbDown = FALSE;
	}
	break;

	case WM_MOUSEMOVE:
		if (bRmbDown && bMouseMove == TRUE)
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
	for (int x = -30; x <= 30; ++x)
	{
		glVertex3f((GLfloat)x,
			0.0f, 30.0f);
		glVertex3f((GLfloat)x,
			0.0f, -30.0f);
		for (int z = -30; z <= 30; ++z)
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
	if (alContext == NULL)
		return;

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
BOOL bImGuiNewCube = FALSE;
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

// grouped Translate / Rotate / Scale controls, shared by
// the selected object and the imguiDraw() test transform
void transformControls(GLfloat& px, GLfloat& py, GLfloat& pz,
	GLfloat& a, GLfloat& ax, GLfloat& ay, GLfloat& az,
	GLfloat& sx, GLfloat& sy, GLfloat& sz)
{
	if (ImGui::CollapsingHeader("Translate:"))
	{
		ImGui::PushID("translate");
		ImGui::SliderFloat("X", &px, -100.0f, 100.0f);
		ImGui::SliderFloat("Y", &py, -100.0f, 100.0f);
		ImGui::SliderFloat("Z", &pz, -100.0f, 100.0f);
		ImGui::PopID();
	}

	if (ImGui::CollapsingHeader("Rotate:"))
	{
		ImGui::PushID("rotate");
		ImGui::SliderFloat("ang", &a, 0.0f, 360.0f);
		ImGui::SliderFloat("X axis", &ax, 0.0f, 1.0f);
		ImGui::SliderFloat("Y axis", &ay, 0.0f, 1.0f);
		ImGui::SliderFloat("Z axis", &az, 0.0f, 1.0f);
		ImGui::PopID();
	}

	if (ImGui::CollapsingHeader("Scale:"))
	{
		ImGui::PushID("scale");
		ImGui::SliderFloat("X", &sx, -100.0f, 100.0f);
		ImGui::SliderFloat("Y", &sy, -100.0f, 100.0f);
		ImGui::SliderFloat("Z", &sz, -100.0f, 100.0f);
		ImGui::PopID();
	}
}

void ImGuiUI(void)
{
	// variables
	BOOL bObjSelected = FALSE;

	// code
	ImGui::Begin("ImGui Wnd");

	// define UI below
	ImGui::Text("FPS: %.1f",
		ImGui::GetIO().Framerate);

	if (ImGui::Button("Export code"))
		bImGuiExport = TRUE;
	ImGui::SameLine(0.0f, 20.0f);
	if (ImGui::Button("Remove latest"))
	{
		if (objIdx > -1)
			--objIdx;
		if (objects.size() > 0)
			Vishwam::popBack(objects);
		if (objlist.size() > 0)
			objlist.pop_back();
	}
	if (ImGui::Button("Make copy"))
	{
		if (objIdx >= 0)
		{
			Vishwam* obj = objects[objIdx];

			if (Quad* quad = dynamic_cast<Quad*>(obj))
			{
				Quad* cpy = new Quad(*quad);
				objects.push_back(cpy);
				objlist.push_back(to_string(objlist.size())
					+ ") Quad");
			}
			else if (Cube* cube = dynamic_cast<Cube*>(obj))
			{
				Cube* cpy = new Cube(*cube);
				objects.push_back(cpy);
				objlist.push_back(to_string(objlist.size())
					+ ") Cube");
			}
			else if (Cylinder* cyl = dynamic_cast<Cylinder*>(obj))
			{
				Cylinder* cpy = new Cylinder(*cyl);
				objects.push_back(cpy);
				objlist.push_back(to_string(objlist.size())
					+ ") Cylinder");
			}
		}
	}
	ImGui::SameLine(0.0f, 34.0f);

	if (ImGui::Button("Goto selected"))
	{
		if (objIdx >= 0)
		{
			vec3 objloc(objects[objIdx]->posX,
				objects[objIdx]->posY,
				objects[objIdx]->posZ);
			eye = objloc - vec3(1.5f, -0.5f, -1.5f);
			lookdir = objloc - eye;
		}
	}

	ImGui::BeginChild("Objects", ImVec2(0, 200),
		true);
	for (int i = 0; i < objlist.size(); ++i)
	{
		if (ImGui::Selectable(objlist[i].c_str(),
			objIdx == i)) objIdx = i;
	}
	ImGui::EndChild();

	if (ImGui::Button("Quad"))
	{
		objects.push_back(new Quad());
		objlist.push_back(to_string(objlist.size())
			+ ") Quad");
	}
	ImGui::SameLine(0.0f, 48.0f);
	if (ImGui::Button("Cube"))
	{
		objects.push_back(new Cube());
		objlist.push_back(to_string(objlist.size())
			+ ") Cube");
	}
	if (ImGui::Button("Cylinder"))
	{
		objects.push_back(new Cylinder());
		objlist.push_back(to_string(objlist.size())
			+ ") Cylinder");
	}
	ImGui::SameLine(0.0f, 20.0f);
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

		objects.push_back(new TexQuad2D(
			filename.c_str()));
		fileDialog.ClearSelected();
		objlist.push_back(to_string(objlist.size())
			+ ") Texture quad");
	}
	if (ImGui::Button("Light"))
	{
		objects.push_back(new Light());
		objlist.push_back(to_string(objlist.size())
			+ ") Light source");
	}
	for (int i = 0; i < childObjs.size(); ++i)
	{
		if (childObjs[i]->className.empty() == false)
		{
			if (ImGui::Button(childObjs[i]->className.c_str()))
			{
				objects.push_back(childObjs[i]->clone());
				objlist.push_back(to_string(objlist.size())
					+ ") " + childObjs[i]->className);
			}
		}
	}
	if (objIdx >= 0 && objIdx < (int)objects.size())
	{
		Vishwam* obj = objects[objIdx];

		ImGui::Text("Color:");
		float col[4] = { obj->color.x, obj->color.y,
			obj->color.z, obj->alpha };
		if (ImGui::ColorPicker4("##picker", col))
		{
			obj->color = Color(col[0], col[1], col[2]);
			obj->alpha = col[3];
		}

		// per-vertex editing (Quad, Cube)
		for (size_t i = 0; i + 2 < obj->vertices.size(); i += 3)
		{
			string label = "Vertex: " + to_string(i / 3);

			ImGui::PushID((int)i);
			if (ImGui::CollapsingHeader(label.c_str()))
			{
				ImGui::SliderFloat("X", &obj->vertices[i + 0], -100.0f, 100.0f);
				ImGui::SliderFloat("Y", &obj->vertices[i + 1], -100.0f, 100.0f);
				ImGui::SliderFloat("Z", &obj->vertices[i + 2], -100.0f, 100.0f);
			}
			ImGui::PopID();
		}

		transformControls(obj->posX, obj->posY, obj->posZ,
			obj->ang, obj->axisX, obj->axisY, obj->axisZ,
			obj->sizeX, obj->sizeY, obj->sizeZ);

		// check if obj is cylinder at runtime
		Cylinder* cylinder = nullptr;
		if (((cylinder = dynamic_cast<Cylinder*>(obj))
			!= nullptr) && ImGui::CollapsingHeader("Radius:"))
		{
			float base = (float)cylinder->baseRad;
			float top = (float)cylinder->topRad;

			ImGui::PushID("radius");
			if (ImGui::SliderFloat("Base", &base, 0.0f, 100.0f)) cylinder->baseRad = base;
			if (ImGui::SliderFloat("Top", &top, 0.0f, 100.0f)) cylinder->topRad = top;
			ImGui::PopID();
		}

		// check if obj is Light at runtime
		Light* light = nullptr;
		if (((light = dynamic_cast<Light*>(obj))
			!= nullptr) && ImGui::CollapsingHeader("Properties:"))
		{
			float shininess = (float)light->shininess;
			float specular = (float)light->specular;
			float ambient = (float)light->ambient;

			ImGui::PushID("Properties");
			if (ImGui::SliderFloat("Shininess", &shininess, 0.0f, 100.0f))
				light->shininess = shininess;
			if (ImGui::SliderFloat("Specular", &specular, 0.0f, 1.0f))
				light->specular = specular;
			if (ImGui::SliderFloat("Ambient", &ambient, 0.0f, 1.0f))
				light->ambient = ambient;
			ImGui::PopID();
		}
		bImGuiDraw = FALSE;
	}
	else if (bImGuiDraw)
	{
		transformControls(posX, posY, posZ,
			ang, axisX, axisY, axisZ,
			sizeX, sizeY, sizeZ);
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
			for (int i = 0; i < objects.size(); ++i)
			{
				Vishwam* obj = objects[i];

				fprintf(f_imgui, "\nglPushMatrix();\n"
					"glColor4f(%.3ff, %.3ff, %.3ff, %.3ff);\n"
					"glTranslatef(%.3ff, %.3ff, %.3ff);\n"
					"glRotatef(%.3ff, %.3ff, %.3ff, %.3ff);\n"
					"glScalef(%.3ff, %.3ff, %.3ff);\n"
					"%s(vec3(%.3ff, %.3ff, %.3ff),\n"
					"vec3(%.3ff, %.3ff, %.3ff), %.3ff,\n"
					"vec3(%.3ff, %.3ff, %.3ff));\n"
					"glPopMatrix();\n", obj->color.x,
					obj->color.y, obj->color.z, obj->alpha,
					obj->posX,
					obj->posY, obj->posZ, obj->ang,
					obj->axisX, obj->axisY, obj->axisZ,
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
	if (ImGui::GetCurrentContext()
		== NULL) return;

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
