/*
** PROLOGUE:
**
**
*/

/*
** PREREQUISITES
*/
#include <stdlib.h>
#include <assert.h>

#include "sw_gfx.h"
#include "sw_anm.h"
#include "sw_spr.h"
//#include "sw_snd.h"
#include "sw_img.h"
#include "sw_tex.h"
//#include "sw_par.h"
#include "sw_app.h"


/*
** DEFINITIONS
*/


/*
** LOCAL VARIABLES
*/


/*
** EXPORTED FUNCTIONS
*/

void swInit( void )
{
    swGfxInit( );
    //swSndInit( );
    swTexInit( );
    swImgInit( );
    swSprInit( );
    swAnmInit( );
    //swParInit( );
}


void swFree( void )
{
    //swParFree( );
    swAnmFree( );
    swSprFree( );        
    swImgFree( );        
    swTexFree( );           
    //swSndFree( );
    swGfxFree( );               
}
    
