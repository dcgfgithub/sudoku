#ifndef __SW_CFG_H__
#define __SW_CFG_H__
/*
** PROLOGUE:
**
** Defines the sizes for the various Silkweed structures
** used by the application.
**
** The application should make a copy of this file, and edit
** its values accordingly. Comments accompany each setting
** that describe how it is used.
*/


/*
** DEFINITIONS
*/
#define SW_DBG_LEVEL 2


#define SW_TEX_MAX 12
    /* The maximum allowable textures.
    ** Each texture definition consumes little memory, with
    ** most of the size taken up by the actual texture
    ** data.
    */

    
#define SW_IMG_MAX 2000
    /* The maximum number of images that can be defined.
    ** Varies depending on the number of allowable textures
    ** and the number of images on each texture atlas.
    **
    */


#define SW_QUAD_MAX 2000
    /* The maximum number of quads that can be displayed
    ** each update cycle. Higher values consume slightly more
    ** memory, and will take longer to render.
    **
    ** Each quad relates to one display element.
    ** A display element can be a single sprite, or a single
    ** particle from a particle generator.
    */
    
    
#define SW_SPR_MAX 1200
    /* The maximum allowable sprites that can be generated.
    ** This value can be larger than the allowable number
    ** of quads if you don't plan on displaying every sprite
    ** on each frame.
    */

    
#define SW_ANM_ANIMS_MAX 600
#define SW_ANM_RTANM_MAX 400
#define SW_ANM_KEYF_MAX  1000
    /* Defines the maximum definitions for the different
    ** elements used for animations.
    **
    ** SW_ANM_ANIMS_MAX is the total number of anims that can
    ** be generated at a time. Anims are generated using 
    ** swAnmGen funtion.
    **
    ** SW_ANM_RTANM_MAX is the maximum allowable runtime
    ** anims. Runtime anims are created for every anim
    ** that is started.
    **
    ** SW_ANM_KEYF_MAX is the total number of keyframes that
    ** can be assigned to all anims. All anims must share
    ** the same pool of keyframes. A new keyframe is created
    ** each time swAnmKeyframe is called.
    */


#define SW_SHA_MAX 4
    /* Defines the maximum allowable number of shaders that
    ** can be defined. A new shader is created each time
    ** swShaGen is called.
    */
    
    
#define SW_PAN_MAX 1


#define SW_MSG_MAX_QUEUE_NODES    128
#define SW_MSG_MAX_CALLBACK_NODES 128


#define SW_PAR_MAX_IMAGES       4
#define SW_PAR_MAX_INTERVALS    4
#define SW_PAR_MAX_PARTICLES    400
#define SW_PAR_MAX_EMITTERS     12


#endif /* __SW_CFG_H__ */



