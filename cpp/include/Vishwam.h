#ifndef VISHWAM_H
#define VISHWAM_H

// header files
#include <windows.h>
#include <cstdio>  // for file io
#include <cstdlib> // for exit()
#include <cmath>
#include <vector>
#include <string>
#include <array>
#include <cstring>
#include <ctime>
#include <filesystem>
using namespace std;

// OGL related headers
#include <gl/GL.h>
#include <gl/GLU.h>

class Vishwam;
class Point;

typedef Point Color;
typedef Point vec3;

extern vector<Vishwam*> objects;
extern vector<Vishwam*> childObjs;
extern vector<string> objlist;
extern int objIdx;
extern GLfloat currtime;
extern vec3 eye;

GLfloat randFloat(GLfloat, GLfloat);
void registerObjects(void);

typedef struct Point
{
	GLfloat x, y, z;

	Point();
	Point(GLfloat x, GLfloat y, GLfloat z);

	Point& operator+=(Point& v);
	Point& operator-=(Point& v);
	Point operator+(Point& v);
	Point operator+(GLfloat f);
	Point operator-(Point& v);
	Point operator-(GLfloat f);
	Point operator*(Point& v);
	Point operator*(GLfloat f);
	GLfloat magnitude(void);
	GLfloat dot(Point& v);
	array<GLfloat, 3> vect(void);
	Point unitv(void);
	Point randP(GLfloat min, GLfloat max);
} Point;

class Vishwam
{
public:
	enum Property { NO_COLLISION, TEST_COLLISION, HANDLE_COLLISION };

	// variables
private:
	inline static vector<Vishwam*>* objs = nullptr;
public:
	inline static string className;
	BOOL bIsBounding = FALSE;
	Property property = HANDLE_COLLISION;
	Color color = Color(0.0f, 0.0f, 0.0f);
	GLfloat alpha = 1.0f;
	GLfloat posX = 0.0f, posY = 0.0f, posZ = 0.0f;
	GLfloat sizeX = 1.0f, sizeY = 1.0f, sizeZ = 1.0f;
	GLfloat ang = 0.0f;
	GLfloat axisX = 0.0f, axisY = 0.0f, axisZ = 0.0f;
	GLfloat left = 0.0f, right = 0.0f, bottom = 0.0f, top = 0.0f;
	GLfloat rear = 0.0f, front = 0.0f;

	// editable geometry (x, y, z per vertex);
	// empty for objects without editable vertices
	vector<GLfloat> vertices;

	// function definitions
private:
	bool handleCollision(Vishwam& obj);

protected:
	virtual void draw(void) = 0;
	virtual void updateObject(void);

public:
	Vishwam(Property prop = NO_COLLISION);
	Vishwam(Vishwam& copy);

	static void drawObjects(vector<Vishwam*>& objects);
	static void handleCollisionsOf(Vishwam& object);
	static void popBack(vector<Vishwam*>& objects);
	static void emptyObjVect(vector<Vishwam*>& objects);

	template<typename T>
	static void registerClass(const string& name)
	{
		static_assert(is_base_of_v<Vishwam, T>,
			"registerClass: class must be derived from Vishwam");
		childObjs.push_back(new T);
		className = name;
	}

	virtual Vishwam* clone(void) = 0;
	virtual ~Vishwam();
};

#endif
