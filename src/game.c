
#include "game.h"

static Ship ship;



void quit_game( int code )
{
    /*
     * Quit SDL so we can release the fullscreen
     * mode and restore the previous video settings,
     * etc.
     */
    SDL_Quit( );

    /* Exit program. */
    exit( code );
}

static void on_key_down( SDL_keysym* keysym )
{
	switch( keysym->sym )
	{
	case SDLK_ESCAPE:
		quit_game( 0 );
		break;

	case SDLK_SPACE:


	case SDLK_LEFT:
		ship.a.x = -5;
		break;
	case SDLK_RIGHT:
		ship.a.x = 5;
		break;

	case SDLK_UP:
		ship.engine_firing = 1;
		break;
	}
}

static void on_key_up( SDL_keysym* keysym )
{
	switch( keysym->sym )
	{
	case SDLK_LEFT:
	case SDLK_RIGHT:
		ship.a.x = 0;
		//ship.v.x = 0;
		break;

	case SDLK_UP:
		ship.engine_firing = 0;
		break;
	}
}

static void process_events( void )
{
    /* Our SDL event placeholder. */
    SDL_Event event;

    /* Grab all the events off the queue. */
    while( SDL_PollEvent( &event ) )
	{
		SDL_keysym* keysym = &event.key.keysym;

        switch( event.type )
		{
        case SDL_KEYDOWN:
            /* Handle key presses. */
			on_key_down( keysym );

            break;

		case SDL_KEYUP:
            /* Handle key presses. */
            on_key_up( keysym );
            break;
        case SDL_QUIT:
            /* Handle quit requests (like Ctrl-c). */
            quit_game( 0 );
            break;
        }
    }
}

static void setup_physics()
{
	ship.a = GRAVITY;
	ship.v = ZERO_VECTOR;
	ship.m = 20.0f;

	ship.x = 50;
	ship.y = 300;
	ship.width = 10;
	ship.height = 10;
}

static void tick_particle( Particle* p, long double dt )
{
	// a(t) = -9.8
	// v(t) = -9.8t + vi
	// y(t) = -4.9*t^2 + vi*t + yi
	
	/*  compute y position */
	p->v.y += p->a.y*dt;
	p->y += p->v.y*dt;
	

	/* compute x position */
	p->v.x += p->a.x*dt;
	p->x += p->v.x*dt;
	
	
	/* bound the particle to the screen bounds */
	if( p->y < p->height )
	{
		p->y = p->height;
		p->v.y = 0.0f;
	}
	else if( p->y > screen_height )
	{
		p->y = screen_height;
		p->v.y = 0.0f;
	}

	if( p->x < 0 )
	{
		p->x = 0;
		p->v.x = 0.0f;
	}
	else if( p->x + p->width > screen_width )
	{
		p->x = screen_width - p->width;
		p->v.x = 0.0f;
	}
}

static const double FRICTION = -0.01f;

static void tick( Uint32 dt_ms )
{
	long double dt = ((long double)dt_ms) / 1000.0f; // convert ms to s

	long double forces;
	long double gravity = ship.m * -9.8f;
	long double friction = 0.0f; //FRICTION * ship.v.y;

	long double engines = 0.0f;
	if( ship.engine_firing == 1 )
	{
		engines = -gravity * 2.0f;	// give the engines just enough thrust
									// to overcome gravity and friction
	}
	
	forces = gravity + friction + engines;

	// F = ma
	// a = F/m
	//
	ship.a.y = forces / ship.m;

	tick_particle( &ship, dt );

	//printf("forces: %f  a_y: %f  v_y: %f  y: %f \n", forces, ship.a.y, ship.v.y, ship.y);
}

static void render( Particle* p )
{
	glBegin(GL_QUADS);
	glVertex2d( p->x, p->y );
	glVertex2d( p->x + p->width, p->y );
	glVertex2d( p->x + p->width, p->y - p->height );
	glVertex2d( p->x, p->y - p->height );
	glEnd();
}


static void draw_screen( void )
{
	glClear(GL_COLOR_BUFFER_BIT);

	if( ship.engine_firing )
	{
		glColor3f( 1.0f, 0.0f, 0.0f );
	}
	else
	{
		glColor3f( 0.0f, 0.0f, 0.0f );
	}

	render( (Particle*)&ship );

    /*
     * Swap the buffers. This this tells the driver to
     * render the next frame from the contents of the
     * back-buffer, and to set all rendering operations
     * to occur on what was the front-buffer.
     *
     * Double buffering prevents nasty visual tearing
     * from the application drawing on areas of the
     * screen that are being updated at the same time.
     */
    SDL_GL_SwapBuffers( );
}

static void setup_opengl()
{
    /* Our shading model--Gouraud (smooth). */
    glShadeModel( GL_SMOOTH );

    glClearColor( 1, 1, 1, 0.0 );

	glDisable( GL_DEPTH_TEST );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluOrtho2D( 0, screen_width, 0, screen_height );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();	  
}



void game_main_loop()
{
	Uint32 current_time, last_render_time, next_render_time;

	/*
     * At this point, we should have a properly setup
     * double-buffered window for use with OpenGL.
     */
    setup_opengl( );
	setup_physics( );
	
	current_time = SDL_GetTicks();
	last_render_time = current_time;
	next_render_time = current_time;

    /*
     * Now we want to begin our normal app process--
     * an event loop with a lot of redrawing.
     */
    while( 1 )
	{
		/* Process incoming events. */
		process_events( );

		current_time = SDL_GetTicks();

		if( current_time >= next_render_time )
		{
			Uint32 dt = current_time - last_render_time;

			/* advance animation */
			tick( dt );

			/* Draw the screen. */
			draw_screen( );

			last_render_time = current_time;
			next_render_time = last_render_time + MS_BETWEEN_RENDERS;

		}
		else
		{
			Uint32 delay_ms = next_render_time - current_time;
			SDL_Delay( delay_ms );
		}
		
    }
}