
#ifndef GLM_H
#define GLM_H

typedef struct
{
	float x, y, z;
}
glmVec3f;

typedef struct
{
	float
		a, b, c, d,
		e, f, g, h,
		i, j, k, l,
		m, n, o, p;
}
glmMat4f;

glmMat4f glmMulMat4f( glmMat4f u, glmMat4f v );

glmVec3f glmNormalize( glmVec3f u );

glmVec3f glmCross( glmVec3f u, glmVec3f v );

void glmCross3f( float* u, float* v, float* dst );

glmMat4f glmIdentity();

glmMat4f glmTranslatef( float x, float y, float z );

glmMat4f glmScalef( float x, float y, float z );

glmMat4f glmPerspective( float fov, float aspect , float zNear, float zFar );

glmMat4f glmLookAt( glmVec3f eye, glmVec3f center, glmVec3f up );

glmMat4f glmRotateX( float degrees );
glmMat4f glmRotateY( float degrees );
glmMat4f glmRotateZ( float degrees );


#endif