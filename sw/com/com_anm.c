/*
** Module Prefix : swAnm
**
** PROLOGUE:
**
**
*/

/*
** PREREQUISITES
*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "sw_cfg.h"
#include "sw_def.h"
#include "sw_dbg.h"
#include "sw_spr.h"
#include "sw_anm.h"


/*
** DEFINITIONS
*/
#define ANM_INUSE      (1<<0)
#define ANM_LOOP       (1<<1)
#define ANM_ONESHOT    (1<<2)
#define ANM_REVERSE    (1<<3)

#define ANM_KF_POS_X   (1<<0)
#define ANM_KF_POS_XA  (1<<1)
#define ANM_KF_POS_Y   (1<<2)
#define ANM_KF_POS_YA  (1<<3)
#define ANM_KF_OFS_X   (1<<4)
#define ANM_KF_OFS_XA  (1<<5)
#define ANM_KF_OFS_Y   (1<<6)
#define ANM_KF_OFS_YA  (1<<7)
#define ANM_KF_AOF_X   (1<<8)
#define ANM_KF_AOF_XA  (1<<9)
#define ANM_KF_AOF_Y   (1<<10)
#define ANM_KF_AOF_YA  (1<<11)
#define ANM_KF_SCL_X   (1<<12)
#define ANM_KF_SCL_XA  (1<<13)
#define ANM_KF_SCL_Y   (1<<14)
#define ANM_KF_SCL_YA  (1<<15)
#define ANM_KF_ROT_Z   (1<<16)
#define ANM_KF_ROT_ZA  (1<<17)
#define ANM_KF_FADE    (1<<18)
#define ANM_KF_FADEA   (1<<19)
#define ANM_KF_IMG     (1<<20)
#define ANM_KF_IMGV    (1<<21)
#define ANM_KF_IMGRV   (1<<22)
#define ANM_KF_CLP_X   (1<<23)
#define ANM_KF_CLP_Y   (1<<24)

struct _ANM_KEYF_S
{
    int flags;
    int duration;
    int easing;
    int next;
    int prev;

    float pos_x;
    float pos_y;
    float off_x;
    float off_y;
    float aof_x;
    float aof_y;
    float scl_x;
    float scl_y;
    float rot_z;
    float clp_x;
    float clp_y;
    int   alpha;
    int   imagec;

    SWimage   imagei;
    SWimage * imagev;
    int     * indexv;
    
    SWanimcb * pcallback;
    int        opaque;
};
typedef struct _ANM_KEYF_S ANM_KEYF_S;

struct _ANM_RT_S
{
    int inuse;
    int flags;
    int reverse;
    int duration;
    int iteration;
    int easing;
    int sprite;
    int keyf;
    int anim;
    int paused;
    int frame;
    int loopc;
    
    char cid;

    float pos_x[2];
    float pos_y[2];
    float off_x[2];
    float off_y[2];
    float aof_x[2];
    float aof_y[2];
    float scl_x[2];
    float scl_y[2];
    float rot_z[2];
    float clp_x[2];
    float clp_y[2];
    int   alpha[2];
    int   image[2];
        /* These variables store the changes to be applied for 
        ** this keyframe to each property. The array contains two values.
        ** The first value is the starting or base value of the property,
        ** and the second is the amount of change to apply.
        */
    
    SWimage * imagev;
    int     * indexv;    
};
typedef struct _ANM_RT_S ANM_RT_S;

struct _ANM_S
{
    int flags;
    int keyf_f;
    int keyf_l;
    int loopc;
    SWanimcb* pcallback;
    int opaque;
};
typedef struct _ANM_S ANM_S;


/*
** LOCAL VARIABLES
*/
static ANM_KEYF_S ga_key[ SW_ANM_KEYF_MAX  ] = {0};
static ANM_RT_S   ga_rta[ SW_ANM_RTANM_MAX ] = {0};
static ANM_S      ga_anm[ SW_ANM_ANIMS_MAX ] = {0};
static int        g_free_keyframe = -1;


/*
** LOCAL FUNCTIONS
*/
static int flagSwap( int flags, int on, int off )
{
    flags &= ~(off);
    flags |= on;
    return flags;
}

static void clearKeyframe( int keyf )
{
    ga_key[keyf].flags    = 0;
    ga_key[keyf].duration = 0;
    ga_key[keyf].easing   = SW_ANM_EASE_LINEAR;
    ga_key[keyf].prev     = 0;
    ga_key[keyf].next     = g_free_keyframe;

    ga_key[keyf].pos_x = 0.0f;
    ga_key[keyf].pos_y = 0.0f;
    ga_key[keyf].off_x = 0.0f;
    ga_key[keyf].off_y = 0.0f;
    ga_key[keyf].aof_x = 0.0f;
    ga_key[keyf].aof_y = 0.0f;
    ga_key[keyf].scl_x = 0.0f;
    ga_key[keyf].scl_y = 0.0f;
    ga_key[keyf].rot_z = 0.0f;
    ga_key[keyf].clp_x = 0.0f;
    ga_key[keyf].clp_y = 0.0f;
    ga_key[keyf].alpha = 0;

    ga_key[keyf].imagei = 0;
    ga_key[keyf].imagev = 0;
    ga_key[keyf].imagec = 0;
    
    ga_key[keyf].pcallback = NULL;
    ga_key[keyf].opaque    = 0;

    g_free_keyframe = keyf;
}

static void initKeyframes( void )
{
    int keyf;

    g_free_keyframe = 0;
    clearKeyframe( 0 );

    for( keyf=1; keyf<SW_ANM_KEYF_MAX; keyf++ )
        clearKeyframe( keyf );
}

static void clearAnim( SWanim anim )
{
    int keyf, keyf_n;
    
    keyf = ga_anm[anim].keyf_f;
    while( keyf != 0 )
    {
        keyf_n = ga_key[keyf].next;
        clearKeyframe( keyf );
        keyf = keyf_n;
    }
    ga_anm[anim].keyf_f = 0;
    ga_anm[anim].keyf_l = 0;
    
    ga_anm[anim].flags = 0;
    ga_anm[anim].pcallback = NULL;
    ga_anm[anim].opaque = 0;    
}


/*
** Easing functions.
** These calculate only the affect of t on the final result.
** The value of t is normalized.
**
** The actual effect is given by ..
** value = starting_value + (iteration_value * ease),
** where ease is the value returned from one of these functions.
*/
static float easeLinear( float t )
{
    return t;
}

static float easeInOut( float t )
{
    return ((-2*t + 3) * (t*t));
}

static float easeIn( float t )
{
    return (t*t*t);
}

static float easeOut( float t )
{
    return ((t*t*t) - (3*t*t) + (3*t));
}

static float easeBack( float t )
{
	return ((4*t*t*t) - (3*t*t));    
}

static float easeElastic( float t )
{
    float t2 = t*t;
    float t3 = t2 * t;
    return ((33*t3*t2) - (106*t2*t2) + (126*t3) - (67*t2) + (15*t));
}


static void rtaSetKeyframe( SWrtanim rt_anim, int keyf )
{
    ANM_RT_S* prta = &ga_rta[rt_anim];
    int        rev = prta->reverse;

    prta->flags     = ga_key[keyf].flags;
    prta->easing    = ga_key[keyf].easing;
    prta->duration  = ga_key[keyf].duration;
    prta->keyf      = keyf;
    prta->iteration = 1;
    
    SWsprite spr = prta->sprite;
    
    if( ga_key[keyf].pcallback )
        ga_key[keyf].pcallback( ga_key[keyf].opaque );

    if( ga_key[keyf].flags & ANM_KF_POS_X )
    {
        prta->pos_x[0] = swSprGetPosX(spr);
        prta->pos_x[1] = (rev ? -ga_key[keyf].pos_x : ga_key[keyf].pos_x);
    }
    
    if( ga_key[keyf].flags & ANM_KF_POS_XA )
    {
        float x0 = swSprGetPosX(spr);
        float xd = ga_key[keyf].pos_x - x0;
        
        prta->pos_x[0] = x0;
        prta->pos_x[1] = (rev ? -xd : xd);
        
        prta->flags = flagSwap( prta->flags, ANM_KF_POS_X, ANM_KF_POS_XA );
    }
    
    if( ga_key[keyf].flags & ANM_KF_POS_Y )
    {
        prta->pos_y[0] = swSprGetPosY(spr);
        prta->pos_y[1] = (rev ? -ga_key[keyf].pos_y : ga_key[keyf].pos_y);
    }
    
    if( ga_key[keyf].flags & ANM_KF_POS_YA )
    {
        float y0 = swSprGetPosY(spr);
        float yd = ga_key[keyf].pos_y - y0;
        
        prta->pos_y[0] = y0;
        prta->pos_y[1] = (rev ? -yd : yd);
        
        prta->flags = flagSwap( prta->flags, ANM_KF_POS_Y, ANM_KF_POS_YA );        
    }
    
    if( ga_key[keyf].flags & ANM_KF_OFS_X )
    {
        prta->off_x[0] = swSprGetOffsX(spr);
        prta->off_x[1] = (rev ? -ga_key[keyf].off_x : ga_key[keyf].off_x);
    }
    
    if( ga_key[keyf].flags & ANM_KF_OFS_XA )
    {
        float x0 = swSprGetOffsX(spr);
        float xd = ga_key[keyf].off_x - x0;
        
        prta->off_x[0] = x0;
        prta->off_x[1] = (rev ? -xd : xd);
        
        prta->flags = flagSwap( prta->flags, ANM_KF_OFS_X, ANM_KF_OFS_XA );        
    }
    
    if( ga_key[keyf].flags & ANM_KF_OFS_Y )
    {
        prta->off_y[0] = swSprGetOffsY(spr);
        prta->off_y[1] = (rev ? -ga_key[keyf].off_y : ga_key[keyf].off_y);
    }
    
    if( ga_key[keyf].flags & ANM_KF_OFS_YA )
    {
        float y0 = swSprGetOffsY(spr);
        float yd = ga_key[keyf].off_y - y0;
        
        prta->off_y[0] = y0;
        prta->off_y[1] = (rev ? -yd : yd);
        
        prta->flags = flagSwap( prta->flags, ANM_KF_OFS_Y, ANM_KF_OFS_YA );                
    }
    
    if( ga_key[keyf].flags & ANM_KF_AOF_X )
    {
        prta->aof_x[0] = swSprGetAnchorOffsX(spr);
        prta->aof_x[1] = (rev ? -ga_key[keyf].aof_x : ga_key[keyf].aof_x);
    }
    
    if( ga_key[keyf].flags & ANM_KF_AOF_XA )
    {
        float x0 = swSprGetAnchorOffsX(spr);
        float xd = ga_key[keyf].aof_x - x0;
        
        prta->aof_x[0] = x0;
        prta->aof_x[1] = (rev ? -xd : xd);
        
        prta->flags = flagSwap( prta->flags, ANM_KF_AOF_X, ANM_KF_AOF_XA );                
    }
    
    if( ga_key[keyf].flags & ANM_KF_AOF_Y )
    {
        prta->aof_y[0] = swSprGetAnchorOffsY(spr);
        prta->aof_y[1] = (rev ? -ga_key[keyf].aof_y : ga_key[keyf].aof_y);
    }
    
    if( ga_key[keyf].flags & ANM_KF_AOF_YA )
    {
        float y0 = swSprGetAnchorOffsY(spr);
        float yd = ga_key[keyf].aof_y - y0;
        
        prta->aof_y[0] = y0;
        prta->aof_y[1] = (rev ? -yd : yd);
        
        prta->flags = flagSwap( prta->flags, ANM_KF_AOF_Y, ANM_KF_AOF_YA );        
    }
        
    if( ga_key[keyf].flags & ANM_KF_ROT_Z )
    {
        prta->rot_z[0] = swSprGetRotate(spr);
        prta->rot_z[1] = (rev ? -ga_key[keyf].rot_z : ga_key[keyf].rot_z);
    }
    
    if( ga_key[keyf].flags & ANM_KF_ROT_ZA )
    {
        float z0 = swSprGetRotate(spr);
        float zd = ga_key[keyf].rot_z - z0;
        
        prta->rot_z[0] = z0;
        prta->rot_z[1] = (rev ? -zd : zd);
        
        prta->flags = flagSwap( prta->flags, ANM_KF_ROT_Z, ANM_KF_ROT_ZA );                        
    }    

    if( ga_key[keyf].flags & ANM_KF_SCL_X )
    {
        prta->scl_x[0] = swSprGetScaleX(spr);
        prta->scl_x[1] = (rev ? -ga_key[keyf].scl_x : ga_key[keyf].scl_x);
    }
    
    if( ga_key[keyf].flags & ANM_KF_SCL_XA )
    {
        float s0 = swSprGetScaleX(spr);
        float sd = ga_key[keyf].scl_x - s0;
        
        prta->scl_x[0] = s0;
        prta->scl_x[1] = (rev ? -sd : sd);
        
        prta->flags = flagSwap( prta->flags, ANM_KF_SCL_X, ANM_KF_SCL_XA );                        
    }    

    if( ga_key[keyf].flags & ANM_KF_SCL_Y )
    {
        prta->scl_y[0] = swSprGetScaleY(spr);
        prta->scl_y[1] = (rev ? -ga_key[keyf].scl_y : ga_key[keyf].scl_y);
    }
    
    if( ga_key[keyf].flags & ANM_KF_SCL_YA )
    {
        float s0 = swSprGetScaleY(spr);
        float sd = ga_key[keyf].scl_y - s0;
        
        prta->scl_y[0] = s0;
        prta->scl_y[1] = (rev ? -sd : sd);
        
        prta->flags = flagSwap( prta->flags, ANM_KF_SCL_Y, ANM_KF_SCL_YA );                                
    }    
        
    if( ga_key[keyf].flags & ANM_KF_FADE )
    {
    	unsigned int colors[4];
        swSprGetColor( spr, colors );
        prta->alpha[0] = colors[3];
        prta->alpha[1] = (rev ? -ga_key[keyf].alpha : ga_key[keyf].alpha);            
    }
    
    if( ga_key[keyf].flags & ANM_KF_FADEA )
    {
        int cd;
    	unsigned int colors[4];
        swSprGetColor( spr, colors );
        cd = ga_key[keyf].alpha - colors[3];
        
        prta->alpha[0] = colors[3];
        prta->alpha[1] = (rev ? -cd : cd);            
        
        prta->flags = flagSwap( prta->flags, ANM_KF_FADE, ANM_KF_FADEA );        
    }
    
    if( ga_key[keyf].flags & ANM_KF_CLP_X )
    {
        prta->clp_x[0] = swSprGetClipX(spr);
        prta->clp_x[1] = (rev ? -ga_key[keyf].clp_x : ga_key[keyf].clp_x);
    }
    
    if( ga_key[keyf].flags & ANM_KF_CLP_Y )
    {
        prta->clp_y[0] = swSprGetClipY(spr);
        prta->clp_y[1] = (rev ? -ga_key[keyf].clp_y : ga_key[keyf].clp_y);
    }
    
    if( ga_key[keyf].flags & ANM_KF_IMG )
    {
        swSprSetImage( spr, ga_key[keyf].imagei );
    }
    
    if( ga_key[keyf].flags & ANM_KF_IMGV )
    {
        prta->image[0] = 0;
        prta->image[1] = ga_key[keyf].imagec;
        prta->imagev   = ga_key[keyf].imagev;
        prta->indexv   = ga_key[keyf].indexv;        
    }
    
    if( ga_key[keyf].flags & ANM_KF_IMGRV )
    {
        prta->image[0] = ga_key[keyf].imagec-1;
        prta->image[1] = ga_key[keyf].imagec;
        prta->imagev   = ga_key[keyf].imagev;
        prta->indexv   = ga_key[keyf].indexv;                
    }    
}

static void rtaStop( SWrtanim rt_anim )
{
    if( ga_anm[ga_rta[rt_anim].anim].pcallback ) 
        ga_anm[ga_rta[rt_anim].anim].pcallback( ga_anm[ga_rta[rt_anim].anim].opaque );

    ga_rta[rt_anim].inuse = 0;
	ga_rta[rt_anim].duration = 0;
    ga_rta[rt_anim].frame = -1;
}

static void rtaHalt( SWrtanim rt_anim )
{
    ga_rta[rt_anim].inuse = 0;
	ga_rta[rt_anim].duration = 0;
    
}

/*
** This routine is called from inside rtaTick when the
** runtime has completed iteration over all of its keyframes.
** It decides what the next course of action for the runtime.
*/
static void rtaEnd( SWrtanim rt_anim )
{
    ANM_RT_S* prta = &ga_rta[rt_anim];
    
    prta->keyf = 0;
        
    /*
    ** The runtime is continuing and now needs to determine 
    ** its next course of action. The runtime will set its keyframe
    ** value to a valid keyframe if it is to continue.
    */
    if( ga_anm[prta->anim].flags & ANM_LOOP ) 
    {
        if( prta->loopc == 0 )
        {
            rtaStop( rt_anim );
            return;
        }
        if( prta->reverse == 0 )
            prta->keyf = ga_anm[prta->anim].keyf_f;
        else
            prta->keyf = ga_anm[prta->anim].keyf_l;
        
        if( prta->loopc > 0 )
            --prta->loopc;
    }
    if( ga_anm[prta->anim].flags & ANM_ONESHOT )
    {
        prta->keyf = 0;
        swSprSetImage( prta->sprite, 0 );
    } 
        
    if( prta->keyf != 0 )
        rtaSetKeyframe( rt_anim, prta->keyf );    
    else 
        rtaStop( rt_anim );
}

/*
** Main processing routine for a runtime.
** Called from swAnmTick.
*/
static void rtaTick( SWrtanim rt_anim )
{
    ANM_RT_S* prta = &ga_rta[rt_anim];
    float ease = 0;
    float t;

    if( prta->iteration > prta->duration )
    {
        int keyf;
        
        if( prta->reverse )
            keyf = ga_key[prta->keyf].prev;
        else
            keyf = ga_key[prta->keyf].next;

        if( keyf == 0 )
        	rtaEnd( rt_anim );
        else
            rtaSetKeyframe( rt_anim, keyf );        
    }
    
    if( ga_rta[rt_anim].inuse == 0 )
    	return; /* Anim was stopped */
        
    if( prta->duration > 0 )
        t = (float)(prta->iteration) / prta->duration;
    else
        t = 1;
        
    switch( prta->easing )
    {
        case SW_ANM_EASE_INOUT:   ease = easeInOut( t );   break;
        case SW_ANM_EASE_IN:      ease = easeIn( t );      break;
        case SW_ANM_EASE_OUT:     ease = easeOut( t );     break;
        case SW_ANM_EASE_BACK:    ease = easeBack(t);      break;
        case SW_ANM_EASE_ELASTIC: ease = easeElastic( t ); break;
        default:                  ease = easeLinear( t );  break;
    }
        
    if( prta->flags & ANM_KF_POS_X )
    {
        float pos_x = prta->pos_x[0] + (prta->pos_x[1] * ease);
        swSprSetPosX( prta->sprite, pos_x );
    }
    
    if( prta->flags & ANM_KF_POS_Y )
    {
        float pos_y = prta->pos_y[0] + (prta->pos_y[1] * ease);
        swSprSetPosY( prta->sprite, pos_y );
    }
    
    if( prta->flags & ANM_KF_OFS_X )
    {
        float ofs_x = prta->off_x[0] + (prta->off_x[1] * ease);
        swSprSetOffsX( prta->sprite, ofs_x );
    }
    
    if( prta->flags & ANM_KF_OFS_Y )
    {
        float ofs_y = prta->off_y[0] + (prta->off_y[1] * ease);
        swSprSetOffsY( prta->sprite, ofs_y );
    }
    
    if( prta->flags & ANM_KF_AOF_X )
    {
        float aof_x = prta->aof_x[0] + (prta->aof_x[1] * ease);
        swSprSetAnchorOffsX( prta->sprite, aof_x );
    }

    if( prta->flags & ANM_KF_AOF_Y )
    {
        float aof_y = prta->aof_y[0] + (prta->aof_y[1] * ease);
        swSprSetAnchorOffsY( prta->sprite, aof_y );
    }
    
    if( prta->flags & ANM_KF_ROT_Z )
    {
        float rot_z = prta->rot_z[0] + (prta->rot_z[1] * ease);
        swSprSetRotate( prta->sprite, rot_z );
    }

    if( prta->flags & ANM_KF_SCL_X )
    {
        float scl_x = prta->scl_x[0] + (prta->scl_x[1] * ease);
        swSprSetScaleX( prta->sprite, scl_x );
    }

    if( prta->flags & ANM_KF_SCL_Y )
    {
        float scl_y = prta->scl_y[0] + (prta->scl_y[1] * ease);
        swSprSetScaleY( prta->sprite, scl_y );
    }
    
    if( prta->flags & ANM_KF_CLP_X )
    {
        float clp_x = prta->clp_x[0] + (prta->clp_x[1] * ease);
        swSprSetClipX( prta->sprite, clp_x );
    }

    if( prta->flags & ANM_KF_CLP_Y )
    {
        float clp_y = prta->clp_y[0] + (prta->clp_y[1] * ease);
        swSprSetClipY( prta->sprite, clp_y );
    }    
    
    if( prta->flags & ANM_KF_FADE )
    {
        int alpha = prta->alpha[0] + (prta->alpha[1] * ease);
        swSprSetColor( prta->sprite, alpha, alpha, alpha, alpha );
    }
    
    if( prta->flags & ANM_KF_IMGV )
    {
        int frm = prta->image[0] + (int)(prta->image[1] * ease); frm = SW_MIN( frm, prta->image[1]-1 );
        int img = prta->indexv ? prta->indexv[frm] : frm;
        swSprSetImage( prta->sprite, prta->imagev[img] );
        prta->frame = frm;
    }
    
    if( prta->flags & ANM_KF_IMGRV )
    {
        int frm = prta->image[0] - (int)(prta->image[1] * ease); frm = SW_MIN( frm, prta->image[1]-1 );
        int img = prta->indexv ? prta->indexv[frm] : frm;
        swSprSetImage( prta->sprite, prta->imagev[img] );
        prta->frame = frm;
    }
            
    prta->iteration++;
}


/*
** EXPORTED FUNCTIONS
*/

int swAnmInit( void )
{
    initKeyframes();
    return 1;
}

void swAnmFree( void )
{    
}


int swAnmGen( int num_anims, SWanim * anims )
{
    int anm, i;

    assert( num_anims >= 0 );
    assert( anims != NULL );

    for( anm=0; anm<num_anims; anm++ )
    {
        anims[anm] = 0;
        for( i=1; i<SW_ANM_ANIMS_MAX; i++ )
        {
            if( ga_anm[i].flags == 0 )
                break;
        }
        if( i >= SW_ANM_ANIMS_MAX )
        {
            SW_ERR( "Animations exhausted!\n");
            return anm;
        }

        ga_anm[i].flags = ANM_INUSE;
        ga_anm[i].keyf_f = 0;
        ga_anm[i].keyf_l = 0;
        ga_anm[i].pcallback = NULL;
        ga_anm[i].opaque = 0;
        
        anims[anm] = i;
    }
    return anm;
}

void swAnmDel( int num_anims, SWanim* anims )
{
    int anm;

    assert( num_anims >= 0 );
    assert( anims != NULL );

    for( anm=0; anm<num_anims; anm++ )
    {
        if( anims[anm] < SW_ANM_ANIMS_MAX )
        	clearAnim( anims[anm] );
    }
}


/*
** Adds a new keyframe to the bound anim.
** Will not add keyframes to the null anim (0).
** Because this function only works on legally bound anims
** the value of ga_anm[anim].keyf_l is updated to the newly added keyframe.
*/
void swAnmKeyframe( SWanim anim, int duration )
{
    if( g_free_keyframe != 0 && anim != 0 )
    {
        int keyf_n = g_free_keyframe;

        g_free_keyframe = ga_key[g_free_keyframe].next;

        if( ga_anm[anim].keyf_f == 0 )
            ga_anm[anim].keyf_f = keyf_n;

        if( ga_anm[anim].keyf_l != 0 )
            ga_key[ga_anm[anim].keyf_l].next = keyf_n;

        ga_key[keyf_n].duration = duration;
        ga_key[keyf_n].prev = ga_anm[anim].keyf_l;
        ga_key[keyf_n].next = 0;

        ga_anm[anim].keyf_l = keyf_n;
        ga_anm[anim].keyf_l = keyf_n;
    }
}


/*
** The following routines operate directly on either
** the bound anim or the bound keyframe. All can safely modify
** the null anim or keyframe.
*/
void swAnmClear( SWanim anim )
{
    assert( anim < SW_ANM_ANIMS_MAX );
    assert( ga_anm[anim].flags & ANM_INUSE );

	clearAnim( anim );
	ga_anm[anim].flags |= ANM_INUSE;
}

void swAnmLoop( SWanim anim, int loop_count )
{
    assert( anim < SW_ANM_ANIMS_MAX );
    assert( ga_anm[anim].flags & ANM_INUSE );
    
	ga_anm[anim].flags |= ANM_LOOP;
    ga_anm[anim].loopc = loop_count;
}

void swAnmReverse( SWanim anim )
{
    assert( anim < SW_ANM_ANIMS_MAX );
    assert( ga_anm[anim].flags & ANM_INUSE );
    
   	ga_anm[anim].flags |= ANM_REVERSE;
}

void swAnmOneShot( SWanim anim )
{
    assert( anim < SW_ANM_ANIMS_MAX );
    assert( ga_anm[anim].flags & ANM_INUSE );
    
   	ga_anm[anim].flags |= ANM_ONESHOT;    
}

void swAnmCallback( SWanim anim, SWanimcb* pfunc, int opaque )
{
    assert( anim < SW_ANM_ANIMS_MAX );
    assert( ga_anm[anim].flags & ANM_INUSE );
    
    if( ga_anm[anim].keyf_l != 0 )
    {
        ga_key[ga_anm[anim].keyf_l].pcallback = pfunc;
        ga_key[ga_anm[anim].keyf_l].opaque    = opaque;
    }
    else
    {
        ga_anm[anim].pcallback = pfunc;
        ga_anm[anim].opaque = opaque;
    }
}

void swAnmEase( SWanim anim, SWeasing ease )
{
    assert( anim < SW_ANM_ANIMS_MAX );
    assert( ga_anm[anim].flags & ANM_INUSE );
    
    ga_key[ga_anm[anim].keyf_l].easing = ease;
}

void swAnmImage( SWanim anim, SWimage image )
{
    assert( anim < SW_ANM_ANIMS_MAX );
    assert( ga_anm[anim].flags & ANM_INUSE );

    ga_key[ga_anm[anim].keyf_l].flags |= ANM_KF_IMG;
    ga_key[ga_anm[anim].keyf_l].imagei = image;
    ga_key[ga_anm[anim].keyf_l].imagec = 1;
}

void swAnmImagev( SWanim anim, int num_images, SWimage* imagev )
{
    assert( anim < SW_ANM_ANIMS_MAX );
    assert( ga_anm[anim].flags & ANM_INUSE );

    ga_key[ga_anm[anim].keyf_l].flags |= ANM_KF_IMGV;
    ga_key[ga_anm[anim].keyf_l].imagev = imagev;
    ga_key[ga_anm[anim].keyf_l].indexv = NULL;      
    ga_key[ga_anm[anim].keyf_l].imagec = num_images;
}

void swAnmImagerv( SWanim anim, int num_images, SWimage* imagev )
{
    assert( anim < SW_ANM_ANIMS_MAX );
    assert( ga_anm[anim].flags & ANM_INUSE );

    ga_key[ga_anm[anim].keyf_l].flags |= ANM_KF_IMGRV;
    ga_key[ga_anm[anim].keyf_l].imagev = imagev;
    ga_key[ga_anm[anim].keyf_l].indexv = NULL;    
    ga_key[ga_anm[anim].keyf_l].imagec = num_images;
}

void swAnmImagevi( SWanim anim, int num_images, int * indexv, SWimage* imagev )
{
    assert( anim < SW_ANM_ANIMS_MAX );
    assert( ga_anm[anim].flags & ANM_INUSE );

    ga_key[ga_anm[anim].keyf_l].flags |= ANM_KF_IMGV;
    ga_key[ga_anm[anim].keyf_l].imagev = imagev;
    ga_key[ga_anm[anim].keyf_l].indexv = indexv;
    ga_key[ga_anm[anim].keyf_l].imagec = num_images;    
}



/*
** swAnmMove
*/
void swAnmMove( SWanim anim, float x, float y )
{
    assert( anim < SW_ANM_ANIMS_MAX );
    assert( ga_anm[anim].flags & ANM_INUSE );

    ga_key[ga_anm[anim].keyf_l].flags &= ~(ANM_KF_POS_XA|ANM_KF_POS_YA);
    ga_key[ga_anm[anim].keyf_l].flags |= (ANM_KF_POS_X|ANM_KF_POS_Y);
    ga_key[ga_anm[anim].keyf_l].pos_x = x;
    ga_key[ga_anm[anim].keyf_l].pos_y = y;
}

void swAnmMoveX( SWanim anim, float x )
{
    assert( anim < SW_ANM_ANIMS_MAX );
    assert( ga_anm[anim].flags & ANM_INUSE );

    ga_key[ga_anm[anim].keyf_l].flags &= ~(ANM_KF_POS_XA);
    ga_key[ga_anm[anim].keyf_l].flags |= (ANM_KF_POS_X);	
    ga_key[ga_anm[anim].keyf_l].pos_x = x;
}

void swAnmMoveY( SWanim anim, float y )
{
    assert( anim < SW_ANM_ANIMS_MAX );
    assert( ga_anm[anim].flags & ANM_INUSE );

    ga_key[ga_anm[anim].keyf_l].flags &= ~(ANM_KF_POS_YA);
    ga_key[ga_anm[anim].keyf_l].flags |= (ANM_KF_POS_Y);	    
    ga_key[ga_anm[anim].keyf_l].pos_y = y;
}

void swAnmMoveTo( SWanim anim, float x, float y )
{
    assert( anim < SW_ANM_ANIMS_MAX );
    assert( ga_anm[anim].flags & ANM_INUSE );

    ga_key[ga_anm[anim].keyf_l].flags &= ~(ANM_KF_POS_X|ANM_KF_POS_Y);
    ga_key[ga_anm[anim].keyf_l].flags |= (ANM_KF_POS_XA|ANM_KF_POS_YA);
    ga_key[ga_anm[anim].keyf_l].pos_x = x;
    ga_key[ga_anm[anim].keyf_l].pos_y = y;
}

void swAnmMoveToX( SWanim anim, float x )
{
    assert( anim < SW_ANM_ANIMS_MAX );
    assert( ga_anm[anim].flags & ANM_INUSE );

    ga_key[ga_anm[anim].keyf_l].flags &= ~(ANM_KF_POS_X);
    ga_key[ga_anm[anim].keyf_l].flags |= (ANM_KF_POS_XA);	
	ga_key[ga_anm[anim].keyf_l].pos_x = x;
}

void swAnmMoveToY( SWanim anim, float y )
{
    assert( anim < SW_ANM_ANIMS_MAX );
    assert( ga_anm[anim].flags & ANM_INUSE );

    ga_key[ga_anm[anim].keyf_l].flags &= ~(ANM_KF_POS_Y);
    ga_key[ga_anm[anim].keyf_l].flags |= (ANM_KF_POS_YA);	 
    ga_key[ga_anm[anim].keyf_l].pos_y = y;
}


/*
 ** swAnmOffset
 */
void swAnmOffset( SWanim anim, float x, float y )
{
    assert( anim < SW_ANM_ANIMS_MAX );
    assert( ga_anm[anim].flags & ANM_INUSE );

    ga_key[ga_anm[anim].keyf_l].flags &= ~(ANM_KF_OFS_XA|ANM_KF_OFS_YA);
    ga_key[ga_anm[anim].keyf_l].flags |= (ANM_KF_OFS_X|ANM_KF_OFS_Y);
    ga_key[ga_anm[anim].keyf_l].off_x = x;
    ga_key[ga_anm[anim].keyf_l].off_y = y;
}

void swAnmOffsetX( SWanim anim, float x )
{
    assert( anim < SW_ANM_ANIMS_MAX );
    assert( ga_anm[anim].flags & ANM_INUSE );

    ga_key[ga_anm[anim].keyf_l].flags &= ~(ANM_KF_OFS_XA);
    ga_key[ga_anm[anim].keyf_l].flags |= (ANM_KF_OFS_X);	
    ga_key[ga_anm[anim].keyf_l].off_x = x;
}

void swAnmOffsetY( SWanim anim, float y )
{
    assert( anim < SW_ANM_ANIMS_MAX );
    assert( ga_anm[anim].flags & ANM_INUSE );

    ga_key[ga_anm[anim].keyf_l].flags &= ~(ANM_KF_OFS_YA);
    ga_key[ga_anm[anim].keyf_l].flags |= (ANM_KF_OFS_Y);	    
    ga_key[ga_anm[anim].keyf_l].off_y = y;
}

void swAnmOffsetTo( SWanim anim, float x, float y )
{
    assert( anim < SW_ANM_ANIMS_MAX );
    assert( ga_anm[anim].flags & ANM_INUSE );

    ga_key[ga_anm[anim].keyf_l].flags &= ~(ANM_KF_OFS_X|ANM_KF_OFS_Y);
    ga_key[ga_anm[anim].keyf_l].flags |= (ANM_KF_OFS_XA|ANM_KF_OFS_YA);
    ga_key[ga_anm[anim].keyf_l].off_x = x;
    ga_key[ga_anm[anim].keyf_l].off_y = y;
}

void swAnmOffsetToX( SWanim anim, float x )
{
    assert( anim < SW_ANM_ANIMS_MAX );
    assert( ga_anm[anim].flags & ANM_INUSE );

    ga_key[ga_anm[anim].keyf_l].flags &= ~(ANM_KF_OFS_X);
    ga_key[ga_anm[anim].keyf_l].flags |= (ANM_KF_OFS_XA);	
	ga_key[ga_anm[anim].keyf_l].off_x = x;
}

void swAnmOffsetToY( SWanim anim, float y )
{
    assert( anim < SW_ANM_ANIMS_MAX );
    assert( ga_anm[anim].flags & ANM_INUSE );

    ga_key[ga_anm[anim].keyf_l].flags &= ~(ANM_KF_OFS_Y);
    ga_key[ga_anm[anim].keyf_l].flags |= (ANM_KF_OFS_YA);	 
    ga_key[ga_anm[anim].keyf_l].off_y = y;
}


/*
** swAnmAnchorOffset
*/
void swAnmAnchorOffset( SWanim anim, float x, float y )
{
    assert( anim < SW_ANM_ANIMS_MAX );
    assert( ga_anm[anim].flags & ANM_INUSE );

    ga_key[ga_anm[anim].keyf_l].flags &= ~(ANM_KF_AOF_XA|ANM_KF_AOF_YA);
    ga_key[ga_anm[anim].keyf_l].flags |= (ANM_KF_AOF_X|ANM_KF_AOF_Y);
    ga_key[ga_anm[anim].keyf_l].aof_x = x;
	ga_key[ga_anm[anim].keyf_l].aof_y = y;
}

void swAnmAnchorOffsetX( SWanim anim, float x )
{
    assert( anim < SW_ANM_ANIMS_MAX );
    assert( ga_anm[anim].flags & ANM_INUSE );

    ga_key[ga_anm[anim].keyf_l].flags &= ~(ANM_KF_AOF_XA);
    ga_key[ga_anm[anim].keyf_l].flags |= (ANM_KF_AOF_X);
    ga_key[ga_anm[anim].keyf_l].aof_x = x;
}

void swAnmAnchorOffsetY( SWanim anim, float y )
{
    assert( anim < SW_ANM_ANIMS_MAX );
    assert( ga_anm[anim].flags & ANM_INUSE );

    ga_key[ga_anm[anim].keyf_l].flags &= ~(ANM_KF_AOF_YA);
    ga_key[ga_anm[anim].keyf_l].flags |= (ANM_KF_AOF_Y);
    ga_key[ga_anm[anim].keyf_l].aof_y = y;
}

void swAnmAnchorOffsetTo( SWanim anim, float x, float y )
{
    assert( anim < SW_ANM_ANIMS_MAX );
    assert( ga_anm[anim].flags & ANM_INUSE );

    ga_key[ga_anm[anim].keyf_l].flags &= (ANM_KF_AOF_X|ANM_KF_AOF_Y);
    ga_key[ga_anm[anim].keyf_l].flags |= (ANM_KF_AOF_XA|ANM_KF_AOF_YA);
    ga_key[ga_anm[anim].keyf_l].aof_x = x;
	ga_key[ga_anm[anim].keyf_l].aof_y = y;
}

void swAnmAnchorOffsetToX( SWanim anim, float x )
{
    assert( anim < SW_ANM_ANIMS_MAX );
    assert( ga_anm[anim].flags & ANM_INUSE );

    ga_key[ga_anm[anim].keyf_l].flags &= ~(ANM_KF_AOF_X);
    ga_key[ga_anm[anim].keyf_l].flags |= (ANM_KF_AOF_XA);
    ga_key[ga_anm[anim].keyf_l].aof_x = x;
}

void swAnmAnchorOffsetToY( SWanim anim, float y )
{
    assert( anim < SW_ANM_ANIMS_MAX );
    assert( ga_anm[anim].flags & ANM_INUSE );

    ga_key[ga_anm[anim].keyf_l].flags &= ~(ANM_KF_AOF_Y);
    ga_key[ga_anm[anim].keyf_l].flags |= (ANM_KF_AOF_YA);
    ga_key[ga_anm[anim].keyf_l].aof_y = y;
}


/*
** swAnmRotate
*/
void swAnmRotate( SWanim anim, float angle )
{
    assert( anim < SW_ANM_ANIMS_MAX );
    assert( ga_anm[anim].flags & ANM_INUSE );

	ga_key[ga_anm[anim].keyf_l].flags &= ~(ANM_KF_ROT_ZA);
	ga_key[ga_anm[anim].keyf_l].flags |= (ANM_KF_ROT_Z);
    ga_key[ga_anm[anim].keyf_l].rot_z = angle;
}

void swAnmRotateTo( SWanim anim, float angle )
{
    assert( anim < SW_ANM_ANIMS_MAX );
    assert( ga_anm[anim].flags & ANM_INUSE );

	ga_key[ga_anm[anim].keyf_l].flags &= ~(ANM_KF_ROT_Z);
	ga_key[ga_anm[anim].keyf_l].flags |= (ANM_KF_ROT_ZA);
    ga_key[ga_anm[anim].keyf_l].rot_z = angle;
}


/*
** swAnmScale
*/
void swAnmScale ( SWanim anim, float x_scale, float y_scale )
{
    assert( anim < SW_ANM_ANIMS_MAX );
    assert( ga_anm[anim].flags & ANM_INUSE );

    ga_key[ga_anm[anim].keyf_l].flags &= ~(ANM_KF_SCL_XA|ANM_KF_SCL_YA);
    ga_key[ga_anm[anim].keyf_l].flags |= (ANM_KF_SCL_X|ANM_KF_SCL_Y);
    ga_key[ga_anm[anim].keyf_l].scl_x = x_scale;
    ga_key[ga_anm[anim].keyf_l].scl_y = y_scale;
}

void swAnmScaleX( SWanim anim, float x_scale )
{
    assert( anim < SW_ANM_ANIMS_MAX );
    assert( ga_anm[anim].flags & ANM_INUSE );

    ga_key[ga_anm[anim].keyf_l].flags &= ~(ANM_KF_SCL_XA);
    ga_key[ga_anm[anim].keyf_l].flags |= (ANM_KF_SCL_X);
    ga_key[ga_anm[anim].keyf_l].scl_x = x_scale;
}

void swAnmScaleY( SWanim anim, float y_scale )
{
    assert( anim < SW_ANM_ANIMS_MAX );
    assert( ga_anm[anim].flags & ANM_INUSE );

    ga_key[ga_anm[anim].keyf_l].flags &= ~(ANM_KF_SCL_YA);
    ga_key[ga_anm[anim].keyf_l].flags |= (ANM_KF_SCL_Y);
    ga_key[ga_anm[anim].keyf_l].scl_y = y_scale;
}

void swAnmScaleTo( SWanim anim, float x_scale, float y_scale )
{
    assert( anim < SW_ANM_ANIMS_MAX );
    assert( ga_anm[anim].flags & ANM_INUSE );

    ga_key[ga_anm[anim].keyf_l].flags &= ~(ANM_KF_SCL_X|ANM_KF_SCL_Y);
    ga_key[ga_anm[anim].keyf_l].flags |= (ANM_KF_SCL_XA|ANM_KF_SCL_YA);
    ga_key[ga_anm[anim].keyf_l].scl_x = x_scale;
    ga_key[ga_anm[anim].keyf_l].scl_y = y_scale;
}

void swAnmScaleToX( SWanim anim, float x_scale )
{
    assert( anim < SW_ANM_ANIMS_MAX );
    assert( ga_anm[anim].flags & ANM_INUSE );

    ga_key[ga_anm[anim].keyf_l].flags &= ~(ANM_KF_SCL_X);
    ga_key[ga_anm[anim].keyf_l].flags |= (ANM_KF_SCL_XA);
    ga_key[ga_anm[anim].keyf_l].scl_x = x_scale;
}

void swAnmScaleToY( SWanim anim, float y_scale )
{
    assert( anim < SW_ANM_ANIMS_MAX );
    assert( ga_anm[anim].flags & ANM_INUSE );

    ga_key[ga_anm[anim].keyf_l].flags &= ~(ANM_KF_SCL_Y);
    ga_key[ga_anm[anim].keyf_l].flags |= (ANM_KF_SCL_YA);
    ga_key[ga_anm[anim].keyf_l].scl_y = y_scale;
}

/*
** Routines used for controlling the sprite clipping
** region.
*/
void swAnmClip( SWanim anim, float clip_x, float clip_y )
{
    assert( anim < SW_ANM_ANIMS_MAX );
    assert( ga_anm[anim].flags & ANM_INUSE );

    ga_key[ga_anm[anim].keyf_l].flags |= (ANM_KF_CLP_X|ANM_KF_CLP_Y);
    ga_key[ga_anm[anim].keyf_l].clp_x = clip_x;
    ga_key[ga_anm[anim].keyf_l].clp_y = clip_y;    
}

void swAnmClipX( SWanim anim, float clip_x )
{
    assert( anim < SW_ANM_ANIMS_MAX );
    assert( ga_anm[anim].flags & ANM_INUSE );

    ga_key[ga_anm[anim].keyf_l].flags |= ANM_KF_CLP_X;
    ga_key[ga_anm[anim].keyf_l].clp_x = clip_x;    
}

void swAnmClipY( SWanim anim, float clip_y )
{
    assert( anim < SW_ANM_ANIMS_MAX );
    assert( ga_anm[anim].flags & ANM_INUSE );

    ga_key[ga_anm[anim].keyf_l].flags |= ANM_KF_CLP_Y;
    ga_key[ga_anm[anim].keyf_l].clp_y = clip_y;    
}



/*
** swAnmFade
*/
void swAnmFade( SWanim anim, float fade )
{
    assert( anim < SW_ANM_ANIMS_MAX );
    assert( ga_anm[anim].flags & ANM_INUSE );

    ga_key[ga_anm[anim].keyf_l].flags &= ~(ANM_KF_FADEA);    
    ga_key[ga_anm[anim].keyf_l].flags |= (ANM_KF_FADE);
    ga_key[ga_anm[anim].keyf_l].alpha = fade;
}

void swAnmFadeTo( SWanim anim, float fade )
{
    assert( anim < SW_ANM_ANIMS_MAX );
    assert( ga_anm[anim].flags & ANM_INUSE );

    ga_key[ga_anm[anim].keyf_l].flags &= ~(ANM_KF_FADE);
    ga_key[ga_anm[anim].keyf_l].flags |= (ANM_KF_FADEA);
    ga_key[ga_anm[anim].keyf_l].alpha = fade;
}

void swAnmFadeIn( SWanim anim )
{
    swAnmFadeTo( anim, 255 );
}

void swAnmFadeOut( SWanim anim )
{
    swAnmFadeTo( anim, 0 );
}


/*
** swAnmStart
*/
SWrtanim swAnmStart( SWanim anim, SWsprite sprite )
{
    SWrtanim rt_anim = 0;
    SWrtanim rta;
    
    assert( anim < SW_ANM_ANIMS_MAX );
    assert( ga_anm[anim].flags & ANM_INUSE );
        
    if( ga_anm[anim].keyf_f != 0 )
    {
        /*
        ** Locate the first free runtime animation structure from
        ** the runtime animation structure array.
        ** Simple linear search is fine for this, and order is
        ** not important.
        */
        for( rta=1; rta<SW_ANM_RTANM_MAX && rt_anim == 0; rta++ ) 
        {
            if( ga_rta[rta].inuse == 0 )
                rt_anim = rta;
        }
        if( rt_anim != 0 ) 
        {
            ga_rta[rt_anim].inuse = 1;
            ga_rta[rt_anim].paused = 0;
            ga_rta[rt_anim].anim = anim;
            ga_rta[rt_anim].sprite = sprite;
            ga_rta[rt_anim].frame = -1;
            ga_rta[rt_anim].loopc = ga_anm[anim].loopc;
            
            if( ga_anm[anim].flags & ANM_REVERSE ) 
            {
                ga_rta[rt_anim].reverse = 1;
                rtaSetKeyframe( rt_anim, ga_anm[anim].keyf_l );
            }
            else
            {
                ga_rta[rt_anim].reverse = 0;
                rtaSetKeyframe( rt_anim, ga_anm[anim].keyf_f );
            }
        }
    }
    return rt_anim;
}

/*
** swAnmStop
*/
void swAnmStopAll( void )
{
    SWrtanim rta;

    for( rta=1; rta<SW_ANM_RTANM_MAX; rta++ )
    {
        if( ga_rta[rta].inuse == 1 )
            rtaHalt(rta);
    }
}

void swAnmStop( SWrtanim rtanim )
{
	assert( rtanim < SW_ANM_RTANM_MAX );
    rtaHalt(rtanim);
}

void swAnmStopv( int count, SWrtanim * rtas )
{
    for( int rta=0; rta<count; rta++ )
    {
        swAnmStop( rtas[rta] );
        rtas[rta] = 0;
    }
}

/*
** swAnmPause/Resume
*/
void swAnmPauseAll( void )
{
    SWrtanim rta;
    
    for( rta=1; rta<SW_ANM_RTANM_MAX; rta++ )
    {
        if( ga_rta[rta].inuse == 1 )
            ga_rta[rta].paused = 1;
    }
}

void swAnmPause( SWrtanim rtanim )
{
	assert( rtanim < SW_ANM_RTANM_MAX );
    ga_rta[rtanim].paused = 1;
}

void swAnmPausev( int count, SWrtanim * rtas )
{
    for( int rta=0; rta<count; rta++ )
    {
        if( ga_rta[rtas[rta]].inuse == 1 )
            ga_rta[rtas[rta]].paused = 1;
    }
}

void swAnmResumeAll( void )
{
    SWrtanim rta;
    
    for( rta=1; rta<SW_ANM_RTANM_MAX; rta++ )
    {
        if( ga_rta[rta].inuse == 1 )
            ga_rta[rta].paused = 0;
    }
}

void swAnmResume( SWrtanim rtanim )
{
	assert( rtanim < SW_ANM_RTANM_MAX );
    ga_rta[rtanim].paused = 0;
}

void swAnmResumev( int count, SWrtanim * rtas )
{
    for( int rta=0; rta<count; rta++ )
    {
        if( ga_rta[rtas[rta]].inuse == 1 )
            ga_rta[rtas[rta]].paused = 0;
    }
}

/*
** Getting current frame number
*/
int swAnmGetFrame( SWrtanim rtanim )
{
    assert( rtanim < SW_ANM_RTANM_MAX );
    return ga_rta[rtanim].frame;
}


/*
** Processing function.
*/
void swAnmTick( void )
{
    int rta;    
    for( rta=0; rta<SW_ANM_RTANM_MAX; rta++ ) 
    {
        if( ga_rta[rta].inuse && ga_rta[rta].paused == 0 )
        {
            rtaTick( rta );
        }
    }
}

