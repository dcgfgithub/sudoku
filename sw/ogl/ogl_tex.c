/*
** PROLOGUE:
**
** Implements the texture functionality.
** Textures in this context act as a wrapper around an OpenGL
** texture, and add a few platform dependant routines, such
** as loading a texture from a file.
*/

/*
** PREREQUISITES
*/
#include <stdlib.h>
#include <assert.h>

#include "sw_cfg.h"
#include "sw_dbg.h"
#include "sw_tex.h"

#include "ogl_tex.h"


/*
** EXPORTED VARIABLES
*/

SW_OGL_TEX_S sw_ogl_tex_as[ SW_TEX_MAX ] = { 0 };


/*
** EXPORTED FUNCTIONS
*/

int swTexInit( void )
{
    /*
    ** No initialization is required as zeroing texture
    ** array is sufficient.
    */
    SW_INFOF( "swTexInit() - Maximum textures : %d", SW_TEX_MAX );
    return 1;
}


void swTexFree( void )
{
	int tx;
    /*
    ** As a precaution when shutting down, check to see
    ** if any textures are still allocated.
    */
    SW_INFO( "swTexFree()" );
    
    for( tx=1; tx<SW_TEX_MAX; tx++ )
    {
        if( sw_ogl_tex_as[tx].name != 0 )
        {
            SW_WARNF( "swTexFree() - Texture %d was not properly released", tx );
            glDeleteTextures(1, &sw_ogl_tex_as[tx].name);
            sw_ogl_tex_as[tx].name = 0;            
        }
    }
}


int swTexGen( int num_textures, SWtexture * textures )
{
    /*
    ** Generate one or more new texture handles.
    ** Return the number of actual textures allocated, which
    ** may be less than the number requested if there are
    ** not enough texture spots.
    */
    int tex, i;
    GLuint tex_name;

    assert( num_textures >= 0 );
    assert( textures != NULL );

    for( tex=0; tex<num_textures; tex++ )
    {
        textures[tex] = 0;

        for( i=1; i<SW_TEX_MAX; i++ )
        {
            /*
            ** Order is not important, so use first free
            ** texture space.
            */
            if( sw_ogl_tex_as[i].name == 0 )
                break;
        }
        if( i >= SW_TEX_MAX )
        {
            /*
            ** If index passes maximum allowed, all
            ** available textures are used.
            */
            SW_ERR( "swTexGen() - Textures exhausted!" );
            return tex;
        }
        glGenTextures(1, &tex_name);
        
        if( tex_name == 0 )
        {
            SW_ERR( "swTexGen() - glGenTextures failed" );
        }

        sw_ogl_tex_as[i].name   = tex_name;
        sw_ogl_tex_as[i].width  = 0;
        sw_ogl_tex_as[i].height = 0;
        
        textures[tex] = i;
    }
    return tex;
}


void swTexDel( int num_textures, SWtexture * textures )
{
    /*
    ** Frees previously allocated texture handles.
    ** Safe to pass in null or 0 handle.
    */
    int tex;

    assert( textures != NULL );
    assert( num_textures >= 0 );
    
    for( tex=0; tex<num_textures; tex++ )
    {
        SWtexture t = textures[tex];
        
        if( t > 0 && t < SW_TEX_MAX )
        {
            SW_INFOF( "swTexDel() - %d", t );
            glDeleteTextures(1, &sw_ogl_tex_as[t].name);
            sw_ogl_tex_as[t].filename[0] = 0;
            sw_ogl_tex_as[t].name = 0;
        }
    }
}


void swTexCreate( SWtexture texture, int width, int height )
{
	GLubyte * tex_data;
    
    assert( texture < SW_TEX_MAX );
    assert( texture > 0 );
    assert( width   > 0 );
    assert( height  > 0 );
    
    SW_OGL_TEX_S * ptexture = &sw_ogl_tex_as[ texture ];
    
    tex_data = (GLubyte *)calloc(width * height * 4, sizeof(GLubyte));
    
    glBindTexture  ( GL_TEXTURE_2D, ptexture->name );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D   ( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data );
    
    free( tex_data );
    
	SW_INFOF( "swTexCreate() - %d (%d) : (%d, %d)", texture, sw_ogl_tex_as[texture].name, width, height );    
    
    ptexture->width  = width;
    ptexture->height = height;
}


void swTexLoad( SWtexture texture, const char* filename )
{
    assert( filename != NULL );
    assert( texture < SW_TEX_MAX );
    
    if( texture != 0 )
    {
        SW_INFOF( "swTexLoad() - %d (%d) : %s", texture, sw_ogl_tex_as[texture].name, filename );
        
        swoglTexLoad( &sw_ogl_tex_as[texture], filename );
    }
}


void swTexLoadUser( SWtexture texture, const char* filename )
{
    assert( filename != NULL );
    assert( texture < SW_TEX_MAX );
    
    if( texture != 0 )
        swoglTexLoadUser( &sw_ogl_tex_as[texture], filename );
}


void swTexLoadCompressed( SWtexture texture, const char* filename )
{
    assert( filename != NULL );
    assert( texture < SW_TEX_MAX );
    
    if( texture != 0 )
        swoglTexLoadCompressed( &sw_ogl_tex_as[texture], filename );
}


int swTexGetWidth ( SWtexture texture )
{
	return sw_ogl_tex_as[texture].width;
}

int swTexGetHeight( SWtexture texture )
{
	return sw_ogl_tex_as[texture].height;
}





