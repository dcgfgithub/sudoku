#ifndef __SW_ANM_H__
#define __SW_ANM_H__
/*
** Module Prefix : swAnm
**
** PROLOGUE:
**
** Implements a simple animation system based on keyframes and
** tweening. The tween frames can be controlled using a selection
** of easing functions.
**
** Anims use sprites as their display medium. As such, an anim provides
** control over all of the same sprite properties.
**
** There are two mechanisms defined, an anim and a runtime anim, or
** rtanim for short. The anim is used to define the actions that the
** anim will perform, and the rtanim is responsible for performing
** those actions on a sprite. You can use the same animation to 
** animate multiple sprites.
*/
#if defined( __cplusplus )
	extern "C" {
#endif


/*
** PREREQUISITES
*/
#include "sw_spr.h"


/*
** DEFINITIONS
*/
typedef unsigned int SWanim;
typedef unsigned int SWrtanim;

typedef enum 
{
	SW_ANM_EASE_LINEAR = 0,
	SW_ANM_EASE_INOUT,
	SW_ANM_EASE_IN,
	SW_ANM_EASE_OUT,
    SW_ANM_EASE_BACK,
    SW_ANM_EASE_ELASTIC
} 
SWeasing;

typedef void SWanimcb( int opaque );
		

/*
** FUNCTION PROTOTYPES
*/
int  swAnmInit( void );
void swAnmFree( void );
                                
int  swAnmGen( int num_anims, SWanim* anims );
void swAnmDel( int num_anims, SWanim* anims );


/*
** Removes all keyframes and clears all flags from
** the anim. Use if you want to reuse an existing anim.
*/
void swAnmClear( SWanim anim );


/*
** These routines set the various states for the bound
** animation. Most of them determine what the animation will
** do when it completes its last frame.
*/		
void swAnmLoop   ( SWanim anim, int loop_count );
void swAnmReverse( SWanim anim );
void swAnmOneShot( SWanim anim );

        
/*
** Adds a new keyframe with the given duration to the
** bound animation. After adding a keyframe, all proceeding
** actions will be applied to it, until a new
** keyframe is added.
*/
void swAnmKeyframe( SWanim anim, int duration );


/*
** Sets the easing action to use for tween frames.
** Default for new keyframes is linear easing.
*/		
void swAnmEase( SWanim anim, SWeasing ease );


/*
** Sets the callback function for the animation.
*/
void swAnmCallback( SWanim anim, SWanimcb* pcallback, int opaque );

		
/*
** Sets the current keyframe to move the anim.
** Changes the sprite position.
**
** Move will move the anim by the amount relative to the sprites
** current position when the keyframe is executed.
** MoveTo will move the anim to a fixed point. 
*/
void swAnmMove   ( SWanim anim, float x, float y );
void swAnmMoveX  ( SWanim anim, float x );
void swAnmMoveY  ( SWanim anim, float y );
void swAnmMoveTo ( SWanim anim, float x, float y );
void swAnmMoveToX( SWanim anim, float x );
void swAnmMoveToY( SWanim anim, float y );

		
/*
** Set the current keyframe to offset the anim.
** Changes the sprite offset.
*/
void swAnmOffset   ( SWanim anim, float x, float y );
void swAnmOffsetX  ( SWanim anim, float x );
void swAnmOffsetY  ( SWanim anim, float y );
void swAnmOffsetTo ( SWanim anim, float x, float y );
void swAnmOffsetToX( SWanim anim, float x );
void swAnmOffsetToY( SWanim anim, float y );

		
/*
** These routines set the current keyframe to change the
** anchor offset value for the animation.
*/
void swAnmAnchorOffset   ( SWanim anim, float x, float y );
void swAnmAnchorOffsetX  ( SWanim anim, float x );
void swAnmAnchorOffsetY  ( SWanim anim, float y );
void swAnmAnchorOffsetTo ( SWanim anim, float x, float y );
void swAnmAnchorOffsetToX( SWanim anim, float x );
void swAnmAnchorOffsetToY( SWanim anim, float y );
		

/*
** These routines are used for setting the rotation
** of the anim for the current keyframe.
**
** The rotation angle is given in degrees, where a positive angle
** represents a counter clockwise rotation, and a negative value is
** a clockwise rotation. Angles given above 360 will modded.
*/
void swAnmRotate  ( SWanim anim, float angle );
void swAnmRotateTo( SWanim anim, float angle );


/*
** These routines are used for setting the scale
** of the anim for the current keyframe.
**
** The anim scale determines the relative size of the anim
** when it is displayed. Values above 1.0 will make the anim appear
** larger, while values below 1.0 will make the anim appear smaller.
** Negative values are taken as absolute values.
*/
void swAnmScale   ( SWanim anim, float x_scale, float y_scale );
void swAnmScaleX  ( SWanim anim, float x_scale );
void swAnmScaleY  ( SWanim anim, float y_scale );
void swAnmScaleTo ( SWanim anim, float x_scale, float y_scale );
void swAnmScaleToX( SWanim anim, float x_scale );
void swAnmScaleToY( SWanim anim, float y_scale );


/*
** These routines are used to control the animation fading
** in and out.
*/
void swAnmFade   ( SWanim anim, float fade );
void swAnmFadeTo ( SWanim anim, float fade );
void swAnmFadeIn ( SWanim anim );
void swAnmFadeOut( SWanim anim );


/*
** Routine for defining an animation as a series of images.
**
** The images will be displayed evenly across the duration of
** the keyframe. If you want a smooth even animation, it is
** best to specify the keyframe duration as a multiple of the
** number of images.
*/
void swAnmImage  ( SWanim anim, SWimage image );
void swAnmImagev ( SWanim anim, int num_img, SWimage* images );
void swAnmImagerv( SWanim anim, int num_img, SWimage* images );
void swAnmImagevi( SWanim anim, int num_img, int * indexes, SWimage* images );



/*
** Routines used for controlling the sprite clipping
** region.
*/
void swAnmClip ( SWanim anim, float clip_x, float clip_y );
void swAnmClipX( SWanim anim, float clip_x );
void swAnmClipY( SWanim anim, float clip_y );


/*
** Starts the bound animation and uses the given sprite
** for its actions. Returns a handle to a rtanim that can
** be used for further control.
*/
SWrtanim swAnmStart( SWanim anim, SWsprite sprite );


/*
** Immediately stops the given rtanim.
** All further actions are halted, and the sprite remains
** in its current state.
*/
void swAnmStopAll( void );
void swAnmStop   ( SWrtanim rtanim );
void swAnmStopv  ( int count, SWrtanim * rtas );


/*
** Pauses and resumes rtanims.
** Paused animations remain visible on screen, but do not 
** advance. Resuming them resumes processing from their paused
** state.
*/
void swAnmPauseAll    ( void );
void swAnmPause       ( SWrtanim rtanim );
void swAnmPausev      ( int count, SWrtanim * rtas );

void swAnmResumeAll   ( void );
void swAnmResume      ( SWrtanim rtanim );
void swAnmResumev     ( int count, SWrtanim * rtas );
   
        
/*
** Return the current animation frame number.
** Will only return a useful value when used with animations
** that are set with swAnmImagev, or swAnmImagerv
*/
int  swAnmGetFrame( SWrtanim rtanim );

		
/*
** Main anim processing function.
** Performs a single tick on all rtanims.
*/		
void swAnmTick( void );


#if defined( __cplusplus )
		}
#endif

#endif /* __SW_ANM_H__ */



