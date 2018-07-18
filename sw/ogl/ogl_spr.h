#ifndef __SW_OGL_SPR_H__
#define __SW_OGL_SPR_H__
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
#include "sw_cfg.h"

#include "com_spr.h"

#include "ogl_quad.h"
#include "ogl_tran.h"
#include "ogl_mat.h"


/*
** DEFINITIONS
*/

struct _SW_OGL_SPR_S
{
    SW_OGL_QUAD_S       quad;
    SW_OGL_QUAD_S       quadTransformed;
    SW_OGL_QUAD_INFO_S  quadInfo;
    SW_OGL_MAT_S        mats;
    SW_OGL_TRANSFORM_S  transform;
};
typedef struct _SW_OGL_SPR_S SW_OGL_SPR_S;


/*
** EXPORTED VARIABLES
*/

extern SW_OGL_SPR_S sw_ogl_spr_as[ SW_SPR_MAX ];
        /*
        ** Expose the global array of OpenGL sprites for easy accesibility to
        ** any supporting modules. Direct access to this array by
        ** external modules is treated as read-only.
        **
        ** The value of the sprite handle SWsprite is a direct index
        ** into this array. The zero (0) element is treated as the
        ** null sprite. 
        **
        ** The element at index (n) in this array corresponds to the 
        ** element at index (n) in the sw_spr_as[] array defined in
        ** com_spr.h. Therefore, the complete description of the sprite
        ** is the combination of these two structures.
        */


#if defined( __cplusplus )
		}
#endif

#endif /* __SW_OGL_SPR_H__ */



