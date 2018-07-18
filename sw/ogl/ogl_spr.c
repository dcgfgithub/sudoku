/*
** Module Prefix : swSpr
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
#include <memory.h>

#include "sw_cfg.h"
#include "sw_def.h"
#include "sw_spr.h"

#include "ogl_gfx.h"
#include "ogl_img.h"
#include "ogl_spr.h"
#include "ogl_tex.h"
#include "ogl_tran.h"


/*
** DEFINITIONS
*/ 
#define SW_SPR_MAX_TRANSFORM_STACK 8

/*
** LOCAL VARIABLES
*/

static float ga_spr_anchor_offset[SW_ANCHOR_COUNT][2] =
{
	{  0.0,  0.0 },
	{  0.5, -0.5 },
	{  0.0, -0.5 },
	{ -0.5, -0.5 },
    {  0.5,  0.0 },
	{ -0.5,  0.0 },
	{  0.5,  0.5 },
	{  0.0,  0.5 },
	{ -0.5,  0.5 }
};

static SW_OGL_TRANSFORM_S ga_spr_transform_stack_as[ SW_SPR_MAX_TRANSFORM_STACK ];
static int                g_spr_transform_stack_pointer = 0;


/*
** EXPORTED VARIABLES
*/

SW_OGL_SPR_S sw_ogl_spr_as[ SW_SPR_MAX ] = {0};


/*
** LOCAL FUNCTIONS
*/

static void swoglSprRecalc( SW_SPR_S * pspr, SW_OGL_SPR_S * poglspr )
{
    /*
    ** This function is called only when the sprite changes in some
    ** way. Any change will trigger this function to be called, thus
    ** recalculating the sprite position, transformation matrix, and
    ** clipping of vertex and texture coordinates.
    */
    
    /*
    ** If the image or the clip region has changed, we need to
    ** reset the coordinates for the sprite.
    */
    if( pspr->flags & (SW_SPR_CHG_IMG) )
    {
        swoglQuadSet( &poglspr->quad, &poglspr->quadInfo,
                       pspr->image, 0.0, 0.0,
                       pspr->size_x, pspr->size_y, pspr->resize,
                       pspr->flip_x, pspr->flip_y
                       );
        if( pspr->flip_x || pspr->flip_y )
        {
            swoglQuadFlip( &poglspr->quad, pspr->flip_x, pspr->flip_y );
        }
        if( pspr->clip_x < 1.0 || pspr->clip_y < 1.0 )
        {
        }
        poglspr->mats.texture = swImgGetTexture( pspr->image );
    }

    if( pspr->flags & (SW_SPR_CHG_COLOR) )
    {
        swoglQuadColorub( &poglspr->quad, pspr->r, pspr->g, pspr->b, pspr->a );
    }
    
    /*
    ** If the position of the sprite changed,
    ** The initial sprite position will be interpreted as the center 
    ** point of the sprite.
    */
    if( pspr->flags & SW_SPR_CHG_POS )
    {
        int image_w = poglspr->quadInfo.size_w;
        int image_h = poglspr->quadInfo.size_h;
    
        /*
        ** Calculate the transformation matrix.
        */
        poglspr->transform = swoglTransIdentity( );
        
        float x_pos = pspr->pos_x + pspr->offs_x;
        float y_pos = pspr->pos_y + pspr->offs_y;
        
        poglspr->transform = swoglTransTranslate( poglspr->transform, x_pos, y_pos   );
        poglspr->transform = swoglTransRotate   ( poglspr->transform, pspr->rotation );
        
        x_pos = (ga_spr_anchor_offset[pspr->anchor][0] * image_w * pspr->scale_x) + pspr->anchor_offs_x;
        y_pos = (ga_spr_anchor_offset[pspr->anchor][1] * image_h * pspr->scale_y) + pspr->anchor_offs_y;
        
        poglspr->transform = swoglTransTranslate( poglspr->transform, x_pos, y_pos );
        poglspr->transform = swoglTransScale    ( poglspr->transform, pspr->scale_x, pspr->scale_y );
    }
        
    /*
    ** Apply transformation matrix to all of the 
    ** points in the quad. Also, if the transform stack is not empty,
    ** apply the transform at the top of the stack.
    */
    poglspr->quadTransformed = poglspr->quad;
    
    SW_OGL_TRANSFORM_S transform_s;
    SW_OGL_POINT_S     point, transformed_point;
    
    transform_s = poglspr->transform;
    
    if( g_spr_transform_stack_pointer > 0 )
        transform_s = swoglTransMultiply(ga_spr_transform_stack_as[g_spr_transform_stack_pointer], transform_s );
    
    point.x = poglspr->quad.vtx00.pos[0];
    point.y = poglspr->quad.vtx00.pos[1];
    
    transformed_point = swoglTransPoint( transform_s, point );
    
    poglspr->quadTransformed.vtx00.pos[0] = transformed_point.x;
    poglspr->quadTransformed.vtx00.pos[1] = transformed_point.y;
    
    point.x = poglspr->quad.vtx01.pos[0];
    point.y = poglspr->quad.vtx01.pos[1];
    
    transformed_point = swoglTransPoint( transform_s, point );
    
    poglspr->quadTransformed.vtx01.pos[0] = transformed_point.x;
    poglspr->quadTransformed.vtx01.pos[1] = transformed_point.y;
    
    point.x = poglspr->quad.vtx10.pos[0];
    point.y = poglspr->quad.vtx10.pos[1];
    
    transformed_point = swoglTransPoint( transform_s, point );
    
    poglspr->quadTransformed.vtx10.pos[0] = transformed_point.x;
    poglspr->quadTransformed.vtx10.pos[1] = transformed_point.y;
    
    point.x = poglspr->quad.vtx11.pos[0];
    point.y = poglspr->quad.vtx11.pos[1];
    
    transformed_point = swoglTransPoint( transform_s, point );
    
    poglspr->quadTransformed.vtx11.pos[0] = transformed_point.x;
    poglspr->quadTransformed.vtx11.pos[1] = transformed_point.y;

    pspr->flags &= ~SW_SPR_CHANGED;
}


/*
** EXPORTED FUNCTIONS
*/

void swSprGetBoundingBox( SWsprite spr, float * ppoints )
{
    assert( ppoints != NULL );
    
    if( sw_spr_as[spr].flags & SW_SPR_CHANGED )
    {
        swoglSprRecalc( &sw_spr_as[spr], &sw_ogl_spr_as[spr] );
    }
    
    ppoints[0] = sw_ogl_spr_as[spr].quadInfo.l;
    ppoints[1] = sw_ogl_spr_as[spr].quadInfo.t;
    ppoints[2] = sw_ogl_spr_as[spr].quadInfo.w;
    ppoints[3] = sw_ogl_spr_as[spr].quadInfo.h;
}

void swSprGetBoundingBox2( SWsprite spr, float * ppoints )
{
    assert( ppoints != NULL );
    
    if( sw_spr_as[spr].flags & SW_SPR_CHANGED )
    {
        swoglSprRecalc( &sw_spr_as[spr], &sw_ogl_spr_as[spr] );
    }
    
    ppoints[0] = sw_ogl_spr_as[spr].quadTransformed.vtx00.pos[0];
    ppoints[1] = sw_ogl_spr_as[spr].quadTransformed.vtx00.pos[1];
    ppoints[2] = sw_ogl_spr_as[spr].quadTransformed.vtx10.pos[0] - sw_ogl_spr_as[spr].quadTransformed.vtx00.pos[0];
    ppoints[3] = sw_ogl_spr_as[spr].quadTransformed.vtx01.pos[1] - sw_ogl_spr_as[spr].quadTransformed.vtx00.pos[1];
}

void swSprRenderDelegate( SWsprite spr )
{
    if( g_spr_transform_stack_pointer > 0 || sw_spr_as[spr].flags & SW_SPR_CHANGED )
    {
        swoglSprRecalc( &sw_spr_as[spr],
                        &sw_ogl_spr_as[spr] );
    }
    swoglGfxRender( &sw_ogl_spr_as[spr].quadTransformed,
                    &sw_ogl_spr_as[spr].mats );
}

void swSprPushDelegate( SWsprite sprite )
{
    assert( g_spr_transform_stack_pointer < SW_SPR_MAX_TRANSFORM_STACK );
    
    ga_spr_transform_stack_as[0] = swoglTransIdentity();
    
    if( g_spr_transform_stack_pointer > 0 || sw_spr_as[sprite].flags & SW_SPR_CHANGED )
    {
        swoglSprRecalc( &sw_spr_as[sprite],
                        &sw_ogl_spr_as[sprite] );
    }
    ga_spr_transform_stack_as[ g_spr_transform_stack_pointer+1 ] =
            swoglTransMultiply( ga_spr_transform_stack_as[ g_spr_transform_stack_pointer],
                                sw_ogl_spr_as[sprite].transform );
    
    g_spr_transform_stack_pointer++;
}

void swSprPassDelegate( SWsprite sprite )
{
    assert( g_spr_transform_stack_pointer < SW_SPR_MAX_TRANSFORM_STACK );
    
    if( g_spr_transform_stack_pointer > 0 || sw_spr_as[sprite].flags & SW_SPR_CHANGED )
    {
        swoglSprRecalc( &sw_spr_as[sprite],
                       &sw_ogl_spr_as[sprite] );
    }
}

void swSprPopDelegate( void )
{
    if( g_spr_transform_stack_pointer > 0 )
        g_spr_transform_stack_pointer--;
}


