#ifndef TRANSLATE_SCALE_ROTATE_H
#define TRANSLATE_SCALE_ROTATE_H

#ifndef __FREEGLUT_H__
#include <GL/freeglut.h>
#endif

void translatef(GLfloat x, GLfloat y, GLfloat z);
void scalef(GLfloat x, GLfloat y, GLfloat z);
void rotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);

#endif
