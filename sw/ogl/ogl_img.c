/*
** PREREQUISITES
*/
#include <stdlib.h>
#include <assert.h>

#include "sw_cfg.h"
#include "sw_def.h"
#include "sw_dbg.h"
#include "sw_img.h"

#include "ogl_img.h"
#include "ogl_quad.h"
#include "ogl_gfx.h"
#include "ogl_tex.h"


/*
** EXPORTED VARIABLES
*/
SW_OGL_IMG_S sw_ogl_img_as[ SW_IMG_MAX ] = { 0 };


/*
** EXPORTED FUNCTIONS
*/

int  swImgInit( void )
{
    /*
    ** No initialization is required, as zeroing out the
    ** image array is sufficient.
    */
    SW_INFOF( "swImgInit() - Maximum images : %d", SW_IMG_MAX );
    return 1;
}
void swImgFree( void )
{
}


int swImgGen( int image_count, SWimage * images )
{
    /*
    ** Generate image_count new images and place the handles
    ** inside the passed array. Assumes array is large enough
    ** to hold all the handles.
    */
    int img, i;

    assert( image_count >= 0 );
    assert( images != NULL );

    for( img=0; img<image_count; img++ )
    {
        images[img] = 0;

        for( i=1; i<SW_IMG_MAX; i++ )
        {
            /*
            ** Image order is irrelevant, so use the first image
            ** marked as unused.
            */
            if( sw_ogl_img_as[i].used == 0 )
                break;
        }
        if( i >= SW_IMG_MAX )
        {
            /*
            ** If index runs to the end, all the images are used up.
            */
            SW_ERR("swImgGen() - All images exhausted");
            return img;
        }

        sw_ogl_img_as[i].used     = 1;
        sw_ogl_img_as[i].texture  = 0;
        sw_ogl_img_as[i].ofs[0]   = 0;
        sw_ogl_img_as[i].ofs[1]   = 0;
        sw_ogl_img_as[i].osz[0]   = 0;
        sw_ogl_img_as[i].osz[1]   = 0;
        sw_ogl_img_as[i].tex00[0] = 0;
        sw_ogl_img_as[i].tex00[1] = 0;
        sw_ogl_img_as[i].tex01[0] = 0;
        sw_ogl_img_as[i].tex01[1] = 0;
        sw_ogl_img_as[i].tex10[0] = 0;
        sw_ogl_img_as[i].tex10[1] = 0;
        sw_ogl_img_as[i].tex11[0] = 0;
        sw_ogl_img_as[i].tex11[1] = 0;

        images[img] = i;
    }
    return img;
}


void swImgDel( int num_images, SWimage * images )
{
    /*
    ** Delete all images in passed array and return them to
    ** the system. Okay to have null image (0) in array.
    */
    int img;

    assert( num_images >= 0 );
    assert( images != NULL );

    for( img=0; img<num_images; img++ )
    {
        if( images[img] > 0 && images[img] < SW_IMG_MAX )
        {
            sw_ogl_img_as[images[img]].used = 0;
        }
    }
}


void swImgMap( SWimage image, SWtexture texture )
{
    /*
    ** Map a single image as the entire contents
    ** of a texture.
    */
    assert( texture != 0 );
    assert( image   != 0 );
    assert( image   <  SW_IMG_MAX );
    
    int tw = swTexGetWidth ( texture );
    int th = swTexGetHeight( texture );
        
    sw_ogl_img_as[image].texture = texture;
    
    sw_ogl_img_as[image].tex00[0] = 0;
    sw_ogl_img_as[image].tex00[1] = 0;
    sw_ogl_img_as[image].tex10[0] = tw;
    sw_ogl_img_as[image].tex10[1] = 0;
    sw_ogl_img_as[image].tex11[0] = tw;
    sw_ogl_img_as[image].tex11[1] = th;
    sw_ogl_img_as[image].tex01[0] = 0;
    sw_ogl_img_as[image].tex01[1] = th;
    
    sw_ogl_img_as[image].osz[0]   = tw;
    sw_ogl_img_as[image].osz[1]   = th;
    sw_ogl_img_as[image].ofs[0]   = 0;
    sw_ogl_img_as[image].ofs[1]   = 0;
}


void
swImgMapGrid(
    int num_images, SWimage * images, SWtexture texture,
    int x, int y,
    int width, int height,
    int rows, int cols,
    int row_pad, int col_pad
  	)
{
    /*
    ** Manual image mapping function.
    ** Assumes images are arranged in an evenly spaced grid
    ** on the texture, and all of the images are the same size.
    **
    ** Assumes the size of the image array passed in is at least
    ** as large as the total number of images in the grid, given
    ** by the number of rows and columns.
    */

    int total_images;
    int rw, cl;
    int img;

    assert( num_images > 0 );
    assert( images != NULL );
    assert( texture != 0 );

    total_images = rows * cols;
    assert( total_images <= num_images );

    if( total_images > num_images )
        total_images = num_images;

    rw = 0;
    cl = 0;

    for( img=0; img<total_images; img++ )
    {
        int id = images[img];
        
        assert( id >=0 && id < SW_IMG_MAX );

        if( cl >= cols ) 
        {
            cl = 0;
            rw++;
        }

        sw_ogl_img_as[id].texture  = texture;
            
        sw_ogl_img_as[id].tex00[0] = (x + (cl*width)  + (cl*col_pad));
        sw_ogl_img_as[id].tex00[1] = (y + (rw*height) + (rw*row_pad));

        sw_ogl_img_as[id].tex10[0] = sw_ogl_img_as[id].tex00[0] + width;
        sw_ogl_img_as[id].tex10[1] = sw_ogl_img_as[id].tex00[1];
        sw_ogl_img_as[id].tex11[0] = sw_ogl_img_as[id].tex10[0];
        sw_ogl_img_as[id].tex11[1] = sw_ogl_img_as[id].tex10[1] + height;
        sw_ogl_img_as[id].tex01[0] = sw_ogl_img_as[id].tex00[0];
        sw_ogl_img_as[id].tex01[1] = sw_ogl_img_as[id].tex11[1];

        sw_ogl_img_as[id].osz[0]   = width;
        sw_ogl_img_as[id].osz[1]   = height;
        sw_ogl_img_as[id].ofs[0]   = 0;
        sw_ogl_img_as[id].ofs[1]   = 0;

        cl++;
    }
}


void
swImgMapFrames( 
    int image_count, SWimage * images, SWtexture texture,
    int frame_count, SW_IMG_FRAME_S * frames
    )
{
    /*
    ** Image mapping function that uses an array of
    ** predefined image frame structures. Assumes the passed in
    ** frames array is large enough to hold the requested number
    ** of images.
    */

    int img;
    
    assert( image_count > 0 );
    assert( frame_count > 0 );
    assert( texture != 0 );    
    assert( images  != NULL );
    assert( frames  != NULL );
        
    for( img=0; img<image_count; img++ )
    {
        int id = images[img];
        assert( id > 0 && id < SW_IMG_MAX );
        
        sw_ogl_img_as[id].texture  = texture;
        
        sw_ogl_img_as[id].tex00[0] = frames[img].x;
        sw_ogl_img_as[id].tex00[1] = frames[img].y;
        
        sw_ogl_img_as[id].tex10[0] = sw_ogl_img_as[id].tex00[0] + frames[img].w;
        sw_ogl_img_as[id].tex10[1] = sw_ogl_img_as[id].tex00[1];
        
        sw_ogl_img_as[id].tex11[0] = sw_ogl_img_as[id].tex10[0];
        sw_ogl_img_as[id].tex11[1] = sw_ogl_img_as[id].tex10[1] + frames[img].h;
        
        sw_ogl_img_as[id].tex01[0] = sw_ogl_img_as[id].tex00[0];
        sw_ogl_img_as[id].tex01[1] = sw_ogl_img_as[id].tex11[1];
        
        sw_ogl_img_as[id].osz[0]   = frames[img].ow;
        sw_ogl_img_as[id].osz[1]   = frames[img].oh;
        sw_ogl_img_as[id].ofs[0]   = frames[img].ox;
        sw_ogl_img_as[id].ofs[1]   = frames[img].oy;
    }
}

void
swImgMapFrames2(
               int image_count, SWimage * images, SWtexture * textures,
               int frame_count, SW_IMG_FRAME_S * frames
               )
{
    /*
     ** Image mapping function that uses an array of
     ** predefined image frame structures. Assumes the passed in
     ** frames array is large enough to hold the requested number
     ** of images.
     */
    
    int img;
    
    assert( image_count > 0 );
    assert( frame_count > 0 );
    assert( textures != NULL );
    assert( images   != NULL );
    assert( frames   != NULL );
    
    for( img=0; img<image_count; img++ )
    {
        int id = images[img];
        assert( id > 0 && id < SW_IMG_MAX );
        
        sw_ogl_img_as[id].texture  = textures[frames[img].texture];
        
        sw_ogl_img_as[id].tex00[0] = frames[img].x;
        sw_ogl_img_as[id].tex00[1] = frames[img].y;
        
        sw_ogl_img_as[id].tex10[0] = sw_ogl_img_as[id].tex00[0] + frames[img].w;
        sw_ogl_img_as[id].tex10[1] = sw_ogl_img_as[id].tex00[1];
        
        sw_ogl_img_as[id].tex11[0] = sw_ogl_img_as[id].tex10[0];
        sw_ogl_img_as[id].tex11[1] = sw_ogl_img_as[id].tex10[1] + frames[img].h;
        
        sw_ogl_img_as[id].tex01[0] = sw_ogl_img_as[id].tex00[0];
        sw_ogl_img_as[id].tex01[1] = sw_ogl_img_as[id].tex11[1];
        
        sw_ogl_img_as[id].osz[0]   = frames[img].ow;
        sw_ogl_img_as[id].osz[1]   = frames[img].oh;
        sw_ogl_img_as[id].ofs[0]   = frames[img].ox;
        sw_ogl_img_as[id].ofs[1]   = frames[img].oy;
    }
}


void
swImgMapPixels(
    SWimage image, SWtexture texture,
    int x, int y,
    int w, int h,
    void * pbits
    )
{
    sw_ogl_img_as[image].texture = texture;
    
    sw_ogl_img_as[image].ofs[0] = 0;
    sw_ogl_img_as[image].ofs[1] = 0;
    sw_ogl_img_as[image].osz[0] = w;
    sw_ogl_img_as[image].osz[1] = h;
    
    sw_ogl_img_as[image].tex00[0] = x;
    sw_ogl_img_as[image].tex00[1] = y;
    
    sw_ogl_img_as[image].tex10[0] = sw_ogl_img_as[image].tex00[0] + w;
    sw_ogl_img_as[image].tex10[1] = sw_ogl_img_as[image].tex00[1];
    
    sw_ogl_img_as[image].tex11[0] = sw_ogl_img_as[image].tex10[0];
    sw_ogl_img_as[image].tex11[1] = sw_ogl_img_as[image].tex10[1] + h;
    
    sw_ogl_img_as[image].tex01[0] = sw_ogl_img_as[image].tex00[0];
    sw_ogl_img_as[image].tex01[1] = sw_ogl_img_as[image].tex11[1];
    
    glBindTexture( GL_TEXTURE_2D, sw_ogl_tex_as[texture].name );
    glTexSubImage2D(
                    GL_TEXTURE_2D, 0, x, y, w, h,
                    GL_RGBA,
                    GL_UNSIGNED_BYTE, pbits );
}


int swImgGetWidth ( SWimage image )
{
    assert( image < SW_IMG_MAX );
    return( sw_ogl_img_as[image].tex10[0] - sw_ogl_img_as[image].tex00[0] );
}
int swImgGetHeight( SWimage image )
{
    assert( image < SW_IMG_MAX );
    return( sw_ogl_img_as[image].tex01[1] - sw_ogl_img_as[image].tex00[1] );
}

int swImgGetUncroppedWidth ( SWimage image )
{
    assert( image < SW_IMG_MAX );
    return( sw_ogl_img_as[image].osz[0] );
}
int swImgGetUncroppedHeight( SWimage image )
{
    assert( image < SW_IMG_MAX );
    return( sw_ogl_img_as[image].osz[1] );
}

int swImgGetOffsetX( SWimage image )
{
    assert( image < SW_IMG_MAX );
    return( sw_ogl_img_as[image].ofs[0] );
}
int swImgGetOffsetY( SWimage image )
{
    assert( image < SW_IMG_MAX );
    return( sw_ogl_img_as[image].ofs[1] );
}

SWtexture swImgGetTexture( SWimage image )
{
    assert( image < SW_IMG_MAX );
    return( sw_ogl_img_as[image].texture );    
}


