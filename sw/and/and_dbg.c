#include <android/log.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "sw_dbg.h"

#define SW_DBG_ERRBUF_SIZE 256
#define SW_DBG_TAG "SW_DBG"

void swHalt( void )
{
}


void swAssert( 
    const char * expr, 
    const char * file, 
    unsigned int line 
    )
{    
    __android_log_print( ANDROID_LOG_FATAL, SW_DBG_TAG, "\n\n%s(%d) : Assertion Failed : '%s'\n", file, line, expr );
    swHalt();
}


void swLog(
    const char * fmt, ...
    )
{
    char msg[SW_DBG_ERRBUF_SIZE] = {0};
    va_list args = { 0 };

    if( NULL == fmt )
        return;

    va_start( args, fmt );
    vsprintf( msg, fmt, args);
    va_end( args );
    
    __android_log_write( ANDROID_LOG_DEBUG, SW_DBG_TAG, msg );
}


void swMsg(
    const char prefix, 
    const char * file, unsigned int line, 
    const char * fmt,
    ...
    )
{
	char msg[SW_DBG_ERRBUF_SIZE] = { 0 };
    va_list args = { 0 };

    va_start( args, fmt );
    vsprintf( msg, fmt, args );
    va_end( args );
    
    __android_log_print( ANDROID_LOG_DEBUG, SW_DBG_TAG, "[%c:] %14s(%05d) : %s", prefix, file, line, msg );
}

