#ifndef OBJECT_H
#define OBJECT_H

#include <windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>

typedef struct Color
{
	GLfloat r, g, b, a;
} Color;

// function prototypes
Color color(GLfloat, GLfloat, GLfloat, GLfloat);
GLfloat* vect(Color*);

typedef struct Vector ObjVect;
typedef struct Object Object;

struct Object
{
	enum Property { NO_COLLISION, TEST_COLLISION, HANDLE_COLLISION };

	// variables
	BOOL bIsBounding;
	Property property;
	Color color;
	GLuint texture;
	GLuint vertexCount;
	GLfloat* vertices;
	GLfloat* texCoords, *quadCoords;
	GLfloat posX, posY, posZ;
	GLfloat sizeX, sizeY, sizeZ;
	GLfloat ang;
	GLfloat axisX, axisY, axisZ;
	GLfloat left, right, bottom, top;
	GLfloat rear, front;
	GLdouble baseRad, topRad;
	GLUquadric* quadric;

	// function pointer declarations
	void (*description)(Object*);
	bool (*handleCollision)(Object*, Object*);
	void (*updateObject)(Object*);
};

// function prototypes
Object* newObject(Object::Property);
ObjVect objVect(int);
void emptyObjVect(ObjVect*);
void deleteObjVect(ObjVect*);
void objectInit(Object*, Object::Property, GLfloat[], GLuint);
bool handleCollision(Object*, Object*);
void handleCollisionsOf(Object*);
void drawObjects(ObjVect*);

struct Vector
{
	// variables
	Object* objects;
	int objs;       // number of objects stored
	int cap;        // number of objects allocated

	// function pointer declarations
	void (*pushBack)(ObjVect*, Object);
	void (*popBack)(ObjVect*);
};

#endif
