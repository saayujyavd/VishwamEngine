/*
* Title : Vishwam
* Date : 13 - 09 -2026
* Author : Saayujya Deshpande
* Technology Used : Win32 SDK
* Programming Language : C++ Language
* References:
	1. Gokhale sir's live code
*/

// header files
#include <windows.h>

// OGL related headers
#include <gl/GL.h>
#include <gl/GLU.h>

// Vishwam engine header files
#include "../include/Vishwam.h"

class Ocean : public Vishwam
{
	static constexpr GLint numVertexX = 300;
	static constexpr GLint numVertexY = 300;
	static constexpr GLint numWaves = 12;
	GLfloat waveNum[numWaves], amplitude[numWaves];
	GLfloat dirX[numWaves], dirZ[numWaves];
	GLfloat phase[numWaves], omega[numWaves];
	Point vertices[numVertexX][numVertexY]{};
	vec3 normals[numVertexX][numVertexY]{};
	Color vertexCol[numVertexX][numVertexY]{};

	void draw(void) override
	{
		const GLfloat steep = 0.8f;
		GLfloat pX = 0.0f;
		Color sky(0.53f, 0.81f, 1.0f);

		// wave math (Gerstner waves)
		for (GLint x = 0; x < numVertexX; ++x, pX += 0.05f)
		{
			GLfloat pZ = 0.0f;
			for (GLint z = 0; z < numVertexY; ++z, pZ += 0.05f)
			{
				GLfloat vX = pX, vY = 0.0f, vZ = pZ;
				GLfloat nX = 0.0f, nY = 1.0f, nZ = 0.0f;

				for (GLint k = 0; k < numWaves; ++k)
				{
					GLfloat dX = dirX[k], dZ = dirZ[k];
					GLfloat S = dX * pX + dZ * pZ;
					GLfloat theta = waveNum[k] * S - omega[k] * currtime + phase[k];
					GLfloat q = steep / (waveNum[k] * amplitude[k] * numWaves);

					GLfloat cos_t = cosf(theta);
					GLfloat sin_t = sinf(theta);

					vX += q * amplitude[k] * dX * cos_t;
					vZ += q * amplitude[k] * dZ * cos_t;
					vY += amplitude[k] * sin_t;

					nX -= dX * waveNum[k] * amplitude[k] * cos_t;
					nZ -= dZ * waveNum[k] * amplitude[k] * cos_t;
					nY -= q * waveNum[k] * amplitude[k] * sin_t;
				}

				// Fresnels effect
				vec3 vertexDir = vec3(vX * sizeX + posX - eye.x, vY * sizeY + posY - eye.y,
					vZ * sizeZ + posZ - eye.z).unitv();
				vec3 norm = vec3(nX / sizeX, nY / sizeY, nZ / sizeZ).unitv();
				GLfloat fresnelFact = 1.0f + norm.dot(vertexDir);

				if (fresnelFact > 1.0f) fresnelFact = 1.0f;
				else if (fresnelFact < 0.0f) fresnelFact = 0.0f;

				// Schlick's approximation
				fresnelFact = powf(fresnelFact, 5.0f);

				vertexCol[x][z].x = sky.x * fresnelFact + color.x * (1.0f - fresnelFact);
				vertexCol[x][z].y = sky.y * fresnelFact + color.y * (1.0f - fresnelFact);
				vertexCol[x][z].z = sky.z * fresnelFact + color.z * (1.0f - fresnelFact);

				/* foam effect
				GLfloat foam = (steep - norm.y) / 0.15f;
				if (foam > 1.0f) foam = 1.0f;
				else if (foam < 0.0f) foam = 0.0f;

				vertexCol[x][z].x = vertexCol[x][z].x * (1.0f - foam) + foam;
				vertexCol[x][z].y = vertexCol[x][z].y * (1.0f - foam) + foam;
				vertexCol[x][z].z = vertexCol[x][z].z * (1.0f - foam) + foam;
				*/
				vertices[x][z] = Point(vX, vY, vZ);
				normals[x][z] = norm;
			}
		}

		glPushMatrix();
		glTranslatef(posX, posY, posZ);
		glRotatef(ang, axisX, axisY, axisZ);
		glScalef(sizeX, sizeY, sizeZ);
		for (GLint x = 0; x < numVertexX - 1; ++x)
		{
			glBegin(GL_TRIANGLE_STRIP);
			for (GLint z = 0; z < numVertexY; ++z)
			{
				glColor3f(vertexCol[x][z].x, vertexCol[x][z].y, vertexCol[x][z].z);
				glNormal3f(normals[x][z].x, normals[x][z].y, normals[x][z].z);
				glVertex3f(vertices[x][z].x, vertices[x][z].y, vertices[x][z].z);

				glColor3f(vertexCol[x + 1][z].x, vertexCol[x + 1][z].y, vertexCol[x + 1][z].z);
				glNormal3f(normals[x + 1][z].x, normals[x + 1][z].y, normals[x + 1][z].z);
				glVertex3f(vertices[x + 1][z].x, vertices[x + 1][z].y, vertices[x + 1][z].z);
			}
			glEnd();
		}
		glPopMatrix();
	}

public:
	Ocean()
	{
		for (GLint i = 0; i < numVertexX; ++i)
		{
			for (GLint j = 0; j < numVertexY; ++j)
			{
				vertices[i][j] = Point((GLfloat)i, 0.0f,
					(GLfloat)j);
				normals[i][j] = vec3(0.0f, 1.0f, 0.0f);
				vertexCol[i][j] = Color(0.0f, 0.0f, 1.0f);
			}
		}

		// params. for Gerstner waves
		const GLfloat twoPi = 6.283185f;
		const GLfloat medianAmp = 0.003f;
		const GLfloat medianWaveLen = 0.3f;
		const GLfloat slopeRatio = medianAmp / medianWaveLen;
		GLfloat windAng = randFloat(0.0f, twoPi);

		for (GLint k = 0; k < numWaves; ++k)
		{
			GLfloat waveLen = randFloat(medianWaveLen / 2.0f, medianWaveLen * 2.0f);
			GLfloat ang = windAng + randFloat(0.0f, 1.0f);

			waveNum[k] = twoPi / waveLen;
			dirX[k] = cosf(ang);
			dirZ[k] = sinf(ang);
			phase[k] = randFloat(0.0f, twoPi);
			amplitude[k] = waveLen * slopeRatio;
			omega[k] = sqrtf(9.8f * waveNum[k]);
		}
	}

	Vishwam* clone(void) override
	{
		return(new Ocean());
	}
};

void registerObjects(void)
{
	Vishwam::registerClass<Ocean>("Ocean");
}
