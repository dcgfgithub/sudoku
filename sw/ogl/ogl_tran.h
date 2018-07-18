#ifndef __SW_OGL_TRAN_H__
#define __SW_OGL_TRAN_H__
/*
** Module Prefix : swoglTrans
**
** PROLOGUE:
**
*/
#if defined( __cplusplus )
extern "C" {
#endif

/*
** PREREQUISITES
*/


/*
** DEFINITIONS
*/
struct _SW_OGL_TRANSFORM_S
{
    float a;
    float b;
    float c;
    float d;
    float tx;
    float ty;
};
typedef struct _SW_OGL_TRANSFORM_S SW_OGL_TRANSFORM_S;

struct _SW_OGL_POINT_S
{
    float x;
    float y;
};
typedef struct _SW_OGL_POINT_S SW_OGL_POINT_S;


/*
** EXPORTED FUNCTIONS
*/

SW_OGL_TRANSFORM_S
swoglTransIdentity( void );
        /*
        ** Returns a new transform representing the identity transform.
        */


SW_OGL_TRANSFORM_S
swoglTransRotate(
        SW_OGL_TRANSFORM_S transform,
        float degrees
        );
    
    
SW_OGL_TRANSFORM_S
swoglTransTranslate(
        SW_OGL_TRANSFORM_S transform,
        float x,
        float y
        );
    
    
SW_OGL_TRANSFORM_S
swoglTransScale(
        SW_OGL_TRANSFORM_S transform,
        float xs,
        float ys
        );
    
    
SW_OGL_TRANSFORM_S
swoglTransMultiply(
        SW_OGL_TRANSFORM_S transform1,
        SW_OGL_TRANSFORM_S transform2
        );
    
    
SW_OGL_POINT_S
swoglTransPoint(
        SW_OGL_TRANSFORM_S transform,
        SW_OGL_POINT_S     point
        );
    
    
#if defined( __cplusplus )
}
#endif

#endif /* __SW_OGL_TRAN_H__ */
