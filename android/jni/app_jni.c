/*
*/
#include <jni.h>
#include <android/log.h>
#include <string.h>

#include "app_jni.h"

#include "sw_app.h"
#include "sw_dbg.h"
#include "sw_msg.h"

#include "sw_app_msg.h"

/*
** DEFINITIONS
*/

#define  LOG_TAG    "app_jni"
#define  LOGD(...)  __android_log_print( ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__ )

/*
** LOCAL VARIABLES
*/

static JavaVM * 	g_pvm;
static JNIEnv *	 	g_penv;

static jclass   	g_jni_bridge;

static jmethodID    g_jmid_fileBasePath;
static jmethodID    g_jmid_back;

static jmethodID    g_jmid_loadTexture;
static jmethodID    g_jmid_textureW;
static jmethodID    g_jmid_textureH;

void JNICALL jni_init      		( JNIEnv * env, jobject obj, jint width, jint height );
void JNICALL jni_start    		( JNIEnv * env, jobject obj );
void JNICALL jni_restart    	( JNIEnv * env, jobject obj );
void JNICALL jni_stop    		( JNIEnv * env, jobject obj );
void JNICALL jni_pause    		( JNIEnv * env, jobject obj );
void JNICALL jni_resume    		( JNIEnv * env, jobject obj );
void JNICALL jni_reload         ( JNIEnv * env, jobject obj );
void JNICALL jni_render    		( JNIEnv * env, jobject obj );
void JNICALL jni_touch     		( JNIEnv * env, jobject obj, jfloat x, jfloat y );
void JNICALL jni_touch_up  		( JNIEnv * env, jobject obj, jfloat x, jfloat y );
void JNICALL jni_touch_move		( JNIEnv * env, jobject obj, jfloat x, jfloat y );
void JNICALL jni_back           ( JNIEnv * env, jobject obj );

static JNINativeMethod g_native_methods[] =
{
	{"nativeInit", 			"(II)V", 	(void *)jni_init},
	{"nativeStart",			"()V",     	(void *)jni_start},
	{"nativeRestart",		"()V",     	(void *)jni_restart},
	{"nativeStop",			"()V",     	(void *)jni_stop},
	{"nativePause",			"()V",     	(void *)jni_pause},
	{"nativeResume",		"()V",     	(void *)jni_resume},					
	{"nativeReload",		"()V",     	(void *)jni_reload},						
	{"nativeRender",		"()V",     	(void *)jni_render},
	{"nativeTouch",     	"(FF)V",    (void *)jni_touch},
	{"nativeTouchUp",   	"(FF)V",    (void *)jni_touch_up},
	{"nativeTouchMove", 	"(FF)V",    (void *)jni_touch_move},
	{"nativeKeyBack",       "()V",      (void *)jni_back}
};

static int jni_initialized = 0;

/*
** LOCAL FUNCTIONS
*/


/*
** EXPORTED FUNCTIONS
*/

jint JNI_OnLoad( JavaVM * vm, void * reserved )
{
	jint ok;

    g_pvm = vm;

    LOGD( "JNI_OnLoad\n" );

    LOGD( "- Obtaining the jni env pointer ... " );
	ok = (*g_pvm)->GetEnv( g_pvm, (void**)&g_penv, JNI_VERSION_1_4 );
    if( ok != 0 )
    {
        LOGD( "Unable to retrieve env pointer\n" );
        return -1;
    }

    /*
    ** Locate the jni bridge class that contains all of the native functions, as
    ** well as all of the callback to Java functions needed.
    */
    LOGD( "- Obtaining the class pointer to AppJni bridge class ... " );

    jclass jni_bridge = (*g_penv)->FindClass( g_penv, "com/qlik/sudoku/AppJni" );
    if( jni_bridge == NULL )
	{
		LOGD( "Unable to find Jni bridge class\n" );
		return -1;
	}
	g_jni_bridge = (jclass)((*g_penv)->NewGlobalRef( g_penv, jni_bridge ));

	/*
	** Register the native methods directly.
	** Easier than providing lookup and JNI formatted function signatures.
	*/
	LOGD( "- Registering native methods ... " );

	ok = (*g_penv)->RegisterNatives( g_penv, g_jni_bridge, g_native_methods, 12 );
	if( ok != 0 )
	{
		LOGD( "Unable to register native methods\n" );
		return -1;
	}

	/*
	** Cache all of the java callback functions.
	*/
    g_jmid_fileBasePath= (*g_penv)->GetStaticMethodID( g_penv, g_jni_bridge, "jniFileBasePath", "()Ljava/lang/String;" );
	g_jmid_back        = (*g_penv)->GetStaticMethodID( g_penv, g_jni_bridge, "jniBack",         "()V" );    
    
    g_jmid_loadTexture = (*g_penv)->GetStaticMethodID( g_penv, g_jni_bridge, "jniTextureLoad",  "(ILjava/lang/String;)V" );
    g_jmid_textureW    = (*g_penv)->GetStaticMethodID( g_penv, g_jni_bridge, "jniTextureWidth", "()I" );
    g_jmid_textureH    = (*g_penv)->GetStaticMethodID( g_penv, g_jni_bridge, "jniTextureHeight","()I" );                
        
	jni_initialized = 0;    
            
    return JNI_VERSION_1_4;
}

void 
app_jni_Back( void )
{
		(*g_penv)->CallStaticVoidMethod(
				g_penv,
				g_jni_bridge, g_jmid_back
				);
}

void 
app_jni_LoadTexture
		( 
		int tn, 
		const char * texName, 
		int * pw, 
		int * ph 
		)
{
		jstring jsz = (*g_penv)->NewStringUTF( g_penv, texName );

		(*g_penv)->CallStaticVoidMethod( 
				g_penv, 
				g_jni_bridge, 
				g_jmid_loadTexture, tn, jsz 
				);
		(*g_penv)->DeleteLocalRef( g_penv, jsz );

	    *pw = (*g_penv)->CallStaticIntMethod( g_penv, g_jni_bridge, g_jmid_textureW, jsz );
    	*ph = (*g_penv)->CallStaticIntMethod( g_penv, g_jni_bridge, g_jmid_textureH, jsz );
}
	
void
app_jni_FileBasePath
		(
		char * path_buffer,
		int    path_size
		)
{
		jstring path;
		const char * path_p;
		
		path = (*g_penv)->CallStaticObjectMethod(
				g_penv,
				g_jni_bridge, g_jmid_fileBasePath
				);
				
		path_p = (*g_penv)->GetStringUTFChars( g_penv, path, 0 );
		strncpy( path_buffer, path_p, path_size );
		(*g_penv)->ReleaseStringUTFChars( g_penv, path, path_p );
}

/*
** JNI callback routines from Java
*/

extern void swoglTexReload( void );

void JNICALL 
jni_init
		( 
		JNIEnv * env, jobject obj, 
		jint width, 
		jint height 
		)
{
		g_penv = env;
		
		if( jni_initialized == 0 )
		{
				swAppInit();		
				jni_initialized = 1;
		}
		swAppDisplaySize( width, height );
}

void JNICALL 
jni_reload
		( 
		JNIEnv * env, 
		jobject obj 
		)
{
		if( jni_initialized != 0 )
		{
				g_penv = env;
				swoglTexReload();
		}
}

void JNICALL jni_start( JNIEnv * env, jobject obj )
{
	swAppStart();
}
void JNICALL jni_restart( JNIEnv * env, jobject obj )
{
	swAppResume();
}
void JNICALL jni_stop( JNIEnv * env, jobject obj )
{
	swAppSuspend();
}
void JNICALL jni_pause( JNIEnv * env, jobject obj )
{
	swAppInactive();
}
void JNICALL jni_resume( JNIEnv * env, jobject obj )
{
	swAppActive();
}
void JNICALL jni_render( JNIEnv * env, jobject obj )
{
	g_penv = env;

	swAppProcess();
	swAppDisplay();
}
void JNICALL jni_touch( JNIEnv * env, jobject obj, jfloat x, jfloat y )
{
	g_penv = env;
	swAppTouch( x, y );
}
void JNICALL jni_touch_up( JNIEnv * env, jobject obj, jfloat x, jfloat y )
{
	g_penv = env;
	swAppTouchUp( x, y );
}
void JNICALL jni_touch_move( JNIEnv * env, jobject  obj, jfloat x, jfloat y )
{
	g_penv = env;
	swAppTouchMove( x, y );
}

void JNICALL jni_back( JNIEnv * env, jobject obj )
{
	g_penv = env;
	swAppBack();
}



