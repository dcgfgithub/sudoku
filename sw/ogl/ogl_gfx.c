/*
** Module Prefix : swGfx
**
** PROLOGUE:
**
**
*/

/*
** PREREQUISITES
*/
#include <stdlib.h>
#include <assert.h>

#include "sw_cfg.h"
#include "sw_def.h"
#include "sw_gfx.h"
#include "sw_dbg.h"

#include "ogl_quad.h"
#include "ogl_tex.h"
#include "ogl_gfx.h"


/*
** DEFINITIONS
*/
#define SW_OGL_IND_PER_QUAD 6
#define SW_OGL_IND_MAX      (SW_QUAD_MAX * SW_OGL_IND_PER_QUAD)


/*
** LOCAL VARIABLES
*/
static SW_OGL_QUAD_S   ga_quad[ SW_QUAD_MAX ];
static GLushort        ga_indices[ SW_OGL_IND_MAX ];
static GLuint          gi_quads;

static SW_OGL_MAT_S    ga_mats[ SW_QUAD_MAX ];

static SW_APP_ORIENT_E gi_orient = SW_APP_ORIENT_PORTRAIT;
static float           gi_display_w = 320.0;
static float           gi_display_h = 480.0;


/*
** EXPORTED FUNCTIONS
*/

int swGfxInit( void )
{
    int i;
        
    for( i=0; i<SW_QUAD_MAX; i++ ) {
        ga_indices[i*6+0] = i*4+0;
        ga_indices[i*6+1] = i*4+1;
        ga_indices[i*6+2] = i*4+2;
        ga_indices[i*6+3] = i*4+3;
        ga_indices[i*6+4] = i*4+2;
        ga_indices[i*6+5] = i*4+1;		
    }
    return 1;    
}

void swGfxFree( void )
{
}



/*
** Manages the dimensions of the main display screen.
** Normally, the size of the display is determined by the hardware,
** or device being used, and is reported by the system.
**
** Here, we need to set up the OpenGL state variables to account
** for the dimensions. Note also that the display sizes passed here
** have taken into account the orientation of the device, which will
** be detected by the system.
*/
void swGfxSetDisplaySize( float w, float h )
{
    glViewport(0, 0, w, h);
    
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrthof(0.0f, (GLfloat)w, 0.0f, (GLfloat)h, -1.0f, 1.0f);
    
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    
	glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        
    gi_display_w = w;
    gi_display_h = h;
}

float swGfxGetDisplayW( void )
{
    return gi_display_w;
}
float swGfxGetDisplayH( void )
{
    return gi_display_h;
}


/*
** Manages the current orientation of the device.
** Setting the orientation is normally done in response to 
** a call to swAppOrientBegin, or swAppOrientEnd.
**
** Currently, the orientation is only stored as a state value,
** and no additional processing is performed when the orientation
** changes. Instead, all necessary changes are performed inside
** the swGfxSetDisplaySize, which accompanies any orientation changes.
*/
void swGfxSetOrientation( SW_APP_ORIENT_E orient )
{
    switch( orient )
    {
        case SW_APP_ORIENT_PORTRAIT_INV:  
        case SW_APP_ORIENT_LANDSCAPE_LEFT:            
        case SW_APP_ORIENT_LANDSCAPE_RIGHT:
            gi_orient = orient;
            break;            
            
        default:
            gi_orient = SW_APP_ORIENT_PORTRAIT;
            break;
    }
}

SW_APP_ORIENT_E swGfxGetOrientation( void )
{
    return gi_orient;
}



void swGfxRenderBegin( void )
{
	gi_quads = 0;    
}

void swGfxRenderEnd( void )
{
    if( gi_quads <= 0 )
        return;
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    
    glVertexPointer  ( 2, GL_FLOAT,         sizeof(SW_OGL_VTX_S), &(ga_quad[0].vtx00.pos)); 
    glTexCoordPointer( 2, GL_FLOAT,         sizeof(SW_OGL_VTX_S), &(ga_quad[0].vtx00.tex));
    glColorPointer   ( 4, GL_UNSIGNED_BYTE, sizeof(SW_OGL_VTX_S), &(ga_quad[0].vtx00.clr));
    
    int first = 0;
    int count = 0;
    
    while( first < gi_quads )
    {
        SWtexture texture = ga_mats[first].texture;
             
        while( first+count < gi_quads && ga_mats[first+count].texture == texture ) count++;
        
        glBindTexture ( GL_TEXTURE_2D, sw_ogl_tex_as[texture].name );
        glDrawElements( GL_TRIANGLES, count*SW_OGL_IND_PER_QUAD, GL_UNSIGNED_SHORT, ga_indices + (first*SW_OGL_IND_PER_QUAD));
                                
        first = first + count;
        count = 0;
    } 
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);            
}


void swGfxRenderFlush( void )
{
    swGfxRenderEnd();
    swGfxRenderBegin();
}

void swGfxClip( int enabled )
{
    swGfxRenderFlush();
    
    if( enabled )
        glEnable( GL_SCISSOR_TEST );
    else
        glDisable( GL_SCISSOR_TEST );
}

void swGfxClipRect( float l, float b, float w, float h )
{
    glScissor( l, b, w, h );
}

void
swoglGfxRender(
    SW_OGL_QUAD_S * pquad,
    SW_OGL_MAT_S  * pmat
    )
{
    assert( pquad != NULL );
    assert( pmat  != NULL );
    
    if( gi_quads >= SW_QUAD_MAX )
    {
        swGfxRenderFlush();
    }
    
    ga_quad[ gi_quads ] = *pquad;
    ga_mats[ gi_quads ] = *pmat;
        
    gi_quads++;
}




