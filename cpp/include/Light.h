#ifndef LIGHT_H
#define LIGHT_H

#include <windows.h>
#include <GL/GL.h>

void initLight(void);
void light(GLfloat* pos, GLfloat* color,
	GLfloat shininess, GLfloat specular,
	GLfloat ambient);
void uninitLight(void);

#endif
