#ifndef __SW_OGL_GFX_H__
#define __SW_OGL_GFX_H__
/*
** PROLOGUE:
**
** Contains definitions for internal data structures and routines
** used to support the OpenGL implementation.
**
*/
#if defined( __cplusplus )
	extern "C" {
#endif


/*
** PREREQUISITES
*/
#include "ogl_quad.h"
#include "ogl_mat.h"


/*
** DEFINITIONS
*/


/*
** EXPORTED FUNCTIONS
*/

void
swoglGfxRender(
        SW_OGL_QUAD_S * pquad,
        SW_OGL_MAT_S  * pmat
        );


#if defined( __cplusplus )
		}
#endif

#endif /* __SW_OGL_GFX_H__ */



