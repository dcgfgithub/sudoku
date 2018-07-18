/*
** Module Prefix : dcgDraw
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

#include "sw_def.h"
#include "sw_gfx.h"

#include "ogl_quad.h"
#include "ogl_img.h"


/*
** DEFINITIONS
*/

/*
** EXPORTED FUNCTIONS
*/

void swoglQuadClear( SW_OGL_QUAD_S * pquad )
{
	assert( pquad != NULL );
        
    pquad->vtx00.pos[0] = 0.0;
    pquad->vtx00.pos[1] = 0.0;
    pquad->vtx00.tex[0] = 0.0;
    pquad->vtx00.tex[1] = 0.0;
    pquad->vtx00.clr[0] = 0;
    pquad->vtx00.clr[1] = 0;
    pquad->vtx00.clr[2] = 0;
    pquad->vtx00.clr[3] = 0;
    
    pquad->vtx01 = pquad->vtx00;
    pquad->vtx10 = pquad->vtx00;
    pquad->vtx11 = pquad->vtx00;
}

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
    )
{
	assert( pquad != NULL );
    
    SW_OGL_IMG_S ogl_img = sw_ogl_img_as[ image ];
    
    int iw = swImgGetWidth (image);
    int ih = swImgGetHeight(image);
    
    int iw2 = iw >> 1;
    int ih2 = ih >> 1;
    
    int ow = ogl_img.osz[0];
    int oh = ogl_img.osz[1];
    
    int ow2 = ow >> 1;
    int oh2 = oh >> 1;
    
    if( size_x == 0 ) size_x = ow;
    if( size_y == 0 ) size_y = oh;
    
    int size_x2 = size_x >> 1;
    int size_y2 = size_y >> 1;
    
    int ox = ogl_img.ofs[0];
    int oy = ogl_img.ofs[1];
    
    if( iw == ow && ih == oh )
    {
        /*
        ** If the cropped and uncropped image sizes are the same, 
        ** it is not necessary to realign the offset.
        */
        ox = 0;
        oy = 0;
        
        if( resize )
        {
            iw = size_x;
            ih = size_y;
        }
    }
    else
    {
        /*
        ** The cropped and uncropped sizes are different, so we need to
        ** reposition the cropped image inside the uncropped region.
        **
        ** To do this, find the middle point of the uncropped image, and
        ** normalize it between -1 and 1 in both dimensions, then 
        ** use that to find the position in the new size.
        */
        ox = ox + iw2;
        oy = oy + ih2;
        
        float nx = (float)(ox - ow2) / (float)ow2;
        float ny = (float)(oy - oh2) / (float)oh2;
        
        nx = (nx * size_x2) + size_x2;
        ny = (ny * size_y2) + size_y2;
        
        float sx = (float)size_x / (float)ow;
        float sy = (float)size_y / (float)oh;
        
        if( resize != 0 )
        {
            iw = SW_UPPER(((float)iw * sx));
            ih = SW_UPPER(((float)ih * sy));
        }
        else
        {
            float scl = SW_MIN( sx, sy );
            
            iw = SW_UPPER(((float)iw * scl));
            ih = SW_UPPER(((float)ih * scl));
        }
        
        iw2 = iw >> 1;
        ih2 = ih >> 1;
        
        ox = SW_UPPER(nx) - iw2;
        oy = SW_UPPER(ny) - ih2;
    }
    
    if( flip_x == 0 )
        pquad->vtx01.pos[0] = (orig_x - size_x2) + ox;
    else
        pquad->vtx01.pos[0] = (orig_x + size_x2) - (ox+iw);
    
    if( flip_y == 0 )
        pquad->vtx01.pos[1] = (orig_y + size_y2) - oy;
    else
        pquad->vtx01.pos[1] = (orig_y - size_y2) + (oy+ih);
    
    pquad->vtx11.pos[0] = pquad->vtx01.pos[0] + iw;
    pquad->vtx11.pos[1] = pquad->vtx01.pos[1];
    pquad->vtx00.pos[0] = pquad->vtx01.pos[0];
    pquad->vtx00.pos[1] = pquad->vtx01.pos[1] - ih;
    pquad->vtx10.pos[0] = pquad->vtx11.pos[0];
    pquad->vtx10.pos[1] = pquad->vtx00.pos[1];
    
    int tw = swTexGetWidth (ogl_img.texture);
    int th = swTexGetHeight(ogl_img.texture);
    
    pquad->vtx00.tex[0] = ((float)ogl_img.tex01[0] / tw);
    pquad->vtx00.tex[1] = ((float)ogl_img.tex01[1] / th);
    pquad->vtx10.tex[0] = ((float)ogl_img.tex11[0] / tw);
    pquad->vtx10.tex[1] = ((float)ogl_img.tex11[1] / th);
    pquad->vtx01.tex[0] = ((float)ogl_img.tex00[0] / tw);
    pquad->vtx01.tex[1] = ((float)ogl_img.tex00[1] / th);
    pquad->vtx11.tex[0] = ((float)ogl_img.tex10[0] / tw);
    pquad->vtx11.tex[1] = ((float)ogl_img.tex10[1] / th);
    
    if( pinfo )
    {
        pinfo->size_w = size_x;
        pinfo->size_h = size_y;
        pinfo->l      = ox;
        pinfo->t      = oy;
        pinfo->w      = iw;
        pinfo->h      = ih;
    }
}


void
swoglQuadImage(
    SW_OGL_QUAD_S * pquad,
    SWimage         image,
    int             cropped
    )
{
	assert( pquad != NULL );
    
    int iw;
    int ih;
    int ox;
    int oy;
    
    SW_OGL_IMG_S ogl_img = sw_ogl_img_as[ image ];
    
    if( cropped != 0 )
    {
        iw = swImgGetWidth ( image );
        ih = swImgGetHeight( image );
        ox = 0;
        oy = 0;
    }
    else
    {
        iw = swImgGetUncroppedWidth ( image );
        ih = swImgGetUncroppedHeight( image );
        ox = ogl_img.ofs[0];
        oy = ogl_img.ofs[1];
    }
    
    pquad->vtx01.pos[0] = 0 - (iw * 0.5f) + ox;
    pquad->vtx01.pos[1] = 0 + (ih * 0.5f) - oy;
    
    pquad->vtx11.pos[0] = pquad->vtx01.pos[0] + iw;
    pquad->vtx11.pos[1] = pquad->vtx01.pos[1];
    pquad->vtx00.pos[0] = pquad->vtx01.pos[0];
    pquad->vtx00.pos[1] = pquad->vtx01.pos[1] - ih;
    pquad->vtx10.pos[0] = pquad->vtx11.pos[0];
    pquad->vtx10.pos[1] = pquad->vtx00.pos[1];
    
    iw = swTexGetWidth (ogl_img.texture);
    ih = swTexGetHeight(ogl_img.texture);
    
    pquad->vtx00.tex[0] = ((float)ogl_img.tex01[0] / iw);
    pquad->vtx00.tex[1] = ((float)ogl_img.tex01[1] / ih);
    pquad->vtx10.tex[0] = ((float)ogl_img.tex11[0] / iw);
    pquad->vtx10.tex[1] = ((float)ogl_img.tex11[1] / ih);
    pquad->vtx01.tex[0] = ((float)ogl_img.tex00[0] / iw);
    pquad->vtx01.tex[1] = ((float)ogl_img.tex00[1] / ih);
    pquad->vtx11.tex[0] = ((float)ogl_img.tex10[0] / iw);
    pquad->vtx11.tex[1] = ((float)ogl_img.tex10[1] / ih);
}


void swoglQuadRect(
    SW_OGL_QUAD_S * pquad,
    int l,
    int b,
    unsigned int w,
    unsigned int h
    )
{
    /*
    ** Sets the position values for a quad using the 
    ** dimensions of the passed in rectangle.
    */
	assert( pquad != NULL );
    
    pquad->vtx00.pos[0] = l;
    pquad->vtx00.pos[1] = b;
    pquad->vtx10.pos[0] = pquad->vtx00.pos[0] + w;
    pquad->vtx10.pos[1] = pquad->vtx00.pos[1];
    pquad->vtx01.pos[0] = pquad->vtx00.pos[0];
    pquad->vtx01.pos[1] = pquad->vtx00.pos[1] + h;
    pquad->vtx11.pos[0] = pquad->vtx10.pos[0];
    pquad->vtx11.pos[1] = pquad->vtx01.pos[1];
}


void swoglQuadFlip(
    SW_OGL_QUAD_S * pquad,
    int flip_x, 
    int flip_y    
    )
{
    /*
    ** Flips the image by swapping the texture coordinates
    ** in either/both the u and v values.
    */

    float tmp;
    
    if( flip_x )
    {
        tmp = pquad->vtx00.tex[0];
        pquad->vtx00.tex[0] = pquad->vtx10.tex[0];
        pquad->vtx10.tex[0] = tmp;
        
        tmp = pquad->vtx01.tex[0];
        pquad->vtx01.tex[0] = pquad->vtx11.tex[0];
        pquad->vtx11.tex[0] = tmp;
    }
    if( flip_y )
    {
        tmp = pquad->vtx00.tex[1];
        pquad->vtx00.tex[1] = pquad->vtx01.tex[1];
        pquad->vtx01.tex[1] = tmp;
        
        tmp = pquad->vtx10.tex[1];
        pquad->vtx10.tex[1] = pquad->vtx11.tex[1];
        pquad->vtx11.tex[1] = tmp;
    }
}


void swoglQuadColorub(
	SW_OGL_QUAD_S * pquad,
    unsigned char r,
    unsigned char g,
    unsigned char b,
    unsigned char a
    )
{
    /*
    ** Sets the color value for all vertices of a quad.
    ** Color values are clamped between 0 and 255.
    */

    assert( pquad != NULL );
    
	pquad->vtx00.clr[0] = SW_CLAMP(r, 0, 255);
	pquad->vtx00.clr[1] = SW_CLAMP(g, 0, 255);
	pquad->vtx00.clr[2] = SW_CLAMP(b, 0, 255);
    pquad->vtx00.clr[3] = SW_CLAMP(a, 0, 255);
    
    pquad->vtx01.clr[0] = pquad->vtx00.clr[0];
    pquad->vtx01.clr[1] = pquad->vtx00.clr[1];
    pquad->vtx01.clr[2] = pquad->vtx00.clr[2];
    pquad->vtx01.clr[3] = pquad->vtx00.clr[3];

    pquad->vtx10.clr[0] = pquad->vtx00.clr[0];
    pquad->vtx10.clr[1] = pquad->vtx00.clr[1];
    pquad->vtx10.clr[2] = pquad->vtx00.clr[2];
    pquad->vtx10.clr[3] = pquad->vtx00.clr[3];

    pquad->vtx11.clr[0] = pquad->vtx00.clr[0];
    pquad->vtx11.clr[1] = pquad->vtx00.clr[1];
    pquad->vtx11.clr[2] = pquad->vtx00.clr[2];
    pquad->vtx11.clr[3] = pquad->vtx00.clr[3];
}


void swoglQuadColorf(
    SW_OGL_QUAD_S * pquad,
    float r,
    float g, 
    float b,
    float a
    )
{
    /*
    ** Sets the color value for all vertices of a quad.
    ** Color values are clamped between 0 and 1.
    */

    assert( pquad != NULL );
    
	pquad->vtx00.clr[0] = SW_CLAMP(r, 0.0, 1.0) * 255;
	pquad->vtx00.clr[1] = SW_CLAMP(g, 0.0, 1.0) * 255;
	pquad->vtx00.clr[2] = SW_CLAMP(b, 0.0, 1.0) * 255;
    pquad->vtx00.clr[3] = SW_CLAMP(a, 0.0, 1.0) * 255;
    
    pquad->vtx01.clr[0] = pquad->vtx00.clr[0];
    pquad->vtx01.clr[1] = pquad->vtx00.clr[1];
    pquad->vtx01.clr[2] = pquad->vtx00.clr[2];
    pquad->vtx01.clr[3] = pquad->vtx00.clr[3];
    
    pquad->vtx10.clr[0] = pquad->vtx00.clr[0];
    pquad->vtx10.clr[1] = pquad->vtx00.clr[1];
    pquad->vtx10.clr[2] = pquad->vtx00.clr[2];
    pquad->vtx10.clr[3] = pquad->vtx00.clr[3];
    
    pquad->vtx11.clr[0] = pquad->vtx00.clr[0];
    pquad->vtx11.clr[1] = pquad->vtx00.clr[1];
    pquad->vtx11.clr[2] = pquad->vtx00.clr[2];
    pquad->vtx11.clr[3] = pquad->vtx00.clr[3];
}


void swoglQuadTintf(
    SW_OGL_QUAD_S * pquad,
    float a
    )
{
    /*
    ** Changes the vertex color by tinting, or multiplying
    ** the current color by the given tint factor.
    */
    assert( pquad != NULL );
    
    float tint = SW_CLAMP(a, 0.0, 1.0);
    
	pquad->vtx00.clr[0] *= tint;
	pquad->vtx00.clr[1] *= tint;
	pquad->vtx00.clr[2] *= tint;
    pquad->vtx00.clr[3] *= tint;
    
    pquad->vtx01.clr[0] = pquad->vtx00.clr[0];
    pquad->vtx01.clr[1] = pquad->vtx00.clr[1];
    pquad->vtx01.clr[2] = pquad->vtx00.clr[2];
    pquad->vtx01.clr[3] = pquad->vtx00.clr[3];
    
    pquad->vtx10.clr[0] = pquad->vtx00.clr[0];
    pquad->vtx10.clr[1] = pquad->vtx00.clr[1];
    pquad->vtx10.clr[2] = pquad->vtx00.clr[2];
    pquad->vtx10.clr[3] = pquad->vtx00.clr[3];
    
    pquad->vtx11.clr[0] = pquad->vtx00.clr[0];
    pquad->vtx11.clr[1] = pquad->vtx00.clr[1];
    pquad->vtx11.clr[2] = pquad->vtx00.clr[2];
    pquad->vtx11.clr[3] = pquad->vtx00.clr[3];
}




