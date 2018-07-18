#ifndef __SW_TEX_H__
#define __SW_TEX_H__
/*
** Module Prefix : swTex
**
** PROLOGUE:
**
** Implements routines to create and manage textures.
** Textures are usually arranged as a texture atlas, or multiple
** individual images all arranged inside one big image.
** 
*/

/*
** PREREQUISITES
*/

/*
** DEFINITIONS
*/
typedef unsigned int SWtexture;


/*
** EXPORTED FUNCTIONS
*/
#if defined( __cplusplus )
        extern "C" {
#endif

int  swTexInit( void );
void swTexFree( void );

int  swTexGen( int count, SWtexture * textures );
void swTexDel( int count, SWtexture * textures );

void swTexCreate( SWtexture texture, int width, int height );
        /*
        ** Create a new texture that has the given dimensions.
        **
        ** The texture must have dimensions that are a power of 2.
        ** The width and height do not have to be the same.
        */


void swTexLoad    ( SWtexture texture, const char * filename );
void swTexLoadUser( SWtexture texture, const char * filename );
        /*
        ** Loads a texture from an external file and assigns
        ** it to the passed texture. The texture file must be an PNG file.
        **
        ** The texture must have dimensions that are a power of 2.
        ** The width and height do not have to be the same.
        */


void swTexLoadCompressed( SWtexture texture, const char * filename );
        /*
        ** Loads a texture from an external compressed file and assigns
        ** it to the passed texture.
        **
        ** The actual format of the texture depends on the 
        ** current platform.
        */


int swTexGetWidth ( SWtexture texture );
int swTexGetHeight( SWtexture texture );
        /*
        ** These functions provide basic information about each texture.
        */


#if defined( __cplusplus )
		}
#endif

#endif /* __SW_TEX_H__ */

