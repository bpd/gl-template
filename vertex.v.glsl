#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;

layout(location = 1) in float intensity;

uniform float threshold;

uniform mat4 MVP;

mat4 glmTranslate( float x, float y, float z )
{
	return mat4 (
		1,	0,	0,	x,
		0,	1,	0,	y,
		0,	0,	1,	z,
		0,	0,	0,	1
	);
}

void main()
{
	// transform to 4d vector
	vec4 v = vec4( vertexPosition_modelspace, 1 );
	
	if( intensity < threshold )
	{
		gl_PointSize = 0;
		gl_Position = vec4( 0, 0.0, 0.0, 0.0 );
	}
	else
	{
		gl_PointSize = 3;
		
		mat4 trans = glmTranslate( 0.5, 0.2, 2 );
		
		gl_Position = trans * MVP * v;
	}

	
}