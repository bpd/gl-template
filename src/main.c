/**
	
*/

#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GL/glfw.h>


#ifdef _MSC_VER

typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;

#else
#include <stdint.h>
#endif

#include <math.h>

//#include "game.h"
#include "glm.h"
#include "shader.h"

static const double PIXELS_PER_SECOND = 5.0;

static const float KEY_PRESS_OFFSET = 0.05f;

static GLfloat x = 0;
static GLfloat y = 0;

void handle_input()
{
  if( glfwGetKey( GLFW_KEY_LEFT ) == GLFW_PRESS ) // GLFW_RELEASE
	{
	  x -= KEY_PRESS_OFFSET;
	}
	
	if( glfwGetKey( GLFW_KEY_RIGHT ) == GLFW_PRESS ) // GLFW_RELEASE
	{
	  x += KEY_PRESS_OFFSET;
	}
	
	if( glfwGetKey( GLFW_KEY_UP ) == GLFW_PRESS ) // GLFW_RELEASE
	{
	  y += KEY_PRESS_OFFSET;
	}
	
	if( glfwGetKey( GLFW_KEY_DOWN ) == GLFW_PRESS ) // GLFW_RELEASE
	{
	  y -= KEY_PRESS_OFFSET;
	}
}

void animate( double dt )
{
  x += PIXELS_PER_SECOND * dt;
	
	if( x > 360.0 )
	{
	  x = 0.0f;
	}
}

static const char* VERTEX_SHADER = 
"//#version 330 core "

// Input vertex data, different for all executions of this shader.
" layout(location = 0) in vec3 vertexPosition_modelspace; "
" layout(location = 1) in float intensity; "

" uniform float threshold; "
" uniform mat4 MVP; "

" mat4 glmTranslate( float x, float y, float z ) "
" { "
" 	return mat4 ( "
" 		1,	0,	0,	x, "
" 		0,	1,	0,	y, "
" 		0,	0,	1,	z, "
" 		0,	0,	0,	1 "
" 	); "
" } "

" void main() "
" { "
" 	// transform to 4d vector "
" 	vec4 v = vec4( vertexPosition_modelspace, 1 ); "
	
" 	if( intensity < threshold ) "
" 	{ "
" 		gl_PointSize = 0; "
" 		gl_Position = vec4( 0, 0.0, 0.0, 0.0 ); "
" 	} "
" 	else "
"  	{ "
" 		gl_PointSize = 3; "
		
" 		mat4 trans = glmTranslate( 0.5, 0.2, 2 ); "
		
" 		gl_Position = trans * MVP * v; "
" 	}"

	
" } "
;

static const char* FRAGMENT_SHADER = 

"#version 330 core "

// Output data
"out vec4 color; "

"void main() "
"{ "
"	// Output color = red  "
"	color = vec4(1,0,0, 1); "
" } "
;


void init( int width, int height, char* title )
{
  if( !glfwInit() )
  {
		fprintf( stderr, "Unable to init GLFW\n" );
	}

  // open an OpenGL 3.3 'core' context
	glfwOpenWindowHint( GLFW_FSAA_SAMPLES, 4 );
	glfwOpenWindowHint( GLFW_OPENGL_VERSION_MAJOR, 3 );
	glfwOpenWindowHint( GLFW_OPENGL_VERSION_MINOR, 3 );
  glfwOpenWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
	glfwOpenWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );

	// open a window and creates OpenGL context
	if( !glfwOpenWindow( width, height, 0,0,0,0, 32,0, GLFW_WINDOW ) )
  {
			fprintf( stderr, "Failed to open GLFW window\n" );
      
			glfwTerminate();
			exit(1);
	}
  
  int major, minor, rev;
  glfwGetGLVersion( &major, &minor, &rev );
  printf( "OpenGL version: %d.%d.%d", major, minor, rev );
  
	// setup glew
  // 
  if( GLEW_OK != glewInit() )
  {
    // GLEW failed!
    fprintf( stderr, "Unable to init GLEW" );
    exit(1);
  }
	
	glfwSetWindowTitle( title );
  
  //fprintf( stdout, "version: %s", glGetString( GL_VERSION ) );
}

glmMat4f buildMatrix()
{
	// set 'mvp' uniform
	glmMat4f projection = glmPerspective(
		45.0f,			// horizontal Field of View (degrees), (90=wide,30=zoomed)
		4.0f / 3.0f,// Aspect Ratio
		0.1f,				// Near clipping plane, keep as big as possible
		100.0f			// Far clipping plane, keep as little as possible
	);
	
	glmVec3f eye = { 0, 0, 5 };
	glmVec3f center = { 0, 0, 0 };
	glmVec3f up = { 0, 1, 0 };
	
	glmMat4f view = glmLookAt( eye, center, up ); //glmIdentity();
	
	glmMat4f model = glmIdentity();	
	
	model = glmMulMat4f( model, glmScalef( 0.5, 0.5, 0.5f ) );
	
	//model = glmMulMat4f( model, glmTranslatef( 0, 0, y ) );
	
	//model = glmMulMat4f( model, glmRotateX( y ) );
	model = glmMulMat4f( model, glmRotateY( x ) );
	//model = glmMulMat4f( model, glmRotateZ( y ) );
	
	glmMat4f mvp = glmMulMat4f( projection, glmMulMat4f( view, model ) );
	
	return mvp;
}

#define ATTRIB_VERTEX 0
#define ATTRIB_INTENSITY 1

#define VERTEX_COUNT 10000

int main( int argc, char* argv[] )
{
  int running = GL_TRUE;
	double t = 0.0;
	
	GLuint vertexArrayID;
	GLuint vertexbuffer;
	GLuint vertexIntensityBuffer;
  
  printf("Initializing window\n");

  init( 1024, 768, "Test" );
  
  // setup buffers and shaders
	glClearColor( 0.0f, 0.0f, 0.3f, 0.0f );
  
  printf("Binding vertex arrays\n");
  
  glGenVertexArrays( 1, &vertexArrayID );
	glBindVertexArray( vertexArrayID );
  
  printf("Loading shaders...");
  
  GLuint programID = load_shader( VERTEX_SHADER, FRAGMENT_SHADER ); //load_shader_files( "vertex.v.glsl", "fragment.f.glsl" );
  
  printf(" loaded program %d \n", programID);
	
	GLfloat vertex_data[VERTEX_COUNT];
	for( int i=0; i<VERTEX_COUNT; i+=3 )
	{
		vertex_data[i] = ((float)(rand() % 100) / 100.0f) * 6.0f - 3;
		vertex_data[i+1] = ((float)(rand() % 100) / 100.0f) * 6.0f - 3;
		vertex_data[i+2] = 0;
	}
	/*	-1,	-1,	0,
		 1,	-1,	0,
		 0,	 1,	0,
	};*/
	
	GLfloat vertex_intensities[VERTEX_COUNT];
	for( int i=0; i<VERTEX_COUNT; i++ )
	{
		vertex_intensities[i] = rand() % 256;
	}
/*	= {
		100,
		200,
		50
	};*/
  
  printf("Generating buffers\n");
	
	glGenBuffers( 1, &vertexbuffer );
  glBindBuffer( GL_ARRAY_BUFFER, vertexbuffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW );
	
	glGenBuffers( 1, &vertexIntensityBuffer );
  glBindBuffer( GL_ARRAY_BUFFER, vertexIntensityBuffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof(vertex_intensities), vertex_intensities, GL_STATIC_DRAW );
	
	//GLfloat threshold = 100;
	
	GLuint thresholdUniformID = glGetUniformLocation( programID, "threshold" );
	
	GLuint mvpUniformID = glGetUniformLocation( programID, "MVP" );
	
	t = glfwGetTime();
	
	while( running )
	{
		double now = glfwGetTime();
    
    // if more than a second has passed, animate
    //animate( now -  t );
    t = now;
   
		handle_input();
    
    if( ( now - t  ) > (1.0f / 60.0f) )
    {
      continue;
		}
    
    printf("Rendering\n");
    
    // TODO: only render at a specific frame rate

    // setup GL config
		glEnable( GL_DEPTH_TEST );
		glDepthFunc( GL_LESS );
		
		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		
		// allow shaders to adjust point size
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
		
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    glUseProgram( programID );
    
		// set 'offset' uniform
    glUniform1f( thresholdUniformID, y );
		
		glmMat4f mvp = buildMatrix();
		
		glUniformMatrix4fv( mvpUniformID, 1, GL_TRUE, (const GLfloat*)&mvp );
	
    // bind the vertex attribute array
		glEnableVertexAttribArray( ATTRIB_VERTEX );
    glBindBuffer( GL_ARRAY_BUFFER, vertexbuffer );
    glVertexAttribPointer(
      ATTRIB_VERTEX,				// attribute 0
      3,				// size
      GL_FLOAT,	// type
      GL_FALSE,	// normalized?
      0,				// stride, skip intensity
      (void*)0	// array buffer offset
    );
		
		glEnableVertexAttribArray( ATTRIB_INTENSITY );
		glBindBuffer( GL_ARRAY_BUFFER, vertexIntensityBuffer );
		glVertexAttribPointer(
      ATTRIB_INTENSITY,				// attribute 1
      1,				// size
      GL_FLOAT,	// type
      GL_FALSE,	// normalized?
      0,				// stride, skip vertex
      (void*)0	// array buffer offset
    );
		
		
		
    
    // run the vertices through the shader
    glDrawArrays( GL_POINTS, 0, VERTEX_COUNT ); // starting from vertex 0; 3 vertices total
    
		glDisableVertexAttribArray( ATTRIB_INTENSITY );
    glDisableVertexAttribArray( ATTRIB_VERTEX );
		
		// Swap front and back rendering buffers
		glfwSwapBuffers();
    
		// Check if ESC key was pressed or window was closed
		running = !glfwGetKey( GLFW_KEY_ESC )
							&& glfwGetWindowParam( GLFW_OPENED );
	}
  
  glDeleteBuffers( 1, &vertexbuffer );
	glDeleteBuffers( 1, &vertexIntensityBuffer );
  glDeleteProgram( programID );
  glDeleteVertexArrays( 1, &vertexArrayID );
	
	glfwTerminate();

	/* Never reached. */
	return 0;
}
