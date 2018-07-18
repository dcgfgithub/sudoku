#ifndef __SW_DBG_H__
#define __SW_DBG_H__
/*
** Module
*/

#include "sw_cfg.h"

/*
** Defines the basic debugging functions.
** Access to these functions is always available, regardless of the 
** build settings, but how each one behaves can vary between debug and
** release builds. 
**
** In general, it is recommened that you use the macros defined below
** for error checking.
*/
#if defined(__cplusplus)
  extern "C" {
#endif

extern void swHalt( void );
extern void swAssert(
    const char * expr, 
    const char * file, unsigned int line
    );
extern void swLog(
    const char * fmt, ...
    );
extern void swMsg(
    const char   prefix, 
    const char * file, unsigned int line, 
    const char * fmt, ...
    );

#if defined(__cplusplus)
  }
#endif


/*
** Common debugging macros.
*/
#if defined(DEBUG)

#if SW_DBG_LEVEL > 1

    #define SW_ASSERT(exp)\
        if( !(exp) ) {\
          swAssert( #exp, (const char *)__FILE__, (unsigned int)__LINE__ );\
        }
    #define SW_ASSERTFALSE\
        swAssert( "AssertFalse", (const char *)__FILE__, (unsigned int)__LINE__ );

    #define SW_LOG(fmt)\
        swLog( (const char *)(fmt) )
        
    #define SW_LOGF(fmt, ...)\
        swLog( (const char *)(fmt), __VA_ARGS__ )

    #define SW_MSG(pfx, fmt)\
        swMsg( (const char)(pfx), (const char *)__FILE__, (unsigned int)__LINE__, (const char *)(fmt) );
        
    #define SW_MSGF(pfx, fmt, ...)\
        swMsg( (const char)(pfx), (const char *)__FILE__, (unsigned int)__LINE__, (const char *)(fmt), __VA_ARGS__ );
        
    #define SW_ERR(fmt)\
        swMsg( 'E', (const char *)__FILE__, (unsigned int)__LINE__, (const char *)(fmt) );
        
    #define SW_ERRF(fmt, ...)\
        swMsg( 'E', (const char *)__FILE__, (unsigned int)__LINE__, (const char *)(fmt), __VA_ARGS__ );
        
    #define SW_WARN(fmt)\
        swMsg( 'W', (const char *)__FILE__, (unsigned int)__LINE__, (const char *)(fmt) );
                
    #define SW_WARNF(fmt, ...)\
        swMsg( 'W', (const char *)__FILE__, (unsigned int)__LINE__, (const char *)(fmt), __VA_ARGS__ );
        
    #define SW_INFO(fmt, ...)\
        swMsg( 'I', (const char *)__FILE__, (unsigned int)__LINE__, (const char *)(fmt) );
                
    #define SW_INFOF(fmt, ...)\
        swMsg( 'I', (const char *)__FILE__, (unsigned int)__LINE__, (const char *)(fmt), __VA_ARGS__ );
        
#elif SW_DBG_LEVEL == 1

    #define SW_ASSERT(exp)\
        if( !(exp) ) {\
        swAssert( #exp, (const char *)__FILE__, (unsigned int)__LINE__ );\
        }
    
    #define SW_ASSERTFALSE\
        swAssert( "AssertFalse", (const char *)__FILE__, (unsigned int)__LINE__ );

    #define SW_LOG(fmt)\
        swLog( (const char *)(fmt) )
        
    #define SW_LOGF(fmt, ...)\
        swLog( (const char *)(fmt), __VA_ARGS__ )

    #define SW_MSG(pfx, fmt)\
        swMsg( (const char)(pfx), (const char *)__FILE__, (unsigned int)__LINE__, (const char *)(fmt) );
        
    #define SW_MSGF(pfx, fmt, ...)\
        swMsg( (const char)(pfx), (const char *)__FILE__, (unsigned int)__LINE__, (const char *)(fmt), __VA_ARGS__ );
        
    #define SW_ERR(fmt)\
        swMsg( 'E', (const char *)__FILE__, (unsigned int)__LINE__, (const char *)(fmt) );
        
    #define SW_ERRF(fmt, ...)\
        swMsg( 'E', (const char *)__FILE__, (unsigned int)__LINE__, (const char *)(fmt), __VA_ARGS__ );

    #define SW_WARN(fmt)\
        swMsg( 'W', (const char *)__FILE__, (unsigned int)__LINE__, (const char *)(fmt) );
                
    #define SW_WARNF(fmt, ...)\
        swMsg( 'W', (const char *)__FILE__, (unsigned int)__LINE__, (const char *)(fmt), __VA_ARGS__ );

    #define SW_INFO(fmt, ...)        
    #define SW_INFOF(fmt, ...)        

#else

    #define GT_ASSERT(exp)
    #define GT_ASSERTFALSE    
    
    #define SW_LOG(fmt)
    #define SW_LOGF(fmt, ...)
    #define SW_MSG(pfx, fmt)
    #define SW_MSGF(pfx, fmt, ...)
    #define SW_ERR(fmt)
    #define SW_ERRF(fmt, ...)
    #define SW_WARN(fmt)
    #define SW_WARNF(fmt, ...)
    #define SW_INFO(fmt)
    #define SW_INFOF(fmt, ...)

#endif
        
#else

    #define GT_ASSERT(exp)
    #define GT_ASSERTFALSE    
    
    #define SW_LOG(fmt)
    #define SW_LOGF(fmt, ...)
    #define SW_MSG(pfx, fmt)
    #define SW_MSGF(pfx, fmt, ...)
    #define SW_ERR(fmt)
    #define SW_ERRF(fmt, ...)
    #define SW_WARN(fmt)
    #define SW_WARNF(fmt, ...)
    #define SW_INFO(fmt)
    #define SW_INFOF(fmt, ...)
         
#endif



#endif /* __SW_DBG_H__ */
