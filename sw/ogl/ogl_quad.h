#ifndef __SW_OGL_QUAD_H__
#define __SW_OGL_QUAD_H__
/*
** PROLOGUE:
**
** Contains definitions for internal data structures and routines
** for representing a quad, which is the basic display unit
** for OpenGL implementaton.
**
*/
#if defined( __cplusplus )
	extern "C" {
#endif


/*
** PREREQUISITES
*/
#include "ogl.h"

#include "sw_tex.h"
#include "sw_spr.h"
#include "sw_img.h"


/*
** DEFINITIONS
*/

struct _SW_OGL_VTX_S
{
    GLfloat pos[2];
    GLfloat tex[2];
    GLubyte clr[4];
};
typedef struct _SW_OGL_VTX_S SW_OGL_VTX_S;
        /*
        ** A quad is composed of four vertices.
        ** Each vertex contains information describing its position in
        ** two dimensions, its texture coordinate and its color.
        */

struct _SW_OGL_QUAD_S
{	
    SW_OGL_VTX_S vtx00;
    SW_OGL_VTX_S vtx10;
    SW_OGL_VTX_S vtx01;
    SW_OGL_VTX_S vtx11;
};
typedef struct _SW_OGL_QUAD_S SW_OGL_QUAD_S;

struct _SW_OGL_QUAD_INFO_S
{
    int size_w;
    int size_h;
    int l;
    int t;
    int w;
    int h;
};
typedef struct _SW_OGL_QUAD_INFO_S SW_OGL_QUAD_INFO_S;


/*
** EXPORTED FUNCTIONS
*/

void swoglQuadClear( 
        SW_OGL_QUAD_S * pquad
        );
        
        
void swoglQuadSet(
        SW_OGL_QUAD_S *      pquad,
        SW_OGL_QUAD_INFO_S * pinfo,
        SWimage              image,
        float                orig_x,
        float                orig_y,
        int                  size_x,
        int                  size_y,
        int                  resize,
        int                  flip_x,
        int                  flip_y
        );
        
        
void swoglQuadImage(
        SW_OGL_QUAD_S * pquad,
        SWimage         image,
        int             cropped
                /*
                ** Specifies whether the cropped or uncropped image
                ** dimensions are used to position the image.
                ** A non-zero value indicates that the cropped
                ** image dimensions are used.
                */
        );
        /*
        ** Set the image for the quad.
        ** Setting an image destructively modifies the 
        ** position and texture coordiates, so this should be the 
        ** first operation performed when configuring a quad.
        **
        ** The image is positioned so that it is centered on the origin,
        ** using either the cropped or uncropped image dimensions.
        **
        ** If uncropped image dimensions are used, the actual position
        ** of the image is relative to the uncropped size and the
        ** offset to the cropped image ( see sw_img description ).
        ** This means that the cropped image may no longer be
        ** centered on the origin. This will affect how the image 
        ** is scaled if you plan on applying any scaling transformations 
        ** to the quad. Therefore, only use uncropped image
        ** dimensions if you plan on positioning the quad manually.
        */
        

void swoglQuadSize(
        SW_OGL_QUAD_S * pquad,
        unsigned int    size_x,
        unsigned int    size_y
        );
        /*
        ** Changes the size of a quad.
        ** Has the effect of stretching the image if the size is larger,
        ** or squishing if the size is smaller.
        **
        ** Changes the position of the quad, but leaves the texture
        ** coordinates unchanged.
        */

        
void swoglQuadRect(
        SW_OGL_QUAD_S * pquad,
        int l,
        int t,
        unsigned int w,
        unsigned int h
        );
        /*
        ** Sets the position and dimensions for a quad using the
        ** position and dimensions passed in.        
        **
        ** Changes the position of the quad, but leaves the texture
        ** coordinates unchanged.
        */
        
        
void swoglQuadFlip(
        SW_OGL_QUAD_S * pquad,
        int 	        flip_x,  /* Non-zero to flip horizontal coords */
        int 	        flip_y   /* Non-zero to flip vertical coords   */
        );
        /*
        ** Flips the quad texture coordinates.
        */
        

void swoglQuadColorub(
        SW_OGL_QUAD_S * pquad,
        unsigned char   r,
        unsigned char   g,
        unsigned char   b,
        unsigned char   a
        );
        /*
        ** Sets the color of the vertices.
        ** Values specified as components between 0-255.
        */
        
    
void swoglQuadColorf(
        SW_OGL_QUAD_S * pquad,
        float 	        r,
        float 	        g, 
        float 	        b,
        float 	        a
        );
        /*
        ** Sets the color of the vertices.
        ** Values specified as normalized floats.
        */
    
    
void swoglQuadTintf(
        SW_OGL_QUAD_S * pquad,
        float 	        tint
        );
        /*
        ** Changes the color of the vertices as a relative
        ** value of the current values.
        */
    
    


#if defined( __cplusplus )
		}
#endif

#endif /* __SW_OGL_QUAD_H__ */



