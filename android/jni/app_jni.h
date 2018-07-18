#ifndef __ANDROID_APP_JNI_H__
#define __ANDROID_APP_JNI_H__
/*
**
*/

/*
** PREREQUISITES
*/

/*
** EXPORTED FUNCTIONS
*/

void
app_jni_LoadTexture
	(
	int tn,
	const char * texName,
	int * pw,
	int * ph
	);
		
void
app_jni_FileBasePath
	(
	char * path_buffer,
	int    path_size
	);
	
void app_jni_Back( void );
	
#endif /* __ANDROID_APP_JNI_H__ */
