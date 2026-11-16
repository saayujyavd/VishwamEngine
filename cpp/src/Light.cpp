#include "../include/Light.h"

void initLight(void)
{
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

void light(GLfloat* pos, GLfloat* color,
	GLfloat shininess, GLfloat specular, GLfloat ambient)
{
	// variables
	GLfloat model_ambient[] = { ambient, ambient, ambient, 1.0f };
	GLfloat specul[] = { specular, specular, specular, 1.0f };

	// code
	glMaterialfv(GL_FRONT, GL_SPECULAR, specul);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);
	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, color);
	glLightfv(GL_LIGHT0, GL_SPECULAR, color);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, model_ambient);
}

void uninitLight(void)
{
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);
	glDisable(GL_NORMALIZE);
	glDisable(GL_COLOR_MATERIAL);
}
