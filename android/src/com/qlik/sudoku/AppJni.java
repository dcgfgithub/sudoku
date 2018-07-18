/*
*/

package com.qlik.sudoku;

import android.provider.MediaStore;
import android.util.Log;

import android.net.Uri;

import android.graphics.Bitmap;

import android.content.Context;
import android.content.Intent;

import javax.microedition.khronos.opengles.GL10;

public class AppJni 
{		
	private static final String TAG = "AppJni";
		
	private static AppActivity  m_Activity = null;
	private static AppView      m_View;
	private static Context    	m_Context;
	private static GL10       	m_GL10;
        
    private static CpwaTexture  m_Texture;
        	
	public static native void nativeInit( int width, int height);
	public static native void nativeStart();
	public static native void nativeStop();
	public static native void nativeRestart();
	public static native void nativePause();
	public static native void nativeResume();
	public static native void nativeRender();
	public static native void nativeReload();
	
	public static native void nativeTouch    ( float X, float Y );
	public static native void nativeTouchUp  ( float X, float Y );
	public static native void nativeTouchMove( float X, float Y );
	
	public static native void nativeKeyBack();
			
	static 
	{
		System.loadLibrary( "appjni" );
	}
	
	public static void init( AppActivity activity, AppView view, Context context, GL10 gl, int width, int height )
	{
		if( m_Activity == null )
		{
			m_Activity = activity;
			m_View     = view;
			m_Context  = context;
			m_GL10     = gl;
			
			m_Texture = new AppTexture( context, gl );
						
			nativeInit( width, height );
			nativeStart();
			nativeResume();
		}
		else
		{
			nativeInit( width, height );
			nativeRestart();
			onResume();			
			nativeResume();
		}
	}
	
	public static void reload()
	{
		nativeReload();
	}
	
	public static void render( )
	{
		nativeRender();
	}
	
	public static void onPause()
	{
		nativePause();
	}
	
	public static void onResume()
	{					
		nativeResume();
	}
		
	public static void onStop()
	{
		nativeStop();
	}

	
	/*
	** Texture
	*/
	public static void jniTextureLoad( int tn, String name )
	{
		m_Texture.load( tn, name );
	}
	public static int jniTextureWidth( )
    {
        return m_Texture.width();
    }
    public static int jniTextureHeight( )
    {
        return m_Texture.height();
    }
    	
	/*
	** Return the absolute base path name to the applications local storage folder.
	*/
	public static String jniFileBasePath()
	{
		return m_Context.getFilesDir().getAbsolutePath();
	}
	
	/*
	** Force the activity to minimize the current active activity.
	*/
	public static void jniBack()
	{
    	m_Activity.moveTaskToBack( true );		
	}
}
