#include "../include/Texture.h"

// stb_image related headers
#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"

GLuint imgWidth = 64;
GLuint imgHeight = 64;

GLuint initTex(const char* filename)
{
	// variables
	GLuint width = 0, height = 0;
	GLuint nrChannels = 0;
	GLuint texture = 0;

	// code
	imgWidth = width;
	imgHeight = height;

	// first set stbi to flip tex vertically before loading
	stbi_set_flip_vertically_on_load(1);

	unsigned char* data = stbi_load(filename, (int*)&width, (int*)&height,
		(int*)&nrChannels, 0);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
		GL_UNSIGNED_BYTE, data);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, GL_RGB,
		GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);
	return(texture);
}

void texQuad2D(GLuint texture,
	GLfloat* texCoords, GLfloat* quadCoords)
{
	// code
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBindTexture(GL_TEXTURE_2D, texture);

	glBegin(GL_QUADS);
	glTexCoord2f(texCoords[0], texCoords[1]);
	glVertex3f(quadCoords[0], quadCoords[1], 0.0f);

	glTexCoord2f(texCoords[2], texCoords[3]);
	glVertex3f(quadCoords[2], quadCoords[3], 0.0f);

	glTexCoord2f(texCoords[4], texCoords[5]);
	glVertex3f(quadCoords[4], quadCoords[5], 0.0f);

	glTexCoord2f(texCoords[6], texCoords[7]);
	glVertex3f(quadCoords[6], quadCoords[7], 0.0f);
	glEnd();

	glFlush();
	glDisable(GL_TEXTURE_2D);
}

void loadGLTexture(GLuint* tex, GLsizei width, GLsizei height,
	const GLvoid* image)
{
	// code
	// decide how OGL should store &
	// align bits of img. in memory
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// create OGL texture object
	glGenTextures(1, tex);

	// bind with newly created OGL texture object
	glBindTexture(GL_TEXTURE_2D, *tex);

	// set OGL tex object's parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
		GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
		GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
		GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_NEAREST);

	// accordingly build tex. obj. & its mipmaps
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
		0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, width, height,
		GL_RGBA, GL_UNSIGNED_BYTE, image);

	// unbind OGL tex. object
	glBindTexture(GL_TEXTURE_2D, 0);
}
