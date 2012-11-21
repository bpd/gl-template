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

GLuint roomArrayID;
GLuint roomBufferID;

int roomBufferSize;

void bind_room()
{
  glGenVertexArrays( 1, &roomArrayID );
	glBindVertexArray( roomArrayID );
  
  float vertices[] = {
    // side
    -4,  2,   2,
    -4, -2,   2,
    -4, -2,  -2,
    
    -4,  2,   2,
    -4, -2,  -2,
    -4,  2,  -2,
    
    // bottom
    -4,  -2,   2,
     2,  -2,   2,
    -4,  -2,  -2,
    
     2,  -2,   2,
    -4,  -2,  -2,
     2,  -2,  -2
  };
  
  roomBufferSize = sizeof(vertices);
  
  glGenBuffers( 1, &roomBufferID );
  glBindBuffer( GL_ARRAY_BUFFER, roomBufferID );
  glBufferData( GL_ARRAY_BUFFER, roomBufferSize, &vertices[0], GL_STATIC_DRAW );
  
}

#define ATTRIB_VERTEX 0
#define ATTRIB_NORMAL 1

void render_room()
{
  glBindVertexArray( roomArrayID );
  
  glEnableVertexAttribArray( ATTRIB_VERTEX );
  glBindBuffer( GL_ARRAY_BUFFER, roomBufferID );
  glVertexAttribPointer(
    ATTRIB_VERTEX,				// attribute 0
    3,				// size
    GL_FLOAT,	// type
    GL_FALSE,	// normalized?
    0,				// stride, skip intensity
    (void*)0	// array buffer offset
  );
  
  // draw one triangle
  glDrawArrays( GL_TRIANGLES, 0, roomBufferSize ); // starting from vertex 0; 3 vertices total
    
  glDisableVertexAttribArray( ATTRIB_VERTEX );
}



GLuint vertexArrayID;
GLuint vertexbuffer;
GLuint normalbuffer;

int object_vertex_count;

void bind_object()
{
  GLfloat vertex_data[] = {
    -3, 0,  0,
     0, 0, -1,
     0, 1,  0,
     
     -3, 0, 0,
      0, 1, 0,
      0, 0, 1
  };
  
  object_vertex_count = sizeof(vertex_data) / 3;
  
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
  
  glGenVertexArrays( 1, &vertexArrayID );
	glBindVertexArray( vertexArrayID );
	
	glGenBuffers( 1, &vertexbuffer );
  glBindBuffer( GL_ARRAY_BUFFER, vertexbuffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW );
  glVertexAttribPointer(
    ATTRIB_VERTEX,				// attribute 0
    3,				// size
    GL_FLOAT,	// type
    GL_FALSE,	// normalized?
    0,				// stride, skip intensity
    (void*)0	// array buffer offset
  );
  
  glGenBuffers( 1, &normalbuffer );
  glBindBuffer( GL_ARRAY_BUFFER, normalbuffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof(normal_data), normal_data, GL_STATIC_DRAW );
  glVertexAttribPointer(
    ATTRIB_NORMAL,				// attribute 1
    3,				// size
    GL_FLOAT,	// type
    GL_FALSE,	// normalized?
    0,				// stride, skip vertex
    (void*)0	// array buffer offset
  );
  
  glBindBuffer( GL_ARRAY_BUFFER, 0 ); // deselect (bind to 0) the VBO
  glBindVertexArray( 0 );  // deselect (bind to 0) the VAO
}

void render_object()
{
  glBindVertexArray( vertexArrayID );
	
  // enable the VAAs
  glEnableVertexAttribArray( ATTRIB_VERTEX );
  glEnableVertexAttribArray( ATTRIB_NORMAL );
  
  // run the vertices through the shader
  glDrawArrays( GL_TRIANGLES, 0, object_vertex_count );
  
  // disable the VAAs
  glDisableVertexAttribArray( ATTRIB_NORMAL );
  glDisableVertexAttribArray( ATTRIB_VERTEX );
}



static glmMat4f projection;

static glmMat4f model;

static glmMat4f view;

static glmMat4f modelview;

static glmMat4f mvp;

void print( glmMat4f* m )
{
  float* mat = (float*)m;
  
  int i;
  for( i=0; i<16; i+=4 )
  {
    printf(" %f, %f, %f, %f, \n", mat[i], mat[i+1], mat[i+2], mat[i+3] );
  }
  printf("\n");
}

glmMat4f buildMatrix()
{
	// set 'mvp' uniform
	projection = glmPerspective(
		45.0f,			// horizontal Field of View (degrees), (90=wide,30=zoomed)
		4.0f / 3.0f,// Aspect Ratio
		0.1f,				// Near clipping plane, keep as big as possible
		100.0f			// Far clipping plane, keep as little as possible
	);
  
  //printf("projection:\n");
  //print(&projection);
	
	glmVec3f eye = { 0, 0, 10 };
	glmVec3f center = { 0, 0, 0 };
	glmVec3f up = { 0, 1, 0 };
	
	view = glmLookAt( eye, center, up ); //glmIdentity();
  
  //printf("view:\n");
  //print(&view);
	
	model = glmIdentity();	
	
	//model = glmMulMat4f( model, glmScalef( 0.5, 0.5, 0.5f ) );
	
	//model = glmMulMat4f( model, glmTranslatef( 0, 0, y ) );
	
	model = glmMulMat4f( model, glmRotateX( y ) );
	model = glmMulMat4f( model, glmRotateY( x ) );
	//model = glmMulMat4f( model, glmRotateZ( y ) );
  
  //printf("model:\n");
  //print(&model);
  
  glmMat4f modelview = glmMulMat4f( view, model );
  
  //printf("modelview:\n");
  //print(&modelview);
	
	mvp = glmMulMat4f( projection, modelview );
  
  //printf("mvp:\n");
  //print(&mvp);
  
  /*float mvp2[] = {
    1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, -10.0,
    0.0, 0.0, 0.0, 1.0
  };*/
  
  //print(&mvp);
  
  
	//return *((glmMat4f*)(&mvp2));
	return mvp;
}




int main( int argc, char* argv[] )
{
  int running = GL_TRUE;
	double t = 0.0;
	
  printf("Initializing window\n");

  init( 1024, 768, "Test" );
  
  // setup buffers and shaders
	glClearColor( 0.0f, 0.0f, 0.3f, 0.0f );
  
  printf("Loading shaders...");
  
  //GLuint programID = load_shader( VERTEX_SHADER, FRAGMENT_SHADER ); //load_shader_files( "vertex.v.glsl", "fragment.f.glsl" );
  GLuint programID = load_shader_files( "light.v.glsl", "light.f.glsl" );
  
  printf(" loaded program %d \n", programID);
	
  // bind VBOs
	bind_object();
  bind_room();
	
  // configure uniforms
  GLuint modelviewID = glGetUniformLocation( programID, "MV" );
	GLuint mvpUniformID = glGetUniformLocation( programID, "MVP" );
  GLuint vUniformID = glGetUniformLocation( programID, "V" );
  GLuint mUniformID = glGetUniformLocation( programID, "M" );

  glUniform3f( glGetUniformLocation( programID, "LightPosition_worldspace" ), 0, 0, 2);

	
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

    // setup GL config
		glEnable( GL_DEPTH_TEST );
		glDepthFunc( GL_LESS );
		
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    // enable the shader
    glUseProgram( programID );
    
		// provide the shader with the uniforms
		glmMat4f mvp = buildMatrix();
		
		glUniformMatrix4fv( mvpUniformID, 1, GL_TRUE, (const GLfloat*)&mvp );
    glUniformMatrix4fv( modelviewID, 1, GL_TRUE, (const GLfloat*)&modelview );
    
    glUniformMatrix4fv( mUniformID, 1, GL_TRUE, (const GLfloat*)&model );
    glUniformMatrix4fv( vUniformID, 1, GL_TRUE, (const GLfloat*)&view );
    
    // draw VBOs
    render_room();
    render_object();
		
		// Swap front and back rendering buffers
		glfwSwapBuffers();
    
		// Check if ESC key was pressed or window was closed
		running = !glfwGetKey( GLFW_KEY_ESC )
							&& glfwGetWindowParam( GLFW_OPENED );
	}
  
  // clean up object data
  glDeleteBuffers( 1, &vertexbuffer );
	glDeleteBuffers( 1, &normalbuffer );
  glDeleteVertexArrays( 1, &vertexArrayID );
  
  // clean up room data
  glDeleteBuffers( 1, &roomBufferID );
  glDeleteVertexArrays( 1, &roomArrayID );
  
  glDeleteProgram( programID );
	
	glfwTerminate();

	/* Never reached. */
	return 0;
}
