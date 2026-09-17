#include <cstdio>
#include <cmath>
#include "./include/SDUT.h"
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

	// specify look at
	gluLookAt(eye.x, eye.y, eye.z,
		lookat.x, lookat.y, lookat.z,
		up.x, up.y, up.z);
	// draw the scene
	scene();

	// double buffering
	sdutSwapBuffers();
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
void scene(void)
{
	// function prototypes
	void cube(void);

	// code
	if (sky == NULL)
	{
		sky = new Sphere(
			Color(0.282f, 0.584f, 0.859f),
			50.0f);
	}
	sky->draw();
	cube();
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
