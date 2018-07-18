#ifndef __SW_OGL_MAT_H__
#define __SW_OGL_MAT_H__
/*
** Module Prefix : swoglMat
**
** PROLOGUE:
**
** Provides a mechanism to specify different materials
** for rendering quads.
**
*/
#if defined( __cplusplus )
extern "C" {
#endif


/*
** PREREQUISITES
*/
#include "sw_tex.h"
//#include "ogl_sha.h"


/*
** DEFINITIONS
*/
struct _SW_OGL_MAT_S
{
    SWtexture texture;
    //SWprogram program;
};
typedef struct _SW_OGL_MAT_S SW_OGL_MAT_S;


/*
** EXPORTED FUNCTIONS
*/

    
#if defined( __cplusplus )
}
#endif

#endif /* __SW_OGL_MAT_H__ */


