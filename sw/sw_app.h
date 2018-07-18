#ifndef __SW_APP_H__
#define __SW_APP_H__
/*
** Module Prefix : swApp
**
** PROLOGUE:
**
** Defines the prototypes for the delegate functions required
** to be supplied by the application in order to interact with
** the Silkweed library.
**
** The file sw_app.c inside the templates folder for each
** platform contains function stubs for each of these, and
** should be copied to the application folder to act as
** a starting point for the application implementation.
*/
#if defined( __cplusplus )
	extern "C" {
#endif


/*
** DEFINITIONS
*/
typedef enum
{
    SW_APP_ORIENT_PORTRAIT = 0,
    SW_APP_ORIENT_PORTRAIT_INV,
    SW_APP_ORIENT_LANDSCAPE_LEFT,
    SW_APP_ORIENT_LANDSCAPE_RIGHT,
    SW_APP_ORIENT_COUNT
} SW_APP_ORIENT_E;

#define SW_APP_IS_LANDSCAPE(ori) ((ori) == SW_APP_ORIENT_LANDSCAPE_LEFT || (ori) == SW_APP_ORIENT_LANDSCAPE_RIGHT)
#define SW_APP_IS_PORTRAIT(ori)  ((ori) == SW_APP_ORIENT_PORTRAIT_INV   || (ori) == SW_APP_ORIENT_PORTRAIT)


/*
** EXPORTED FUNCTIONS
*/

/*
** These functions are provided as a convenience to the application 
** allowing it to initialize all of the Silkweed components together.
** It calls all of their associated Init functions, and if any of them
** fail, will terminate the application. 
**
** If you prefer to initialize the individual components yourself, 
** you can do that in place of calling these. You might do this is you
** only want to initialize a subset of the components.
*/
void swInit( void );
void swFree( void );


/*
** The remaining functions define the delegate interface between
** the system and the application. If you are using the reference system
** files, they will call these functions, and expect the application to 
** provide an implementation for each.
**
** Even if you choose not to use the reference files, these still provide
** the common interface that most systems will use to communicate events
** to the application.
**
** See the file sw_app.c in the templates section of each platform
** for a description of each function, how it is called and what 
** is typically required from the application.
*/
extern void swAppInit    ( void );
extern void swAppFree    ( void );

extern void swAppStart   ( void );

extern void swAppActive  ( void );
extern void swAppInactive( void );
extern void swAppSuspend ( void );
extern void swAppResume  ( void );

extern void swAppProcess ( void );
extern void swAppDisplay ( void );

extern void swAppPan      ( float x, float y );
extern void swAppPanUp    ( float x, float y );

extern void swAppRotate   ( float rotation );
extern void swAppRotateUp ( void );

extern void swAppScale    ( float scale );
extern void swAppScaleUp  ( void );

extern void swAppTouch    ( float x, float y );
extern void swAppTouchUp  ( float x, float y );
extern void swAppTouchMove( float x, float y );

extern void swAppBack         ( void );
extern void swAppOrientation  ( SW_APP_ORIENT_E orientation );
extern void swAppDisplaySize  ( float dw, float dh );

#if defined( __cplusplus )
		}
#endif

#endif /* __SW_APP_H__ */



