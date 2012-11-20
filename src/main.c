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
"#version 330 core                                         \n"

" uniform mat4 MVP;                                        "
" uniform mat4 modelview;                                  "
" uniform vec3 lightPos;                     "

// Input vertex data, different for all executions of this shader.
" layout(location = 0) in vec3 vertexPosition_modelspace;  "
" layout(location = 1) in vec3 normal;                     "

" out vec3 fragNormal; "
" out vec3 fragPosition; "

" void main()                                              "
" {                                                        "
"   mat3 normalmatrix = mat3(modelview); "

" fragNormal = normal; "
//" fragNormal = normalmatrix * normalize(normal); "
" fragPosition = vertexPosition_modelspace; "

"gl_Position = MVP * vec4( vertexPosition_modelspace, 1 ); "

" } "
;

static const char* FRAGMENT_SHADER = 

"#version 330 core                \n"

" uniform vec3 lightPos;                     "

"in vec3 fragNormal; "
"in vec3 fragPosition; "

"out vec4 color;                  "

"void main()                      "
"{                                "
"	// Output color = red           \n"
"	color = vec4(1,0,0, 1);         "
""
" vec3 lightDir = normalize(lightPos - fragPosition);   "
" float diffuseLightIntensity = max(0, dot(normalize(fragNormal), lightDir)); "
" color = vec4(diffuseLightIntensity * color.rgb, color.a); "
" color.rgb += vec3(0.2,0.2,0.2); "

//" color = color * 0.3 + color * max(dot(fragNormal, lightPos), 0.0); "

" }                               "
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
  printf( "OpenGL version: %d.%d.%d \n", major, minor, rev );
  
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

static glmMat4f projection;

static glmMat4f modelview;

static glmMat4f mvp;

glmMat4f buildMatrix()
{
	// set 'mvp' uniform
	projection = glmPerspective(
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
	
	model = glmMulMat4f( model, glmRotateX( y ) );
	model = glmMulMat4f( model, glmRotateY( x ) );
	//model = glmMulMat4f( model, glmRotateZ( y ) );
  
  glmMat4f modelview = glmMulMat4f( view, model );
	
	mvp = glmMulMat4f( projection, modelview );
	
	return mvp;
}

#define ATTRIB_VERTEX 0
#define ATTRIB_NORMAL 1

int main( int argc, char* argv[] )
{
  int running = GL_TRUE;
	double t = 0.0;
	
	GLuint vertexArrayID;
	GLuint vertexbuffer;
  GLuint normalbuffer;
	//GLuint vertexIntensityBuffer;
  
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
	
	GLfloat vertex_data[] = {
    -3, 0,  0,
     0, 0, -1,
     0, 1,  0,
     
     -3, 0, 0,
      0, 1, 0,
      0, 0, 1
      
  };
  // triangle normal = vector cross product of two edges of the triangle
  glmVec3f norm1 = glmCross( (glmVec3f){ vertex_data[0], vertex_data[1], vertex_data[2] }, 
                             (glmVec3f){ vertex_data[3], vertex_data[4], vertex_data[5] } );

  glmVec3f norm2 = glmCross( (glmVec3f){ vertex_data[9], vertex_data[10], vertex_data[11] }, 
                             (glmVec3f){ vertex_data[12], vertex_data[13], vertex_data[14] } );
  
  GLfloat normal_data[] = {
    norm1.x, norm1.y, norm1.z,
    norm1.x, norm1.y, norm1.z,
    norm1.x, norm1.y, norm1.z,
    
    norm2.x, norm2.y, norm2.z,
    norm2.x, norm2.y, norm2.z,
    norm2.x, norm2.y, norm2.z
  };
 
  printf("Generating buffers\n");
	
	glGenBuffers( 1, &vertexbuffer );
  glBindBuffer( GL_ARRAY_BUFFER, vertexbuffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW );
  
  glGenBuffers( 1, &normalbuffer );
  glBindBuffer( GL_ARRAY_BUFFER, normalbuffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof(normal_data), normal_data, GL_STATIC_DRAW );
	

  GLuint modelviewID = glGetUniformLocation( programID, "modelview" );
  
	GLuint mvpUniformID = glGetUniformLocation( programID, "MVP" );
  
  GLuint lightPosID = glGetUniformLocation( programID, "lightPos" );
  
  glUniform3f(lightPosID, 0, 0, 0);
	
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
    
    //printf("Rendering\n");
    
    // TODO: only render at a specific frame rate

    // setup GL config
		glEnable( GL_DEPTH_TEST );
		glDepthFunc( GL_LESS );
		
		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		
		// allow shaders to adjust point size
		//glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
		
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    glUseProgram( programID );
    
		// set 'offset' uniform
    //glUniform1f( thresholdUniformID, y );
		
		glmMat4f mvp = buildMatrix();
		
		glUniformMatrix4fv( mvpUniformID, 1, GL_TRUE, (const GLfloat*)&mvp );
    
    glUniformMatrix4fv( modelviewID, 1, GL_TRUE, (const GLfloat*)&modelview );
	
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
		
		glEnableVertexAttribArray( ATTRIB_NORMAL );
		glBindBuffer( GL_ARRAY_BUFFER, normalbuffer );
		glVertexAttribPointer(
      ATTRIB_NORMAL,				// attribute 1
      3,				// size
      GL_FLOAT,	// type
      GL_FALSE,	// normalized?
      0,				// stride, skip vertex
      (void*)0	// array buffer offset
    );
		
		
		
    
    // run the vertices through the shader
    glDrawArrays( GL_TRIANGLES, 0, sizeof(vertex_data) / 3 ); // starting from vertex 0; 3 vertices total
    
		glDisableVertexAttribArray( ATTRIB_NORMAL );
    glDisableVertexAttribArray( ATTRIB_VERTEX );
		
		// Swap front and back rendering buffers
		glfwSwapBuffers();
    
		// Check if ESC key was pressed or window was closed
		running = !glfwGetKey( GLFW_KEY_ESC )
							&& glfwGetWindowParam( GLFW_OPENED );
	}
  
  glDeleteBuffers( 1, &vertexbuffer );
	glDeleteBuffers( 1, &normalbuffer );
  glDeleteProgram( programID );
  glDeleteVertexArrays( 1, &vertexArrayID );
	
	glfwTerminate();

	/* Never reached. */
	return 0;
}
