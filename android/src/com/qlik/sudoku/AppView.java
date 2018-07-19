/*
*/

package com.qlik.sudoku;

import android.content.Context;
import android.util.Log;
import android.view.MotionEvent;
import android.graphics.PixelFormat;
import android.opengl.GLSurfaceView;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

class AppView extends GLSurfaceView
{
    private Context 	m_context;
    private AppActivity	m_activity;
    private AppRenderer m_renderer;
    private AppView     m_view;

    public AppView(AppActivity activity, Context context)
    {
        super(context);
        m_context  = context;
        m_activity = activity;
        m_view     = this;
        init();
    }

    private void init() 
    {
        this.getHolder().setFormat(PixelFormat.TRANSLUCENT);
        setDebugFlags( DEBUG_CHECK_GL_ERROR | DEBUG_LOG_GL_CALLS );
        setEGLContextClientVersion(1);
        setEGLConfigChooser( 8, 8, 8, 8, 0, 0 );
        setPreserveEGLContextOnPause( true );
        m_renderer = new AppRenderer();
        m_renderer.init();
        setRenderer(m_renderer);
    }
    
    @Override
    public void onPause()
    {
    	super.onPause();
		this.queueEvent(new Runnable() {
			@Override
			public void run() {
				m_renderer.onPause();
			}
		});    	
    }
    
    @Override
    public void onResume()
    {
    	super.onResume();
    	this.queueEvent(new Runnable() {
			@Override
			public void run() {
				m_renderer.onResume();
			}
		});      	
    }
            
    public void onStop()
    {
    	this.queueEvent(new Runnable() {
			@Override
			public void run() {
				m_renderer.onStop();
			}
		});  
    }
    
	@Override
	public boolean onTouchEvent(final MotionEvent pMotionEvent) 
	{
		switch (pMotionEvent.getAction() & MotionEvent.ACTION_MASK) 
		{
			case MotionEvent.ACTION_DOWN:
				
				final float xd = pMotionEvent.getX(0);
				final float yd = pMotionEvent.getY(0);
				
				this.queueEvent(new Runnable() {
					@Override
					public void run() {
						m_renderer.onTouch(xd, yd);
					}
				});
				break;

			case MotionEvent.ACTION_MOVE:
				
				final float xm = pMotionEvent.getX(0);
				final float ym = pMotionEvent.getY(0);
				
				this.queueEvent(new Runnable() {
					@Override
					public void run() {
						m_renderer.onTouchMove(xm, ym);
					}
				});
				break;

			case MotionEvent.ACTION_UP:
			
				final float xu = pMotionEvent.getX(0);
				final float yu = pMotionEvent.getY(0);
				
				this.queueEvent(new Runnable() {
					@Override
					public void run() {
						m_renderer.onTouchUp(xu, yu);
					}
				});
				break;

			case MotionEvent.ACTION_CANCEL:
				
				final float xc = pMotionEvent.getX(0);
				final float yc = pMotionEvent.getY(0);
				
				this.queueEvent(new Runnable() {
					@Override
					public void run() {
						m_renderer.onTouchUp(xc, yc);
					}
				});
				break;
		}
		return true;
	}
	
	public void onKeyBack()
	{
		this.queueEvent(new Runnable() {
			@Override
			public void run() {
				m_renderer.onKeyBack();
			}
		});		
	}
	
    private class AppRenderer implements GLSurfaceView.Renderer 
    {
    	private static final String TAG = "AppRenderer";
    	
    	private int m_height;
    	private boolean m_initialized; 
    	private boolean m_paused;

        public void onDrawFrame(GL10 gl) 
        {
            AppJni.render();
        }

        public void onSurfaceChanged(GL10 gl, int width, int height) 
        {                    	
        	Log.d(TAG, "onSurfaceChanged");
        	
        	AppJni.init( m_activity, m_view, m_context, gl, width, height );
        	m_initialized = true;
        	m_height      = height;
        }

        public void onSurfaceCreated(GL10 gl, EGLConfig config) 
        {        	
        	Log.d(TAG, "onSurfaceCreated");
        	AppJni.reload();
        }
        
        public void init()
        {
        	m_initialized = false;
        	m_paused      = false;
        }
        
        public void onPause()
        {        	
        	if( m_initialized )
        	{
				AppJni.onPause();
        		m_paused = true;
        	}
        }
        
        public void onResume()
        {        	
        	if( m_paused )
        	{
				AppJni.onResume();
        		m_paused = false;
        	}
        }
                        
        public void onStop()
        {        	
        	if( m_initialized )
        	{
				AppJni.onStop();
        		m_paused = false;
        				/*
        				 * If the application is stopped, we no longer want to call the JNI resume
        				 * function directly. Rather, it will be called again during the init
        				 * phase when the application is restarted.
        				 */
        	}
        }
        
        public void onTouch( float x, float y )
        {
        	if( m_initialized )
				AppJni.nativeTouch(x, m_height-y);
        }
        
        public void onTouchUp( float x, float y )
        {
        	if( m_initialized )
				AppJni.nativeTouchUp(x, m_height-y);
        }        
        
        public void onTouchMove( float x, float y )
        {
        	if( m_initialized )
				AppJni.nativeTouchMove(x, m_height-y);
        }        
        
        public void onKeyBack()
        {
        	if( m_initialized )
				AppJni.nativeKeyBack();
        }
    }    
}
