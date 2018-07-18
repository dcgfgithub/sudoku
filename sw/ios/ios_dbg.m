#import <Foundation/Foundation.h>

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "sw_dbg.h"

#define SW_DBG_ERRBUF_SIZE 256


void swHalt( void )
{
}


void swAssert( 
    const char * expr, 
    const char * file, 
    unsigned int line 
    )
{    
    fprintf( stderr, "\n\n%s(%d) : Assertion Failed : '%s'\n", 
             file, line, expr );
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

    fprintf( stderr, "%s", msg );
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
    
    NSString * path = [NSString stringWithCString:file encoding:NSASCIIStringEncoding];
    NSArray  * pathComponents = [path pathComponents];
    NSString * name = [pathComponents lastObject];
    
    const char * filename = [name cStringUsingEncoding:NSASCIIStringEncoding];
    
    fprintf( stderr, "[%c:] %14s(%05d) : %s\n", 
             prefix, filename, line, msg );
}

