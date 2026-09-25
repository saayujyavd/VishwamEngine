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
/*export{*/
#include <cstdio>
#include <cmath>
#include "../include/SDUT.h"
/*}*/

#ifdef _WIN32
	#include <direct.h>
	#define mkdir(path) _mkdir(path)
#else
	#include <sys/stat.h>
	#include <sys/types.h>
	#define mkdir(path) mkdir(path, 0777)
#endif

/*export{*/
#pragma comment(lib, "./lib/sdut.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

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
/*}*/

class Export
{
	// variables
	char str[50];

public:
	// function definitions
	void exportCode(void)
	{
		// variables
		FILE* pf_export = NULL;
		FILE* f_in = NULL;
		int c, str_idx = 0;

		// code
		if (pf_export == NULL)
		{
			mkdir("./Exports");
			if ((pf_export = fopen("Exports/OGL.cpp", "w"))
				== NULL)
			{
				MessageBox(NULL, TEXT("Failed to"
					" open file for export"),
					TEXT("Error"), MB_OK);
			}
		}
		if (f_in == NULL)
		{
			if ((f_in = fopen("Vishwam/src/OGL.cpp", "r"))
				== NULL)
			{
				MessageBox(NULL, TEXT("Failed to"
					" open file for reading"),
					TEXT("Error"), MB_OK);
			}
		}

		while (fgets(str, 50, f_in) != NULL)
		{
			if (strcmp(str, "/*export{*/\n") == 0)
			{
				while (fgets(str, 50, f_in) != NULL)
				{
					if (strcmp(str, "/*}*/\n") == 0)
						break;
					else
						fprintf(pf_export, "%s", str);
				}
			}
		}

		if (f_in)
		{
			fclose(f_in);
			f_in = NULL;
		}
		if (pf_export)
		{
			fclose(pf_export);
			pf_export = NULL;
		}
	}
};

// global variables
BOOL bFullscreen = FALSE;
BOOL bRmbDown = FALSE;
Export exprt;

/*export{*/
BOOL bIsKeydown = FALSE;
BOOL bFreeLook = TRUE;

KeyHandler keyhandler;
Sphere* sky = NULL;

int winwidth = WIN_WIDTH;
int winheight = WIN_HEIGHT;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpszCmdLine, int iCmdShow)
{
	// function prototypes
	void initialize(void);
	void uninitialize(void);
	void resize(int, int);
	void render(void);
	void update(void);
/*}*/
	void console(void);
	void keyboard(unsigned char, int, int);
	void keyUp(unsigned char, int, int);
	void mouse(int, int, int, int);

	// code
	// first enable the console
	console();

/*export{*/
	// then initialize & create the window
	sdutInit(iCmdShow, (char**)lpszCmdLine);
	sdutInitDisplayMode(SDUT_DOUBLE | SDUT_RGBA | SDUT_DEPTH);
	sdutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);
	sdutInitWindowPosition(100, 100);
	sdutCreateWindow("Saayujya Deshpande");
	initialize();

	sdutReshapeFunc(resize);
	sdutRenderFunc(render);
	sdutUpdateFunc(update);
	sdutCloseFunc(uninitialize);
/*}*/
	sdutKeyboardFunc(keyboard);
	sdutKeyUpFunc(keyUp);
	sdutMouseFunc(mouse);
/*export{*/
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

	// double buffering
	sdutSwapBuffers();
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

	case 'E':
	case 'e':
		exprt.exportCode();
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
			// remove the person cam effect
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
}

void mouse(int button, int state, int x, int y)
{
	// function prototypes
	void freeLook(int, int, int, int);

	// code
	freeLook(button, state, x, y);
	if(bFreeLook == FALSE)
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

	angCube += 1.0f;
	if (angCube >= 360.0f)
		angCube = 0.0f;

	if (camSinAng >= 6.28f)
		camSinAng = 0.0f;

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
		break;

	case 'S':
	case 's':
		eye -= lookdir.unitv() * camspeed;
		break;

	case 'A':
	case 'a':
		eye -= vec3(lookdir * up).unitv() *
			camspeed;
		break;

	case 'D':
	case 'd':
		eye += vec3(lookdir * up).unitv() *
			camspeed;
		break;

	case VK_ESCAPE:
		sdutLeaveMainLoop();
		break;

	default:
		break;
	}
	person();
}
/*}*/

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

/*export{*/
void scene(void)
{
	// function prototypes
/*}*/
	void grid(void);
/*export{*/
	void land(Color&);

	// code
/*}*/
	grid();
/*export{*/
	land(Color(0.0f, 1.0f, 0.0f));
}
/*}*/

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

/*export{*/
void land(Color& color)
{
	// variables
	GLfloat halfwidth = (GLfloat)winwidth / 2.0f;
	GLfloat halfdepth = (GLfloat)zFar / 2.0f;

	// code
	glColor3f(color.x, color.y, color.z);
	glBegin(GL_QUADS);
		glVertex3f(halfwidth, -0.01f, -halfdepth);
		glVertex3f(-halfwidth, -0.01f, -halfdepth);
		glVertex3f(-halfwidth, -0.01f, halfdepth);
		glVertex3f(halfwidth, -0.01f, halfdepth);
	glEnd();
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

void person(void)
{
	// code
	if (bFreeLook == FALSE)
	{
		eye.y = 0.5 + (0.1f
			* sinf(camSinAng));
		camSinAng += 0.1f;
	}
}

void freeLook(int button, int state, int x, int y)
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
/*}*/
