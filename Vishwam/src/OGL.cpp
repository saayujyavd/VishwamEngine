/*
* Title : Vishwam Engine
* Date : 16 - 09 -2026
* Author : Saayujya Deshpande
* Technology Used : FreeGLUT
* Programming Language : C Language
* References : 
	1. Gokhale sir's live codes
	2. Pradnya maam's C Assignments
*/

#include <cstdio>
#include <cmath>
#include "../include/SDUT.h"

#pragma comment(lib, "./lib/sdut.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#define WIN_WIDTH  800
#define WIN_HEIGHT 600

#define radians(ang) ang * 3.141592f / 180.0f

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
	Point operator-(Point& v)
	{
		return(Point(x - v.x,
			y - v.y, z - v.z));
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

class Sphere
{
	// variables
	GLUquadric* quadric = NULL;
	Color col;
	GLfloat rad;

public:
	// constructor definitions
	Sphere(void) {}
	Sphere(Color& color, GLfloat radius) : col(color),
		rad(radius) {}

	// function definitions
	void draw(void)
	{
		// code
		if (quadric == NULL)
			quadric = gluNewQuadric();
		else
		{
			glColor3fv(col.vect());
			gluSphere(quadric, rad, 360, 360);
		}
	}

	// destructor definitions
	~Sphere(void)
	{
		gluDeleteQuadric(quadric);
		quadric = NULL;
	}
};

// global variables
BOOL bFullscreen = FALSE;
BOOL bIsKeydown = FALSE;
BOOL bRmbDown = FALSE;

KeyHandler keyhandler;
Sphere* sky = NULL;

int winwidth = WIN_WIDTH;
int winheight = WIN_HEIGHT;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpszCmdLine, int iCmdShow)
{
	// function prototypes
	void console(void);
	void initialize(void);
	void uninitialize(void);
	void resize(int, int);
	void render(void);
	void keyboard(unsigned char, int, int);
	void keyUp(unsigned char, int, int);
	void mouse(int, int, int, int);
	void update(void);

	// code
	// first enable the console
	console();

	// then initialize & create the window
	sdutInit(iCmdShow, (char**)lpszCmdLine);
	sdutInitDisplayMode(SDUT_DOUBLE | SDUT_RGBA | SDUT_DEPTH);
	sdutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);
	sdutInitWindowPosition(100, 100);
	sdutCreateWindow("Saayujya Deshpande");
	initialize();

	sdutReshapeFunc(resize);
	sdutRenderFunc(render);
	sdutKeyboardFunc(keyboard);
	sdutKeyUpFunc(keyUp);
	sdutMouseFunc(mouse);
	sdutUpdateFunc(update);
	sdutCloseFunc(uninitialize);
	sdutMainLoop();

	// flow should not come here
	return(0);
}

void initialize(void)
{
	// function prototypes
	void resize(int, int);

	// start rendering API
	glEnable(GL_DEPTH_TEST);

	// choose screen clearing color: blue
	// glClearColor() just chooses color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// warmup resize
	resize(WIN_WIDTH, WIN_HEIGHT);
}

GLdouble zFar = 100.0;

void resize(int width, int height)
{
	// code
	// height must not be 0 if
	// width / height is done
	if (height <= 0)
		height = 1;
	else
	{
		winwidth = width;
		winheight = height;
	}

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

	// variables
	vec3 lookat = lookdir + eye;

	// code
	// clears screen with the color
	// chosen in glClearColor()
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// translate and specify look at
	gluLookAt(eye.x, eye.y, eye.z,
		lookat.x, lookat.y, lookat.z,
		up.x, up.y, up.z);

	printf("\r(eye.x, eye.y, eye.z):"
		" %f, %f, %f\t(lookat.x, lookat.y, "
		"lookat.z): %f, %f, %f", eye.x, eye.y,
		eye.z, lookat.x, lookat.y, lookat.z);

	// draw the scene
	scene();

	// double buffering
	sdutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
	if (bIsKeydown == FALSE)
	{
		bIsKeydown = TRUE;
		keyhandler = KeyHandler(key, x, y);
	}
}

void keyUp(unsigned char key, int x, int y)
{
	if (bIsKeydown == TRUE)
		bIsKeydown = FALSE;

	switch (key)
	{
	case 'F':
	case 'f':
		if (bFullscreen)
		{
			sdutLeaveFullscreen();
			bFullscreen = FALSE;
		}
		else
		{
			sdutFullscreen();
			bFullscreen = TRUE;
		}
		break;

	default:
		break;
	}
}

void mouse(int button, int state, int x, int y)
{
	// variables
	static GLfloat yaw_rad = 0.0f;
	static GLfloat pitch_rad = 0.0f;
	const GLfloat sensitivity = 0.1f;
	static int prevOGLx = 0;
	static int prevOGLy = 0;

	// code
	switch (button)
	{
	case SDUT_RIGHT_BUTTON:
		if (state == SDUT_DOWN)
		{
			bRmbDown = TRUE;
			prevOGLx = x;
			prevOGLy = y;
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

			prevOGLx = x;
			prevOGLy = y;
		}
		break;

	default:
		break;
	}
}

GLfloat currtime = 0.0f;

void update(void)
{
	// code
	currtime = sdutGet(SDUT_CURRENT_TIME)
		/ 1000.0f;

	angCube += 1.0f;
	if (angCube >= 360.0f)
		angCube = 0.0f;

	if (bIsKeydown)
		keyhandler.keyHandler();
}

void uninitialize(void)
{
	// code
	if (sky)
	{
		sky->~Sphere();
		sky = NULL;
	}
	FreeConsole();
}

void KeyHandler::keyHandler(void)
{
	// code
	switch (key)
	{
	case 'W':
	case 'w':
		eye += lookdir.unitv() * 0.1f;
		break;

	case 'S':
	case 's':
		eye -= lookdir.unitv() * 0.1f;
		break;

	case 'A':
	case 'a':
		eye -= vec3(lookdir * up).unitv() *
			0.1f;
		break;

	case 'D':
	case 'd':
		eye += vec3(lookdir * up).unitv() *
			0.1f;
		break;

	case VK_ESCAPE:
		sdutLeaveMainLoop();
		break;

	default:
		break;
	}
}

void console(void)
{
	// variables
	FILE* fout = NULL, *ferr = NULL;
	FILE* fin = NULL;

	// code
	AllocConsole();
	freopen_s(&fout, "CONOUT$", "w", stdout);
	freopen_s(&ferr, "CONOUT$", "w", stderr);
	freopen_s(&fin, "CONOUT$", "r", stdin);
	printf("console log enabled\n");
}

void scene(void)
{
	// function prototypes
	void grid(void);
	void cube(void);

	// code
	if (sky == NULL)
	{
		sky = new Sphere(
			Color(0.282f, 0.584f, 0.859f),
			50.0f);
	}

	grid();
	sky->draw();
	cube();
}

void grid(void)
{
	// variables
	int i_zFar = (int)zFar;

	// code
	glColor3f(0.5f, 0.5f, 0.5f);
	glPushMatrix();

	glBegin(GL_LINES);
	for (int x = -winwidth / 2;
		x <= winwidth / 2; ++x)
	{
		glVertex3f((GLfloat)x,
			0.0f, (GLfloat)zFar);
		glVertex3f((GLfloat)x,
			0.0f, -(GLfloat)zFar);
		for (int z = -zFar; z <= zFar;
			++z)
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

void cube(void)
{
	// code
	glPushMatrix();
		glTranslatef(0.0f, 0.0f, 0.25f);

		glBegin(GL_QUADS);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(0.25f, 0.25f, 0.0f);
		glVertex3f(-0.25f, 0.25f, 0.0f);
		glVertex3f(-0.25f, -0.25f, 0.0f);
		glVertex3f(0.25f, -0.25f, 0.0f);
		glEnd();

		glBegin(GL_QUADS);
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.25f, 0.25f, -0.5f);
		glVertex3f(-0.25f, 0.25f, -0.5f);
		glVertex3f(-0.25f, -0.25f, -0.5f);
		glVertex3f(0.25f, -0.25f, -0.5f);
		glEnd();

		glBegin(GL_QUADS);
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.25f, 0.25f, 0.0f);
		glVertex3f(0.25f, 0.25f, -0.5f);
		glVertex3f(-0.25f, 0.25f, -0.5f);
		glVertex3f(-0.25f, 0.25f, 0.0f);
		glEnd();

		glBegin(GL_QUADS);
		glColor3f(0.0f, 1.0f, 1.0f);
		glVertex3f(-0.25f, 0.25f, 0.0f);
		glVertex3f(-0.25f, 0.25f, -0.5f);
		glVertex3f(-0.25f, -0.25f, -0.5f);
		glVertex3f(-0.25f, -0.25f, 0.0f);
		glEnd();

		glBegin(GL_QUADS);
		glColor3f(1.0f, 0.0f, 1.0f);
		glVertex3f(-0.25f, -0.25f, 0.0f);
		glVertex3f(-0.25f, -0.25f, -0.5f);
		glVertex3f(0.25f, -0.25f, -0.5f);
		glVertex3f(0.25f, -0.25f, 0.0f);
		glEnd();

		glBegin(GL_QUADS);
		glColor3f(1.0f, 1.0f, 0.0f);
		glVertex3f(0.25f, -0.25f, 0.0f);
		glVertex3f(0.25f, -0.25f, -0.5f);
		glVertex3f(0.25f, 0.25f, -0.5f);
		glVertex3f(0.25f, 0.25f, 0.0f);
		glEnd();
	glPopMatrix();
}
