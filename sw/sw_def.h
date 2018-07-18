#ifndef __SW_DEF_H__
#define __SW_DEF_H__
/*
** PROLOGUE:
**
** Contains definitions for various useful macros and
** functions that can be used by the application or the
** sylkworm code base.
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
#define SW_PI 			 	3.1415926f
#define SW_PI_DIV_180    	((float)(SW_PI/180.0f))
#define SW_180_DIV_PI       ((float)(180.0f/SW_PI))
#define SW_DEG_TO_RAD(deg)	((float)(((float)(deg))*SW_PI_DIV_180))
#define SW_RAD_TO_DEG(rad)  ((float)(((float)(rad))*SW_180_DIV_PI))


#define SW_MAX(x, y) (((x) > (y)) ? (x) : (y))
#define SW_MIN(x, y) (((x) < (y)) ? (x) : (y))
        /* 
        ** Standard Min and Max macros.
        ** The parameters can be any numerical type, provided
        ** they are both the same type.
        */


#define SW_CLAMP(x, l, h) (((x) > (h)) ? (h) : (((x) < (l)) ? (l) : (x)))
        /*
        ** Clamps or bounds a number 'x' between the values
        ** 'l' and 'h', such that l <= x <= h, and returns the
        ** result.
        **
        ** The parameters can be any numerical type, and ideally
        ** should all be the same type.
        */


#define SW_ROUND(nm, dm) ((int)(((float)(nm) / (float)(dm)) + 0.5))
#define SW_UPPER(nm)     ((int)((float)(nm) + 0.5))
#define SW_LOWER(nm)     ((int)(float)(nm))
        /*
        ** Used to round up or down floating point values and 
        ** return integer results.
        */


#define SW_ARRSIZE(arr) ((sizeof(arr)) / (sizeof(arr[0])))
        /*
        ** Determines the size of a static array as the 
        ** total number of elements. Array parameters must
        ** be actual array definitions, and not pointers
        ** to an array.
        */
        
        
#define SW_SGN(nm)  ((nm) >= 0 ? 1 : -1)
#define SW_SGN0(nm) ((nm)  > 0 ? 1 : ((nm) < 0 ? -1 : 0))
        /*
        ** Determines the sign of a number by returning a digit indicating
        ** the sign. The first macro returns 1 if the number is greater or equal
        ** to 0, or -1 otherwise. The second macro works the same, but will
        ** return 0 if the number is 0.
        */


/*
** FUNCTION PROTOTYPES
*/

int
swIsTouching(
    float x, float y, 
    float l, float b, float w, float h 
    );
    
int
swIsTouchingv(
    float x, float y,
    float * rect
    );
    
    
void
swRealignPoint(
    float sx, float sy,
    float sw, float sh,
    float dw, float dh,
    float * px,
    float * py
    );


#if defined( __cplusplus )
		}
#endif

#endif /* __SW_DEF_H__ */



