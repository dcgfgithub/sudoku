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
#include <string.h>

#include "sw_dbg.h"
#include "sw_tex.h"

#include "app_jni.h"
#include "ogl_tex.h"


/*
** EXPORTED FUNCTIONS
*/

void swoglTexLoad( SW_OGL_TEX_S * ptexture, const char * filename )
{
    int w, h;
    
	app_jni_LoadTexture( ptexture->name, filename, &w, &h );
			
    ptexture->width  = w;
    ptexture->height = h;
        
	strncpy( ptexture->filename, filename, SW_OGL_TEX_MAX_FILENAME-1 );    
}

void swoglTexLoadUser( SW_OGL_TEX_S * ptexture, const char * filename )
{
}

void swoglTexLoadCompressed( SW_OGL_TEX_S * ptexture, const char * filename )
{
}

void swoglTexReload()
{
	for( int tex=1; tex<SW_TEX_MAX; tex++ )
	{
		if( sw_ogl_tex_as[tex].name != 0 )
		{
			GLuint tex_name;
			
			glGenTextures( 1, &tex_name );
			sw_ogl_tex_as[tex].name = tex_name;			
			
			if( sw_ogl_tex_as[tex].filename[0] != 0 )
			{					
				swTexLoad( tex, 
						sw_ogl_tex_as[tex].filename 
						); 
			}
			else
			{
				swTexCreate( tex, 
						sw_ogl_tex_as[tex].width, 
						sw_ogl_tex_as[tex].height 
						);
			}
		}
	}
}
