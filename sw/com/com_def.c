/*
** PROLOGUE:
**
**
*/

/*
** PREREQUISITES
*/
#include "sw_def.h"


/*
** DEFINITIONS
*/


/*
** LOCAL VARIABLES
*/


/*
** EXPORTED FUNCTIONS
*/

int swIsTouching( 
     float x, float y, 
     float l, float b, float w, float h 
     )    
{
    return( (int)(x >= l && x <= (l+w) && y >= b && y <= (b+h)) );
}

int
swIsTouchingv(
    float x, float y,
    float * rect
    )
{
    return swIsTouching( x, y, rect[0], rect[1], rect[2], rect[3] );
}


void
swRealignPoint(
    float sx, float sy,
    float sw, float sh,
    float dw, float dh,
    float * px,
    float * py
    )
{
    float sw2 = sw * 0.5;
    float sh2 = sh * 0.5;
    
    float dw2 = dw * 0.5;
    float dh2 = dh * 0.5;
    
    float nx = (sx - sw2) / sw2;
    float ny = (sy - sh2) / sh2;
    
    nx = (nx * dw2) + dw2;
    ny = (ny * dh2) + dh2;
    
    if( px ) *px = nx;
    if( py ) *py = ny;
}

