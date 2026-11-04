#ifndef BITMAP_H
#define BITMAP_H

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
	BOOL operator!=(Point& v)
	{
		if (x != v.x || y != v.y ||
			z != v.z) return(TRUE);
		else return(FALSE);
	}
	BOOL operator==(Point& v)
	{
		if ((abs(x - v.x) <= 0.1f)
			&& (abs(y - v.y) <= 0.1f)
			&& (abs(y - v.y) <= 0.1f))
			return(TRUE);
		else return(FALSE);
	}
	BOOL nearPoint(Point& v)
	{
		if ((abs(x - v.x) <= 0.3f)
			&& (abs(y - v.y) <= 0.3f)
			&& (abs(y - v.y) <= 0.3f))
			return(TRUE);
		else return(FALSE);
	}
	GLfloat magnitude(void)
	{
		return(sqrtf(x * x + y * y +
			z * z));
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

void bitmap(char fname[], GLfloat pos_x, GLfloat pos_y,
	GLfloat scale_x, GLfloat scale_y, unsigned int alpha,
	BOOL file_read_flag);
void quarticBezier(Point a, Point b, Point c, Point d,
	Point e, GLfloat thickness);

#endif
