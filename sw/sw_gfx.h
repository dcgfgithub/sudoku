#ifndef __SW_GFX_H__
#define __SW_GFX_H__
/*
** Module Prefix : swGfx
**
** PROLOGUE:
**
**
*/
#if defined( __cplusplus )
	extern "C" {
#endif


/*
** PREREQUISITES
*/
#include "sw_app.h"


/*
** DEFINITIONS
*/


/*
** EXPORTED FUNCTIONS
*/

int  swGfxInit( void );
void swGfxFree( void );


/*
** Sets the display dimensions.
** The dimensions are normally determined by the system, and therefore
** will usually make this call directly.
*/
void  swGfxSetDisplaySize( float w, float h );
float swGfxGetDisplayW   ( void );
float swGfxGetDisplayH   ( void );        

/*
** Sets the orientation of the screen.
** The orientation is normally set during the swAppOrientBegin call,
** after a valid orientation is chosen.
*/
void            swGfxSetOrientation( SW_APP_ORIENT_E orient );
SW_APP_ORIENT_E swGfxGetOrientation( void );


void swGfxRenderBegin( void );
void swGfxRenderEnd  ( void );
void swGfxRenderFlush( void );

void swGfxClip( int enabled );
void swGfxClipRect( float l, float b, float w, float h );

#if defined( __cplusplus )
		}
#endif

#endif /* __SW_GFX_H__ */



