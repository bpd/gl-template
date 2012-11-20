
#include <string.h> // memset
#include <math.h>   // sin,cos

#include "glm.h"

glmMat4f glmMulMat4f( glmMat4f u, glmMat4f v )
{
	glmMat4f matrix;
	matrix.a = (u.a * v.a) + (u.b * v.e) + (u.c * v.i) + (u.d * v.m);
	matrix.b = (u.a * v.b) + (u.b * v.f) + (u.c * v.j) + (u.d * v.n);
	matrix.c = (u.a * v.c) + (u.b * v.g) + (u.c * v.k) + (u.d * v.o);
	matrix.d = (u.a * v.d) + (u.b * v.h) + (u.c * v.l) + (u.d * v.p);
	
	matrix.e = (u.e * v.a) + (u.f * v.e) + (u.g * v.i) + (u.h * v.m);
	matrix.f = (u.e * v.b) + (u.f * v.f) + (u.g * v.j) + (u.h * v.n);
	matrix.g = (u.e * v.c) + (u.f * v.g) + (u.g * v.k) + (u.h * v.o);
	matrix.h = (u.e * v.d) + (u.f * v.h) + (u.g * v.l) + (u.h * v.p);
	
	matrix.i = (u.i * v.a) + (u.j * v.e) + (u.k * v.i) + (u.l * v.m);
	matrix.j = (u.i * v.b) + (u.j * v.f) + (u.k * v.j) + (u.l * v.n);
	matrix.k = (u.i * v.c) + (u.j * v.g) + (u.k * v.k) + (u.l * v.o);
	matrix.l = (u.i * v.d) + (u.j * v.h) + (u.k * v.l) + (u.l * v.p);
	
	matrix.m = (u.m * v.a) + (u.n * v.e) + (u.o * v.i) + (u.p * v.m);
	matrix.n = (u.m * v.b) + (u.n * v.f) + (u.o * v.j) + (u.p * v.n);
	matrix.o = (u.m * v.c) + (u.n * v.g) + (u.o * v.k) + (u.p * v.o);
	matrix.p = (u.m * v.d) + (u.n * v.h) + (u.o * v.l) + (u.p * v.p);
	
	return matrix;
}

glmVec3f glmNormalize( glmVec3f u )
{
	float magnitude = sqrt( (u.x * u.x) + (u.y * u.y) + (u.z * u.z) );
	
	glmVec3f v = {
		u.x / magnitude,
		u.y / magnitude,
		u.z / magnitude
	};
	
	return v;
}

glmVec3f glmCross( glmVec3f u, glmVec3f v )
{
	glmVec3f w = {
		   (u.y * v.z) - (v.y * u.z),
		-( (u.x * v.z) - (v.x * u.z) ),
		   (u.x * v.y) - (v.x * u.y)
	};
	
	return w;
}

static float glmDot( glmVec3f u, glmVec3f v )
{
	return (u.x * v.x) + (u.y * v.y) + (u.z * v.z);
}

glmMat4f glmLookAt( glmVec3f eye, glmVec3f center, glmVec3f up )
{
	glmVec3f f = {
		center.x - eye.x,
		center.y - eye.y,
		center.z - eye.z
	};
	f = glmNormalize( f );

	glmVec3f s = glmNormalize( glmCross(f, up) );
	glmVec3f u = glmCross(s, f);

	glmMat4f m = {
		s.x,	u.x,	-f.x,	0,
		s.y,	u.y,	-f.y,	0,
		s.z,	u.z,	-f.z,	0,
		0,		0,		0,		1
	};

	return glmMulMat4f( m, glmTranslatef( -eye.x, -eye.y, -eye.z ) );	
}

glmMat4f glmIdentity()
{
	glmMat4f matrix = {
		1,	0,	0,	0,
		0,	1,	0,	0,
		0,	0,	1,	0,
		0,	0,	0,	1
	};
	
	return matrix;
}

glmMat4f glmTranslatef( float x, float y, float z )
{
	glmMat4f matrix = {
		1,	0,	0,	x,
		0,	1,	0,	y,
		0,	0,	1,	z,
		0,	0,	0,	1
	};
	
	return matrix;
}

glmMat4f glmScalef( float x, float y, float z )
{
	glmMat4f matrix = {
		x,	0,	0,	0,
		0,	y,	0,	0,
		0,	0,	z,	0,
		0,	0,	0,	1
	};
	
	return matrix;
}

static const float PI_OVER_360 = 3.14159265358979323846264338327950288f / 360.0f;

glmMat4f glmPerspective( float fov, float aspect , float zNear, float zFar )
{
	const float h = 1.0f / tan( fov * PI_OVER_360 );
	float neg_depth = zNear - zFar;
	
	glmMat4f matrix = {
		h/aspect,	0,	0,														0,
		0,				h,	0,														0,
		0,				0,	(zFar + zNear)/neg_depth,			(2.0f*(zNear*zFar))/neg_depth,
		0,				0,	-1,														0
	};
	
	return matrix;
}

/**
			 |  1  0       0       0 |
	 M = |  0  cos(A) -sin(A)  0 |
			 |  0  sin(A)  cos(A)  0 |
			 |  0  0       0       1 |
*/
glmMat4f glmRotateX( float deg )
{
	glmMat4f matrix = {
		1,		0,				0,					0,
		0,		cos(deg),	-sin(deg),	0,
		0,		sin(deg),	cos(deg),		0,
		0,		0,				0,					1
	};
	
	return matrix;
}

/**
			 |  cos(A)  0   sin(A)  0 |
	 M = |  0       1   0       0 |
			 | -sin(A)  0   cos(A)  0 |
			 |  0       0   0       1 |
*/
glmMat4f glmRotateY( float deg )
{
	glmMat4f matrix = {
		cos(deg),		0,		sin(deg),		0,
		0,					1,		0,					0,
		-sin(deg),	0,		cos(deg),		0,
		0,					0,		0,					1
	};
	
	return matrix;
}

/**
			 |  cos(A)  -sin(A)   0   0 |
	 M = |  sin(A)   cos(A)   0   0 |
			 |  0        0        1   0 |
			 |  0        0        0   1 |
*/
glmMat4f glmRotateZ( float deg )
{
	glmMat4f matrix = {
		cos(deg),		-sin(deg),	0,		0,
		sin(deg),		cos(deg),		0,		0,
		0,					0,					1,		0,
		0,					0,					0,		1
	};
	
	return matrix;
}

