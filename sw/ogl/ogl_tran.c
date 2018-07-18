/*
** PREREQUISITES
*/
#include <math.h>

#include "sw_def.h"
#include "ogl_tran.h"


/*
** EXPORTED FUNCTIONS
*/

SW_OGL_TRANSFORM_S swoglTransIdentity( void )
{
    SW_OGL_TRANSFORM_S transform;
    
    transform.a  = 1;
    transform.b  = 0;
    transform.c  = 0;
    transform.d  = 1;
    transform.tx = 0;
    transform.ty = 0;
    
    return transform;
}


SW_OGL_TRANSFORM_S
swoglTransRotate(
    SW_OGL_TRANSFORM_S transform,
    float              degrees
    )
{
    SW_OGL_TRANSFORM_S result;
    
    float cosT = cos( SW_DEG_TO_RAD(degrees) );
    float sinT = sin( SW_DEG_TO_RAD(degrees) );
    
    result.a  = (transform.a  * cosT) + (transform.b  * sinT);
    result.b  = (transform.b  * cosT) - (transform.a  * sinT);
    result.c  = (transform.c  * cosT) + (transform.d  * sinT);
    result.d  = (transform.d  * cosT) - (transform.c  * sinT);
    result.tx = transform.tx;
    result.ty = transform.ty;
    
    return result;
}
    
    
SW_OGL_TRANSFORM_S
swoglTransTranslate(
    SW_OGL_TRANSFORM_S transform,
    float              x,
    float              y
    )
{
    SW_OGL_TRANSFORM_S result = transform;
    
    result.tx = (transform.a * x) + (transform.b * y) + (transform.tx);
    result.ty = (transform.c * x) + (transform.d * y) + (transform.ty);
    
    return result;
}
    
    
SW_OGL_TRANSFORM_S
swoglTransScale(
    SW_OGL_TRANSFORM_S transform,
    float              xs,
    float              ys
    )
{
    SW_OGL_TRANSFORM_S result = transform;
    
    result.a *= xs;
    result.b *= ys;
    result.c *= xs;
    result.d *= ys;
    
    return result;
}
    
    
SW_OGL_TRANSFORM_S
swoglTransMultiply(
    SW_OGL_TRANSFORM_S t1,
    SW_OGL_TRANSFORM_S t2
    )
{
    SW_OGL_TRANSFORM_S result;
    
    result.a  = (t1.a * t2.a)  + (t1.b * t2.c);
    result.b  = (t1.a * t2.b)  + (t1.b * t2.d);
    result.c  = (t1.c * t2.a)  + (t1.d * t2.c);
    result.d  = (t1.c * t2.b)  + (t1.d * t2.d);
    result.tx = (t1.a * t2.tx) + (t1.b * t2.ty) + t1.tx;
    result.ty = (t1.c * t2.tx) + (t1.d * t2.ty) + t1.ty;
    
    return result;
}
    
    
SW_OGL_POINT_S
swoglTransPoint(
    SW_OGL_TRANSFORM_S transform,
    SW_OGL_POINT_S     point
    )
{
    SW_OGL_POINT_S result;
    
    result.x = (transform.a * point.x) + (transform.b * point.y) + transform.tx;
    result.y = (transform.c * point.x) + (transform.d * point.y) + transform.ty;
    
    return result;
}


