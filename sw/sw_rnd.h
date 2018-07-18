#ifndef __SW_RND_H__
#define __SW_RND_H__
/*
** PROLOGUE:
**
** Implementation of the R250 random number generator appearing
** in Dr.Dobbs Journal magazine from May 1991. This implementation
** is faster than the standard C version and is guaranteed to 
** generate the same random sequence given a seed value regardless
** of the platform it is run under.
*/

#ifdef __cplusplus
extern "C" {
#endif

/*
** PREREQUISITES
*/

/*
** DEFINITIONS
*/

/*
** EXPORTED FUNCTIONS
*/

void swRndSeed( int seed );
        /*
        ** Seeds the R250 random number generator with the given seed value.
        ** Using the same seed value guarantees the same random number sequence.
        ** This current implementation has a sequence period of 1.8E75.
        */


unsigned short swRndGen( void );
        /*
        ** Get the next random number from the random sequence.
        ** The value returned will be between 0 and 0xffff.
        */


float swRndGen11( void );
        /*
        ** Get the next random number from the random sequence
        ** as a floating point value between -1.0 and 1.0
        */
    
    
float swRndGen01( void );
        /*
        ** Get the next random number from the random sequence
        ** as a floating point value between 0.0 and 1.0
        */
    
    
#ifdef __cplusplus
}
#endif

#endif /* __SW_RND_H__ */
