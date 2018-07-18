#ifndef __SW_COM_SPR_H__
#define __SW_COM_SPR_H__
/*
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
#include "sw_img.h"
#include "sw_def.h"
#include "sw_cfg.h"
#include "sw_spr.h"


/*
** DEFINITIONS
*/

#define SW_SPR_INUSE        (1<<0)
#define SW_SPR_CHG_POS      (1<<1)
#define SW_SPR_CHG_IMG      (1<<2)
#define SW_SPR_CHG_COLOR    (1<<3)
#define SW_SPR_VISIBLE      (1<<4)
        
#define SW_SPR_CHANGED      (SW_SPR_CHG_POS|SW_SPR_CHG_IMG|SW_SPR_CHG_COLOR)
        
        
struct _SW_SPR_S
{
    int             flags;  /* OR'ed combination of above flags */
    SWimage         image;
    SWanchor        anchor;

    float           pos_x;
    float           pos_y;
    float           offs_x;
    float           offs_y;
    float           anchor_offs_x;
    float           anchor_offs_y;
    float           rotation;
    float           scale_x;
    float           scale_y;
    float           clip_x;
    float           clip_y;
    int             size_x;
    int             size_y;
    int             resize;
    int             flip_x;
    int             flip_y;
    unsigned char   r;
    unsigned char   g;
    unsigned char   b;
    unsigned char   a;
};
typedef struct _SW_SPR_S SW_SPR_S;
        
        
/*
** EXPORTED VARIABLES
*/

extern SW_SPR_S sw_spr_as[ SW_SPR_MAX ];
        /*
        ** Expose the global array of sprites for easy accesibility to
        ** any supporting modules. Direct access to this array by 
        ** external modules is treated as read-only.
        **
        ** The value of the sprite handle SWsprite is a direct index
        ** into this array. The zero (0) element is treated as the
        ** null sprite.
        */


/*
** EXPORTED FUNCTIONS
*/

extern void swSprRenderDelegate( SWsprite sprite );
        /*
        ** External function called by this module.
        ** Expects the specific graphics module to implement
        ** this function.
        */
        
extern void swSprPushDelegate( SWsprite sprite );
extern void swSprPassDelegate( SWsprite sprite );
extern void swSprPopDelegate ( void );
        /*
        ** External functions for pushing and popping sprite context.
        ** As this is tied to sprite rendering, it should be implemented
        ** where the rendering occurs.
        */


#if defined( __cplusplus )
		}
#endif

#endif /* __SW_COM_SPR_H__ */



