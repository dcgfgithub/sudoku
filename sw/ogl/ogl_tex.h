#ifndef __SW_OGL_TEX_H__
#define __SW_OGL_TEX_H__
/*
** PROLOGUE:
**
** Contains definitions for internal data structures and routines
** used to support the OpenGL texture implementation.
**
*/
#if defined( __cplusplus )
	extern "C" {
#endif


/*
** PREREQUISITES
*/
#include "sw_cfg.h"
#include "ogl.h"


/*
** DEFINITIONS
*/

#define SW_OGL_TEX_MAX_FILENAME 80

struct _SW_OGL_TEX_S
{
    GLuint name;
    char   filename[SW_OGL_TEX_MAX_FILENAME];
    int    width;
    int    height;
};
typedef struct _SW_OGL_TEX_S SW_OGL_TEX_S;


/*
** EXPORTED VARIABLES
*/
extern SW_OGL_TEX_S sw_ogl_tex_as[ SW_TEX_MAX ];
        /*
        ** Export the internal array of texture structures.
        ** Direct access provides faster processing for internal
        ** modules requiring access to member variables.
        ** All direct access is treated as read-only.
        **
        ** The 0 entry is the null texture.
        */


/*
** EXPORTED FUNCTIONS
*/

extern void
swoglTexLoad(
        SW_OGL_TEX_S * ptexture,
        const char   * filename
        );
        /*
        ** Delegate function.
        ** Implemented in platform specific code module.
        **
        ** Loads a texture file from the common resources folder.
        */
        
        
extern void
swoglTexLoadUser(
        SW_OGL_TEX_S * ptexture,
        const char   * filename
        );
        /*
        ** Delegate function.
        ** Implemented in platform specific code module.
        **
        ** Loads a texture file from the user area folder.
        ** This is normally a sandbox area where users can save and
        ** load files independent of the resources folder.
        */
        
    
extern void
swoglTexLoadCompressed(
        SW_OGL_TEX_S * ptexture,
        const char   * filename
        );
        /*
        ** Delegate function.
        ** Implemented in platform specific code module.
        **
        ** Loads a compressed texture from the common resources folder.
        ** The format for the compressed texture is platform dependent.
        */
    
    
#if defined( __cplusplus )
		}
#endif

#endif /* __SW_OGL_TEX_H__ */



