/*
** PROLOGUE:
**
** Implementation of the R250 random number generator appearing
** in Dr.Dobbs Journal magazine from May 1991. This implementation
** is faster than the standard C version and is guaranteed to 
** generate the same random sequence given a seed value regardless
** of the platform it is run under.
*/

/*
** PREREQUISITES
*/
#include "sw_rnd.h"

#define R250_A_VALUE	214013L
#define R250_C_VALUE	2531011L

static unsigned short buffer[250];
static unsigned short index;

/* 
** Seeds the R250 random number generator with the given seed value. 
** Using the same seed value guarantees the same random number sequence.
** This current implementation has a sequence period of 1.8E75. 
** The initialization routine generates the first 250*250 random numbers 
** to assure a good mix of results.
*/
void swRndSeed( int seed )
{
	int i,j;
	unsigned short mask, msb;
	int lastValue;

	/*
	** First, assign the 250 array with random numbers. We will use our own
	** version of linear congruent method.
	*/
	index = 0;
	lastValue = seed;
	for( i=0; i<250; i++ )
	{
		lastValue = (R250_A_VALUE * lastValue + R250_C_VALUE);
		buffer[i] = (unsigned short)( (lastValue >> 16) % 0xffff );
	}

	/*
	** To assure linear independance, choose 16 values, mask out left bits and
	** set the MSB on the diagonal to 1.
	*/
	msb  = 0x8000;
	mask = 0xffff;
	for( i=0; i<16; i++ )
	{
		j = 11 * i + 3; /* Choose some index from random number array. */
		buffer[j] &= mask;
		buffer[j] |= msb;
		mask >>= 1;
		msb  >>= 1;
	}

	/*
	** Last part, generate the first 250*250 random numbers 
	** to assure good results.
	*/
	for( i=0; i<(250*250); i++ )
		swRndGen( );
}


/* 
** Get the next random number from the random sequence. 
** The value returned will be between 0 and 0xffff. 
*/
unsigned short swRndGen( void )
{
	unsigned short j, newRand;

	if( index >= 147 )
		j = index - 147;
	else
		j = index + 103;

	newRand = buffer[index] ^ buffer[j];
	buffer[index] = newRand;

	if( index >= 249 )
		index = 0;
	else
		index++;

	return(newRand);
}


float swRndGen11( void )
{
    unsigned short rnd = swRndGen();
    short rndSigned;
    float rndNormal;
    
    rndSigned = (short)rnd;
    rndNormal = (float)rndSigned / (float)(0xffff);
    
    return rndNormal;
}


float swRndGen01( void )
{
    unsigned short rnd = swRndGen();
    float rndNormal;
    
    rndNormal = (float)rnd / (float)(0xffff);
    
    return rndNormal;
}


