/*
*/

package com.qlik.sudoku;

import android.util.Log;

import android.content.Context;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLUtils;

import java.io.InputStream;
import java.io.IOException;

import javax.microedition.khronos.opengles.GL10;


public class AppTexture
{
	private static final String TAG = "AppTexture";
	
	private static Context    	m_Context;
	private static GL10       	m_GL10;
    
    private static int        	m_TextureWidth;
    private static int     	  	m_TextureHeight;
    	
	public AppTexture( Context context, GL10 gl )
	{
		m_Context = context;
		m_GL10    = gl;
	}
		
	public void load( int tn, String name )
	{
		AssetManager assetManager = m_Context.getAssets();
		Bitmap 		 bitmap 	  = null;
		InputStream  istr         = null;
		
        m_TextureWidth  = 0;
        m_TextureHeight = 0;
        
		m_GL10.glEnable( GL10.GL_TEXTURE_2D );
		m_GL10.glBindTexture( GL10.GL_TEXTURE_2D, tn );
		
		m_GL10.glTexParameterf( GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MIN_FILTER, GL10.GL_NEAREST );
		m_GL10.glTexParameterf( GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MAG_FILTER, GL10.GL_LINEAR );

		m_GL10.glTexParameterf( GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_WRAP_S, GL10.GL_REPEAT );
		m_GL10.glTexParameterf( GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_WRAP_T, GL10.GL_REPEAT );

		try 
		{
	 		final BitmapFactory.Options options = new BitmapFactory.Options();
			options.inScaled = false;
			
			istr = assetManager.open( name );
			bitmap = BitmapFactory.decodeStream ( istr, null, options );
		} 
		catch (IOException e) 
		{
            Log.d( TAG, "Error! - Unable to load bitmap" );
			return;
		}		
        finally 
        {
        	if( istr != null )
        	{
        		try 
        		{
            		istr.close();        			
        		}
        		catch( IOException e )
        		{
        		}
        	}
        }
		
        m_TextureWidth  = bitmap.getWidth();
        m_TextureHeight = bitmap.getHeight();
        
		GLUtils.texImage2D( GL10.GL_TEXTURE_2D, 0, bitmap, 0 );
		
        Log.d( TAG, "Bitmap name: " + tn + " dimensions: "+ m_TextureWidth + " " + m_TextureHeight );		
		Log.d( TAG, "texImage2D " + GLUtils.getEGLErrorString(m_GL10.glGetError()) );
		   
		bitmap.recycle();
	}
	
	public int width( )
    {
        return m_TextureWidth;
    }
	
    public int height( )
    {
        return m_TextureHeight;
    }
}
