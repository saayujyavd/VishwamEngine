#ifndef TEXTURE_H
#define TEXTURE_H

// header files
#include <windows.h>
using namespace std;

// OGL related headers
#include <gl/GL.h>
#include <gl/GLU.h>

GLuint initTex(const char*);
void texQuad2D(GLuint,
	GLfloat*, GLfloat*);

#endif
