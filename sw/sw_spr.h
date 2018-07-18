#ifndef __SW_SPR_H__
#define __SW_SPR_H__
/*
** Module Prefix : swSpr
**
** PROLOGUE:
**
** Implement a very simple sprite system designed to run on platforms
** that are implemented on top of a 3d API. As such, it includes operations
** for performing rotation and such which may not be readily available
** on systems using a traditional 2d API. 
**
** A sprite is defined as a single bitmap that can be moved around inside
** a virtural world space, and can interact with other sprites in the world.
** Each sprite is assigned a priority ( or z order ) that determines how
** it appears relative to other sprites. Higher priority sprites appear
** in front of lower priority ones.
**
** The position of the sprite is calculated as a combination of several
** parameters you set individually. These parameters are as follows..
**
**   position
**   offset
**   anchor point
**   anchor offset
**
** The anchor point defines a point relative to the current bitmap
** bound to the sprite where the sprite position is taken. So, for example,
** if the sprite anchor point was set as the center of the image, and 
** the sprite position was set as the cente of the screen, the sprite 
** would appear exactly at the center of the screen. Similarly, if the
** anchor point was set as the bottom left corner, and the sprite position
** was set to 0,0, the sprite would appear exactly at the bottom left 
** corner of the screen. 
**
** Any rotation applied to the sprite will rotate around the anchor point.
**
** You can also specify an anchor point offset, which is applied to the
** anchor point. This is useful when you want to set the anchor point 
** outside the bounds of the sprite bitmap. This allows you to make
** the sprite rotate around an arbitrary point on the screen.
**
** The position of the sprite is specified as an absolute point called the
** position, as well as a seperate offset that is applied to the position.
** The seperate offset can be used, for instance, for animations.
**
** Sprites are drawn to the display as part of the normal refresh cycle
** used by the graphics system, and therefore, there are no immediate drawing
** routines that are part of the API.
**
*/
#if defined( __cplusplus )
	extern "C" {
#endif

/*
** PREREQUISITES
*/
#include "sw_img.h"


/*
** DEFINITIONS
*/
typedef unsigned int SWsprite;

typedef enum
{
    SW_ANCHOR_CENTER = 0,
    SW_ANCHOR_UL,
    SW_ANCHOR_UM,
    SW_ANCHOR_UR,
    SW_ANCHOR_ML,
    SW_ANCHOR_MR,
    SW_ANCHOR_LL,
    SW_ANCHOR_LM,
    SW_ANCHOR_LR,
    SW_ANCHOR_COUNT
} SWanchor;        
  
              
/*
** EXPORTED FUNCTIONS
*/
int  swSprInit( void );
void swSprFree( void );
        
int  swSprGen( int num_sprites, SWsprite* sprites );
void swSprDel( int num_sprites, SWsprite* sprites );


/*
** Clears a sprite returning all settings to null values.
*/
void swSprClear( SWsprite sprite );
        
void swSprShow( SWsprite sprite, int show );

/*
** These routines are used for getting and setting the image
** used to draw the sprite.
**
** Setting the image to 0 is acceptable. 
** In this case, the sprite is essentially invisible. Can be
** used for inserting blank frames for animations.
*/
void    swSprSetImage( SWsprite sprite, SWimage image );
SWimage swSprGetImage( SWsprite sprite );


/*
** These routines are used for setting the size
** for the sprite.
**
** The size is used for setting explicit sizes for the sprite.
** Normally, the size is determined by the image assigned
** to the sprite.
*/
void swSprSetSize  ( SWsprite sprite, int x_size, int y_size, int resize );
void swSprSetSizeX ( SWsprite sprite, int x_size, int resize );
void swSprSetSizeY ( SWsprite sprite, int y_size, int resize );

void  swSprGetSize ( SWsprite sprite, int* px, int* py );
float swSprGetSizeX( SWsprite sprite );
float swSprGetSizeY( SWsprite sprite );


/*
** These routines are used for setting the clipping rectangle
** for the sprite.
**
** The clipping rectangle controls which portion of the image
** is displayed. The clipping rectangle coordinates are given
** as normalized values, and are always relative to the sprite
** anchor point.
*/
void swSprSetClip  ( SWsprite sprite, float x_clip, float y_clip );
void swSprSetClipX ( SWsprite sprite, float x_clip );
void swSprSetClipY ( SWsprite sprite, float y_clip );

void  swSprGetClip ( SWsprite sprite, float* cx, float* cy );
float swSprGetClipX( SWsprite sprite );
float swSprGetClipY( SWsprite sprite );



/*
** These routines are used for setting the flip state 
** of the sprite.
*/
void swSprSetFlip ( SWsprite sprite, int flip_x, int flip_y );
void swSprSetFlipX( SWsprite sprite, int flip_x );
void swSprSetFlipY( SWsprite sprite, int flip_y );


/*
** These routines are used for getting and setting the anchor
** point and anchor offset of the sprite.
*/
void     swSprSetAnchor     ( SWsprite sprite, SWanchor anchor );
SWanchor swSprGetAnchor     ( SWsprite sprite );

void     swSprSetAnchorOffs ( SWsprite sprite, float x, float y );
void     swSprSetAnchorOffsX( SWsprite sprite, float x );
void     swSprSetAnchorOffsY( SWsprite sprite, float y );
void     swSprGetAnchorOffs ( SWsprite sprite, float* px, float* py );

float    swSprGetAnchorOffsX( SWsprite sprite );
float    swSprGetAnchorOffsY( SWsprite sprite );


/*
** These routines are used for getting and setting the position
** of the sprite.
**
** To set the absolute position of the sprite, use swSprSetPos
** You also can set a new position as an offset of the current
** position using swSprSetPosOffs.
*/
void  swSprSetPos     ( SWsprite sprite, float x, float y );
void  swSprSetPosX    ( SWsprite sprite, float x );
void  swSprSetPosY    ( SWsprite sprite, float y );
void  swSprSetPosOffs ( SWsprite sprite, float x, float y );
void  swSprSetPosOffsX( SWsprite sprite, float xoffs );
void  swSprSetPosOffsY( SWsprite sprite, float yoffs );

void  swSprGetPos     ( SWsprite sprite, float* px, float* py );
float swSprGetPosX    ( SWsprite sprite );
float swSprGetPosY    ( SWsprite sprite );


/*
** These routines are used for getting and setting the offset
** of the sprite.
**
** To set the absolute offset of the sprite, use dcgSetoffs.
** You also can set a new offset as an offset of the current
** offset using dcgSetoffs_offs.
*/
void  swSprSetOffs     ( SWsprite sprite, float x, float y );
void  swSprSetOffsX    ( SWsprite sprite, float x );
void  swSprSetOffsY    ( SWsprite sprite, float y );
void  swSprSetOffsOffs ( SWsprite sprite, float x, float yo );
void  swSprSetOffsOffsX( SWsprite sprite, float xoffs );
void  swSprSetOffsOffsY( SWsprite sprite, float yoffs );

void  swSprGetOffs     ( SWsprite sprite, float* px, float* py );
float swSprGetOffsX    ( SWsprite sprite );
float swSprGetOffsY    ( SWsprite sprite );


/*
** These routines are used for getting and setting the rotation
** of the sprite.
**
** The rotation angle is given in degrees, where a positive angle
** represents a counter clockwise rotation, and a negative value is
** a clockwise rotation. Angles above 360.0 will be modded by
** that value.
*/
void  swSprSetRotate( SWsprite sprite, float angle );
float swSprGetRotate( SWsprite sprite );


/*
** These routines are used for getting and setting the scale
** of the sprite.
**
** The sprite scale determines the relative size of the sprite
** when it is displayed. Values above 1.0 will make the sprite appear
** larger, while values below 1.0 will make the sprite appear smaller.
** Negative values are taken as absolute values.
*/
void  swSprSetScale ( SWsprite sprite, float x_scale, float y_scale );
void  swSprSetScaleX( SWsprite sprite, float x_scale );
void  swSprSetScaleY( SWsprite sprite, float y_scale );

void  swSprGetScale ( SWsprite sprite, float* p_x_scale, float* p_y_scale );
float swSprGetScaleX( SWsprite sprite );
float swSprGetScaleY( SWsprite sprite );


/*
** These routines are used to change the sprites transparency
** level and color value.
*/
void swSprSetColor( SWsprite sprite, unsigned char r, unsigned char g, unsigned char b, unsigned char a );
void swSprGetColor( SWsprite sprite, unsigned int * pcolor );

void swSprGetBoundingBox ( SWsprite sprite, float * ppoints );
void swSprGetBoundingBox2( SWsprite sprite, float * ppoints );

void swSprRender( int num_sprites, SWsprite* sprites );

void swSprPush ( SWsprite sprite );
void swSprPass ( SWsprite sprite );
void swSprPassv( int count, SWsprite * sprites );
void swSprPop  ( void );


#if defined( __cplusplus )
		}
#endif

#endif /* __SW_SPR_H__ */



