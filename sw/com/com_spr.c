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

#include "com_spr.h"

#include "sw_cfg.h"
#include "sw_def.h"
#include "sw_spr.h"


/*
** DEFINITIONS
*/


/*
** EXPORTED VARIABLES
*/

SW_SPR_S sw_spr_as[ SW_SPR_MAX ] = {0};


/*
** LOCAL FUNCTIONS
*/

static void sprClear( SWsprite sprite )
{
    sw_spr_as[sprite].flags            = 0;
    sw_spr_as[sprite].image            = 0;
    sw_spr_as[sprite].anchor           = SW_ANCHOR_CENTER;
    sw_spr_as[sprite].pos_x            = 0.0;
    sw_spr_as[sprite].pos_y            = 0.0;
    sw_spr_as[sprite].offs_x           = 0.0;
    sw_spr_as[sprite].offs_y           = 0.0;
    sw_spr_as[sprite].anchor_offs_x    = 0.0;
    sw_spr_as[sprite].anchor_offs_y    = 0.0;
    sw_spr_as[sprite].rotation         = 0.0;
    sw_spr_as[sprite].scale_x          = 1.0;
    sw_spr_as[sprite].scale_y          = 1.0;
    sw_spr_as[sprite].clip_x           = 1.0;
    sw_spr_as[sprite].clip_y           = 1.0;
    sw_spr_as[sprite].size_x           = 0;
    sw_spr_as[sprite].size_y           = 0;
    sw_spr_as[sprite].resize           = 0;
    sw_spr_as[sprite].flip_x           = 0;
    sw_spr_as[sprite].flip_y           = 0;
    sw_spr_as[sprite].r                = 255;
    sw_spr_as[sprite].g                = 255;
    sw_spr_as[sprite].b                = 255;
    sw_spr_as[sprite].a                = 255;
}


/*
** EXPORTED FUNCTIONS
*/

int  swSprInit( void )
{
    return 1;
}
void swSprFree( void )
{
}


int swSprGen( int num_sprites, SWsprite * sprites )
{
    int spr, i;

    assert( num_sprites >= 0 );
    assert( sprites != NULL );
    
    for( spr=0; spr<num_sprites; spr++ )
    {
        sprites[spr] = 0;
        for( i=1; i<SW_SPR_MAX; i++ )
        {
            if( sw_spr_as[i].flags == 0 )
                break;
        }
        if( i >= SW_SPR_MAX )
            return spr;

		sprClear( i );                 
        sw_spr_as[i].flags = SW_SPR_INUSE;
        
        sprites[spr] = i;
    }
    return spr;
}


void swSprDel( int num_sprites, SWsprite* sprites )
{
    /*
    ** Releases one or more sprites previously allocated and
    ** returns them to the sprite pool.    
    */
    int spr;

    assert( num_sprites >= 0 );
    assert( sprites != NULL );

    for( spr=0; spr<num_sprites; spr++ )
    {
        if( sprites[spr] > 0 && sprites[spr] < SW_SPR_MAX )
            sprClear( sprites[spr] );
    }
}


void swSprClear( SWsprite sprite )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );

    sprClear( sprite );
    sw_spr_as[sprite].flags = (SW_SPR_INUSE|SW_SPR_CHG_POS|SW_SPR_CHG_IMG|SW_SPR_CHG_COLOR);
}

void swSprShow( SWsprite sprite, int show )
{
    if( show )
        sw_spr_as[sprite].flags |= SW_SPR_VISIBLE;
    else
        sw_spr_as[sprite].flags &= ~(SW_SPR_VISIBLE);
}


/*
** Image
*/
void swSprSetImage( SWsprite sprite, SWimage image )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    sw_spr_as[sprite].image = image;
    sw_spr_as[sprite].flags |= (SW_SPR_CHG_IMG|SW_SPR_CHG_POS);
}

SWimage swSprGetImage( SWsprite sprite )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    return sw_spr_as[sprite].image;
}


/*
** Size
*/
void swSprSetSize ( SWsprite sprite, int x_size, int y_size, int resize )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    sw_spr_as[sprite].resize = resize;
    sw_spr_as[sprite].size_x = x_size;
    sw_spr_as[sprite].size_y = y_size;
    sw_spr_as[sprite].flags |= SW_SPR_CHG_IMG;
}

void swSprSetSizeX( SWsprite sprite, int x_size, int resize )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );

    sw_spr_as[sprite].resize = resize;
    sw_spr_as[sprite].size_x = x_size;
    sw_spr_as[sprite].flags |= SW_SPR_CHG_IMG;
}

void swSprSetSizeY( SWsprite sprite, int y_size, int resize )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    sw_spr_as[sprite].resize = resize;
    sw_spr_as[sprite].size_y = y_size;
    sw_spr_as[sprite].flags |= SW_SPR_CHG_IMG;
}

void  swSprGetSize ( SWsprite sprite, int* px, int* py )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    if( px ) *px = sw_spr_as[sprite].size_x;
    if( py ) *py = sw_spr_as[sprite].size_y;    
}

float swSprGetSizeX( SWsprite sprite )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    return sw_spr_as[sprite].size_x;    
}

float swSprGetSizeY( SWsprite sprite )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    return sw_spr_as[sprite].size_y;        
}

/*
** Clip
*/
void swSprSetClip( SWsprite sprite, float x_clip, float y_clip )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    sw_spr_as[sprite].clip_x = x_clip;
    sw_spr_as[sprite].clip_y = y_clip;
    sw_spr_as[sprite].flags |= SW_SPR_CHG_IMG;
}

void swSprSetClipX( SWsprite sprite, float x_clip )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    sw_spr_as[sprite].clip_x = x_clip;
    sw_spr_as[sprite].flags |= SW_SPR_CHG_IMG;
}

void swSprSetClipY( SWsprite sprite, float y_clip )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    sw_spr_as[sprite].clip_y = y_clip;
    sw_spr_as[sprite].flags |= SW_SPR_CHG_IMG;
}

void swSprGetClip( SWsprite sprite, float* cx, float* cy )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    if( cx ) *cx = sw_spr_as[sprite].clip_x;
    if( cy ) *cy = sw_spr_as[sprite].clip_y;
}

float swSprGetClipX( SWsprite sprite )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    return sw_spr_as[sprite].clip_x;
}

float swSprGetClipY( SWsprite sprite )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    return sw_spr_as[sprite].clip_y;
}


/*
** Flip
*/
void swSprSetFlip ( SWsprite sprite, int x_flip, int y_flip )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    sw_spr_as[sprite].flip_x = x_flip;
    sw_spr_as[sprite].flip_y = y_flip;
    sw_spr_as[sprite].flags |= SW_SPR_CHG_IMG;
}

void swSprSetFlipX( SWsprite sprite, int x_flip )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    sw_spr_as[sprite].flip_x = x_flip;
    sw_spr_as[sprite].flags |= SW_SPR_CHG_IMG;
}

void swSprSetFlipY( SWsprite sprite, int y_flip )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    sw_spr_as[sprite].flip_y = y_flip;
    sw_spr_as[sprite].flags |= SW_SPR_CHG_IMG;
}



/*
** Anchor
*/
void swSprSetAnchor( SWsprite sprite, SWanchor anchor )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    sw_spr_as[sprite].anchor = anchor;
    sw_spr_as[sprite].flags |= SW_SPR_CHG_POS;
}

SWanchor swSprGetAnchor( SWsprite sprite )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    return sw_spr_as[sprite].anchor;
}


/*
** Anchor offset
*/
void swSprSetAnchorOffs( SWsprite sprite, float x, float y )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    sw_spr_as[sprite].anchor_offs_x = x;
    sw_spr_as[sprite].anchor_offs_y = y;
    sw_spr_as[sprite].flags |= SW_SPR_CHG_POS;
}

void swSprSetAnchorOffsX( SWsprite sprite, float x )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    sw_spr_as[sprite].anchor_offs_x = x;
    sw_spr_as[sprite].flags |= SW_SPR_CHG_POS;
}

void swSprSetAnchorOffsY( SWsprite sprite, float y )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    sw_spr_as[sprite].anchor_offs_y = y;
    sw_spr_as[sprite].flags |= SW_SPR_CHG_POS;
}

void swSprGetAnchorOffs( SWsprite sprite, float* px, float* py )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    if( px ) *px = sw_spr_as[sprite].anchor_offs_x;
    if( py ) *py = sw_spr_as[sprite].anchor_offs_y;
}

float swSprGetAnchorOffsX( SWsprite sprite )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    return sw_spr_as[sprite].anchor_offs_x;
}

float swSprGetAnchorOffsY( SWsprite sprite )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    return sw_spr_as[sprite].anchor_offs_y;
}


/*
** Position
*/
void swSprSetPos( SWsprite sprite, float x, float y )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    sw_spr_as[sprite].pos_x = x;
    sw_spr_as[sprite].pos_y = y;
    sw_spr_as[sprite].flags |= SW_SPR_CHG_POS;
}

void swSprSetPosX( SWsprite sprite, float x )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    sw_spr_as[sprite].pos_x = x;
    sw_spr_as[sprite].flags |= SW_SPR_CHG_POS;
}

void swSprSetPosY( SWsprite sprite, float y )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    sw_spr_as[sprite].pos_y = y;
    sw_spr_as[sprite].flags |= SW_SPR_CHG_POS;
}

void swSprSetPosOffs( SWsprite sprite, float xoffs, float yoffs )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    sw_spr_as[sprite].pos_x += xoffs;
    sw_spr_as[sprite].pos_y += yoffs;
    sw_spr_as[sprite].flags |= SW_SPR_CHG_POS;
}

void swSprSetPosOffsX( SWsprite sprite, float xoffs )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    sw_spr_as[sprite].pos_x += xoffs;
    sw_spr_as[sprite].flags |= SW_SPR_CHG_POS;
}

void swSprSetPosOffsY( SWsprite sprite, float yoffs )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    sw_spr_as[sprite].pos_y += yoffs;
    sw_spr_as[sprite].flags |= SW_SPR_CHG_POS;
}

void swSprGetPos( SWsprite sprite, float* px, float* py )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    if( px ) *px = sw_spr_as[sprite].pos_x;
    if( py ) *py = sw_spr_as[sprite].pos_y;
}

float swSprGetPosX( SWsprite sprite )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    return sw_spr_as[sprite].pos_x;
}

float swSprGetPosY( SWsprite sprite )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    return sw_spr_as[sprite].pos_y;
}


/*
** Offset
*/
void swSprSetOffs( SWsprite sprite, float x, float y )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    sw_spr_as[sprite].offs_x = x;
    sw_spr_as[sprite].offs_y = y;
    sw_spr_as[sprite].flags |= SW_SPR_CHG_POS;
}

void swSprSetOffsX( SWsprite sprite, float x )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    sw_spr_as[sprite].offs_x = x;
    sw_spr_as[sprite].flags |= SW_SPR_CHG_POS;
}

void swSprSetOffsY( SWsprite sprite, float y )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    sw_spr_as[sprite].offs_y = y;
    sw_spr_as[sprite].flags |= SW_SPR_CHG_POS;
}

void swSprSetOffsOffs( SWsprite sprite, float xoffs, float yoffs )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    sw_spr_as[sprite].offs_x += xoffs;
    sw_spr_as[sprite].offs_y += yoffs;
    sw_spr_as[sprite].flags |= SW_SPR_CHG_POS;
}

void swSprSetOffsOffsX( SWsprite sprite, float xoffs )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    sw_spr_as[sprite].offs_x += xoffs;
    sw_spr_as[sprite].flags |= SW_SPR_CHG_POS;
}

void swSprSetOffsOffsY( SWsprite sprite, float yoffs )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    sw_spr_as[sprite].offs_y += yoffs;
    sw_spr_as[sprite].flags |= SW_SPR_CHG_POS;
}

void swSprGetOffs( SWsprite sprite, float* px, float* py )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    if( px ) *px = sw_spr_as[sprite].offs_x;
    if( py ) *py = sw_spr_as[sprite].offs_y;
}

float swSprGetOffsX( SWsprite sprite )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    return sw_spr_as[sprite].offs_x;
}

float  swSprGetOffsY( SWsprite sprite )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    return sw_spr_as[sprite].offs_y;
}


/*
** Rotation
*/
void swSprSetRotate( SWsprite sprite, float angle )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    sw_spr_as[sprite].rotation = angle;
    sw_spr_as[sprite].flags |= SW_SPR_CHG_POS;
}

float swSprGetRotate( SWsprite sprite )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    return sw_spr_as[sprite].rotation;
}


/*
** Scale
*/
void swSprSetScale( SWsprite sprite, float x_scale, float y_scale )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    sw_spr_as[sprite].scale_x = x_scale;
    sw_spr_as[sprite].scale_y = y_scale;
    sw_spr_as[sprite].flags |= SW_SPR_CHG_POS;
}

void  swSprSetScaleX( SWsprite sprite, float x_scale )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    sw_spr_as[sprite].scale_x = x_scale;
    sw_spr_as[sprite].flags |= SW_SPR_CHG_POS;
}

void  swSprSetScaleY( SWsprite sprite, float y_scale )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    sw_spr_as[sprite].scale_y = y_scale;
    sw_spr_as[sprite].flags |= SW_SPR_CHG_POS;
}

void swSprGetScale( SWsprite sprite, float* px, float* py )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    if( px ) *px = sw_spr_as[sprite].scale_x;
    if( py ) *py = sw_spr_as[sprite].scale_y;
}

float swSprGetScaleX( SWsprite sprite )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    return sw_spr_as[sprite].scale_x;
}

float swSprGetScaleY( SWsprite sprite )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    return sw_spr_as[sprite].scale_y;
}


/*
** Color
*/
void swSprSetColor( SWsprite sprite, unsigned char r, unsigned char g, unsigned char b, unsigned char a )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    sw_spr_as[sprite].r = r;
    sw_spr_as[sprite].g = g;
    sw_spr_as[sprite].b = b;
    sw_spr_as[sprite].a = a;
    sw_spr_as[sprite].flags |= SW_SPR_CHG_COLOR;
}

void swSprGetColor( SWsprite sprite, unsigned int * pcolor)
{
	assert( pcolor != NULL );
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    pcolor[0] = sw_spr_as[sprite].r;
    pcolor[1] = sw_spr_as[sprite].g;
    pcolor[2] = sw_spr_as[sprite].b;
	pcolor[3] = sw_spr_as[sprite].a;
}


void swSprRender( int num_sprites, SWsprite* sprites )
{
    SWsprite spr;
    int spr_index;
    
    if( sprites == NULL )
    	num_sprites = SW_SPR_MAX;
    
    for( spr_index=0; spr_index<num_sprites; spr_index++ )
    {
    	spr = (sprites != NULL ? sprites[spr_index] : spr_index);
         
        if( ! (sw_spr_as[spr].flags & SW_SPR_INUSE) )
            continue;
        
        if( ! (sw_spr_as[spr].flags & SW_SPR_VISIBLE) )
            continue;
            
        if( 0 == sw_spr_as[spr].image )
            continue;
        
        swSprRenderDelegate( spr );
    }
}


void swSprPush( SWsprite sprite )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );

    swSprPushDelegate( sprite );
}

void swSprPass( SWsprite sprite )
{
    assert( sprite < SW_SPR_MAX );
    assert( sw_spr_as[sprite].flags & SW_SPR_INUSE );
    
    swSprPassDelegate( sprite );
}

void swSprPassv( int count, SWsprite * sprites )
{
    for( int spr=0; spr<count; spr++ )
        swSprPass( sprites[spr] );
}

void swSprPop( void )
{
    swSprPopDelegate();
}

