#include <cmath>
#include "../include/Object.h"

Color color(GLfloat red, GLfloat green,
	GLfloat blue, GLfloat alpha)
{
	Color col = { red, green, blue, alpha };
	return(col);
}

GLfloat* vect(Color* col)
{
	return(&col->r);
}

static ObjVect* obj_vect = NULL;

static void description(Object* o) {}
static void updateObject(Object* o) {}

static bool dontHandleCollision(Object*, Object*)
{
	return(false);
}

bool handleCollision(Object* objA, Object* objB)
{
	Object::Property prop = Object::NO_COLLISION;

	// code
	if (objA->property == Object::HANDLE_COLLISION &&
		objB->property == Object::HANDLE_COLLISION)
	{
		prop = Object::HANDLE_COLLISION;
	}
	else if ((objA->property == Object::TEST_COLLISION
		&& objB->property == Object::HANDLE_COLLISION)
		|| (objB->property == Object::TEST_COLLISION
			&& objA->property == Object::HANDLE_COLLISION))
	{
		prop = Object::TEST_COLLISION;
	}
	else if (objA->property == Object::TEST_COLLISION
		&& objB->property == Object::TEST_COLLISION)
	{
		prop = Object::TEST_COLLISION;
	}
	else
	{
		return(false);
	}

	GLfloat leftA = objA->posX - objA->sizeX / 4.0f;
	GLfloat rightA = objA->posX + objA->sizeX / 4.0f;
	GLfloat bottomA = objA->posY - objA->sizeY / 4.0f;
	GLfloat topA = objA->posY + objA->sizeY / 4.0f;
	GLfloat rearA = objA->posZ - objA->sizeZ / 4.0f;
	GLfloat frontA = objA->posZ + objA->sizeZ / 4.0f;

	GLfloat leftB = objB->posX - objB->sizeX / 4.0f;
	GLfloat rightB = objB->posX + objB->sizeX / 4.0f;
	GLfloat bottomB = objB->posY - objB->sizeY / 4.0f;
	GLfloat topB = objB->posY + objB->sizeY / 4.0f;
	GLfloat rearB = objB->posZ - objB->sizeZ / 4.0f;
	GLfloat frontB = objB->posZ + objB->sizeZ / 4.0f;

	if (objB->bIsBounding)
	{
		GLfloat pushX = 0, pushZ = 0;

		if (leftA < leftB)
			pushX = leftB - leftA;
		else if (rightA > rightB)
			pushX = rightB - rightA;

		if (rearA < rearB)
			pushZ = rearB - rearA;
		else if (frontA > frontB)
			pushZ = frontB - frontA;

		if (pushX == 0 && pushZ == 0)
			return false;

		if (prop == Object::TEST_COLLISION)
			return true;

		objA->posX += pushX;
		objA->posZ += pushZ;
		return(true);
	}
	else
	{
		if ((leftA < rightB && rightA > leftB) &&
			(bottomA < topB && topA > bottomB) &&
			(rearA < frontB && frontA > rearB))
		{
			if (prop == Object::TEST_COLLISION)
				return(true);

			GLfloat overlapX = fmin(rightA, rightB)
				- fmax(leftA, leftB);
			GLfloat overlapY = fmin(topA, topB)
				- fmax(bottomA, bottomB);
			GLfloat overlapZ = fmin(frontA, frontB)
				- fmax(rearA, rearB);

			if (overlapX <= overlapY &&
				overlapX <= overlapZ)
			{
				if (objA->posX >= objB->posX)
					objA->posX += overlapX;
				else
					objA->posX -= overlapX;
			}
			else if (overlapY <= overlapX
				&& overlapY <= overlapZ)
			{
				if (objA->posY >= objB->posY)
					objA->posY += overlapY;
				else
					objA->posY -= overlapY;
			}
			else
			{
				if (objA->posZ >= objB->posZ)
					objA->posZ += overlapZ;
				else
					objA->posZ -= overlapZ;
			}
			return(true);
		}
	}
	return(false);
}

<<<<<<< HEAD
void objectInit(Object* obj, Object::Property prop, GLfloat v[], GLuint count)
=======
void objectInit(Object* obj, Object::Property prop)
>>>>>>> e1f1a149b16082dc38e84592eb6da50cde74f768
{
	// code
	memset(obj, 0, sizeof(Object));

<<<<<<< HEAD
	obj->vertices = NULL;
	obj->vertexCount = 0;
	if (count > 0)
	{
		obj->vertices = (GLfloat*)malloc(count * sizeof(GLfloat));
		if (obj->vertices != NULL)
		{
			memcpy(obj->vertices, v, count * sizeof(GLfloat));
			obj->vertexCount = count;
		}
	}

=======
>>>>>>> e1f1a149b16082dc38e84592eb6da50cde74f768
	obj->sizeX = 1.0f;
	obj->sizeY = 1.0f;
	obj->sizeZ = 1.0f;

	obj->baseRad = 0.5f;
	obj->topRad = 0.5f;

	obj->color = color(0.0f, 0.0f, 0.0f, 1.0f);
	obj->property = prop;

	obj->description = description;
	obj->updateObject = updateObject;

	if (obj->property == Object::TEST_COLLISION ||
		obj->property == Object::HANDLE_COLLISION)
	{
		obj->left = obj->posX - obj->sizeX / 4.0f;
		obj->right = obj->posX + obj->sizeX / 4.0f;
		obj->bottom = obj->posY - obj->sizeY / 4.0f;
		obj->top = obj->posY + obj->sizeY / 4.0f;
		obj->rear = obj->posZ - obj->sizeZ / 4.0f;
		obj->front = obj->posZ + obj->sizeZ / 4.0f;

		obj->handleCollision = handleCollision;
	}
	else
	{
		obj->handleCollision = dontHandleCollision;
	}
}

Object* newObject(Object::Property prop)
{
	Object* obj = (Object*)malloc(sizeof(Object));
<<<<<<< HEAD
	GLfloat v[1];

	if (obj != NULL)
		objectInit(obj, prop, v, 0);
=======
	if (obj != NULL)
		objectInit(obj, prop);
>>>>>>> e1f1a149b16082dc38e84592eb6da50cde74f768

	return(obj);
}

void drawObjects(ObjVect* Objects)
{
	// variables
	obj_vect = Objects;

	// code
	for (int i = 0; i < obj_vect->objs; ++i)
	{
		obj_vect->objects[i].description(
			&obj_vect->objects[i]);
		obj_vect->objects[i].updateObject(
			&obj_vect->objects[i]);
	}
}

void handleCollisionsOf(Object* object)
{
	if (obj_vect == NULL || object == NULL)
		return;

	object->description(object);
	for (int i = 0; i < obj_vect->objs; ++i)
		object->handleCollision(object,
			&obj_vect->objects[i]);
	object->updateObject(object);
}

void pushBack(ObjVect* vect, Object object)
{
	// variables
	const int initCap = 3;

	if (vect->objs == vect->cap)
	{
		int newCap = (vect->cap == 0) ?
			initCap : 2 * vect->cap;

		Object* tmp = (Object*)realloc(
			vect->objects, newCap * sizeof(Object));
		if (tmp == NULL)
			return;

		vect->objects = tmp;
		vect->cap = newCap;
	}
	vect->objects[vect->objs++] = object;
}

void freeObjectData(Object* obj)
{
	if (obj->texCoords != NULL)
	{
		free(obj->texCoords);
		obj->texCoords = NULL;
	}
	if (obj->quadCoords != NULL)
	{
		free(obj->quadCoords);
		obj->quadCoords = NULL;
	}
	if (obj->texture != 0)
	{
		glDeleteTextures(1, &obj->texture);
		obj->texture = 0;
	}
}

void popBack(ObjVect* vect)
{
	if (vect->objs > 0)
	{
		freeObjectData(&vect->objects[vect->objs - 1]);
		--vect->objs;
	}
}

ObjVect objVect(int objs)
{
	// variables
	ObjVect vect;

	// code
	memset(&vect, 0, sizeof(ObjVect));
	vect.objects = (Object*)malloc(
		objs * sizeof(Object));

	if (vect.objects != NULL)
		vect.cap = objs;

	vect.pushBack = pushBack;
	vect.popBack = popBack;
	return(vect);
}

void emptyObjVect(ObjVect* vect)
{
	for (int i = 0; i < vect->objs; ++i)
		freeObjectData(&vect->objects[i]);
	vect->objs = 0;
}

void deleteObjVect(ObjVect* vect)
{
	for (int i = 0; i < vect->objs; ++i)
		freeObjectData(&vect->objects[i]);

	if (vect->objects != NULL)
	{
		free(vect->objects);
		vect->objects = NULL;
	}
	if (obj_vect == vect)
	{
		obj_vect = NULL;
	}
	vect->objs = 0;
	vect->cap = 0;
}
