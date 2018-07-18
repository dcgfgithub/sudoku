#ifndef __SW_OGL_IMG_H__
#define __SW_OGL_IMG_H__
/*
** PROLOGUE:
**
** OpenGL implementation for images.
** 
*/
#if defined( __cplusplus )
	extern "C" {
#endif


/*
** PREREQUISITES
*/
#include "ogl.h"
#include "sw_cfg.h"
#include "sw_tex.h"


/*
** DEFINITIONS
*/

struct _SW_OGL_IMG_S
{	
    int		    used;
    SWtexture  	texture;
    GLushort    osz[2];     /* Uncropped image dimensions */
    GLushort    ofs[2];     /* Offset to top left corner of cropped image */
    GLushort    tex00[2];
    GLushort    tex01[2];
    GLushort    tex10[2];
    GLushort    tex11[2];
};
typedef struct _SW_OGL_IMG_S SW_OGL_IMG_S;
        /*
        ** To save memory, only the uncropped image dimensions are stored.
        ** The cropped dimensions can be determined by
        ** the texture coordinates.
        */

/*
** EXPORTED VARIABLES
*/

extern SW_OGL_IMG_S sw_ogl_img_as[ SW_IMG_MAX ];
        /*
        ** Exported global array holding the definitions for every
        ** image. Allows quick access to image members for other internal
        ** modules. Direct access to member variables should always
        ** be treated as read-only.
        **
        ** There is a one-to-one correspondance with the SWimage 
        ** value and the index into this array. The element at index 0
        ** is the null image.
        */


/*
** EXPORTED FUNCTIONS
*/


#if defined( __cplusplus )
		}
#endif

#endif /* __SW_OGL_IMG_H__ */



