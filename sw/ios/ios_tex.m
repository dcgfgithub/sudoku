/*
** PROLOGUE:
**
** Implements the texture functionality.
** Textures in this context act as a wrapper around an OpenGL
** texture, and add a few platform dependant routines, such
** as loading a texture from a file.
*/

/*
** PREREQUISITES
*/
#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>
#import <UIKit/UIKit.h>

#include "sw_dbg.h"

#include "ogl.h"
#include "ogl_tex.h"


/*
** EXPORTED FUNCTIONS
*/

/*
** Loads the image from the given filename, and creates a new texture.
** The file is assumed to be located inside the App resources bundle, and the
** name should not include any additional path information.
**
** Loading uses the same method suggested by the various sample apps, where
** the image is loaded using UIImage interface, and then uses a CGBitmapContext
** to extract the data.
*/
static void loadTexture( SW_OGL_TEX_S * ptexture, CGImageRef tex_image )
{
	CGContextRef	tex_context;
	GLubyte *		tex_data;
	size_t			tex_width, tex_height;

	tex_width  = CGImageGetWidth(tex_image);
	tex_height = CGImageGetHeight(tex_image);
	
	if( tex_image )
    {
		tex_data = (GLubyte *) calloc(tex_width * tex_height * 4, sizeof(GLubyte));
		tex_context = CGBitmapContextCreate(
                        tex_data,
                        tex_width, tex_height, 8,
                        tex_width * 4,
                        CGImageGetColorSpace(tex_image),
                        (CGBitmapInfo)kCGImageAlphaPremultipliedLast
                        );
		CGContextDrawImage(
                        tex_context,
                        CGRectMake(0.0, 0.0, (CGFloat)tex_width, (CGFloat)tex_height),
                        tex_image
                        );
		CGContextRelease( tex_context);
        
        glBindTexture( GL_TEXTURE_2D, ptexture->name );
        
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)tex_width, (GLsizei)tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data );
		free( tex_data );
        
		ptexture->width  = (int)tex_width;
		ptexture->height = (int)tex_height;
	}
}


void swoglTexLoad( SW_OGL_TEX_S * ptexture, const char * filename )
{
	NSString * filename_ns = [NSString stringWithCString:filename encoding:NSASCIIStringEncoding];
	CGImageRef tex_image;
    	
	tex_image = [UIImage imageNamed: filename_ns].CGImage;
    if( tex_image == NULL )
    {
        SW_ERRF( "swoglTexLoad() - File %s could not be loaded.\n", filename );
    }
    loadTexture( ptexture, tex_image );
}


void swoglTexLoadUser( SW_OGL_TEX_S * ptexture, const char * filename )
{
	NSString * filename_ns = [NSString stringWithCString:filename encoding:NSASCIIStringEncoding];

    NSArray  * paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory,NSUserDomainMask, YES);
    NSString * path  = [paths objectAtIndex:0];
    NSString * name  = [path stringByAppendingPathComponent:filename_ns];
    
    NSData   * data  = [NSData dataWithContentsOfFile:name];
    CGImageRef image = [UIImage imageWithData:data].CGImage;
    
    if( image == NULL )
    {
        SW_ERRF( "swoglTexLoadUser() - File %s could not be loaded.\n", filename );
    }
    loadTexture( ptexture, image );
}

void swoglTexLoadCompressed( SW_OGL_TEX_S * ptexture, const char * filename )
{
}

