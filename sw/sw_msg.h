#ifndef __SW_MSG__
#define __SW_MSG__
/*
**
** Event Message queue API.
**
** This is the interface to the game message queue.
**
** This same queue is also used to provide the interaction between
** seperate modules of the game.
*/

/*
** PREREQUSITES
*/


/*
** DEFINITIONS
*/
enum
{
    SW_MSG_NONE = 0,
        /*
        ** Nothing has happened.  This is a NULL message.  Normally you won't
        ** receive this message, so it is only useful for internal message
        ** processing where you don't want to do anything. 
        ** Attempting to send this message will appear successful, but
        ** no message will be sent.
        */
    
    SW_MSG_START,
    SW_MSG_STOP,
    
    SW_MSG_PAUSE,
    SW_MSG_UNPAUSE,
    
    SW_MSG_ACTIVATE,
    SW_MSG_DEACTIVATE,
        /*
        ** Application is about to be deactivated.
        ** A SW_MSG_SUSPEND message will typically follow.
        **
        ** An application will deactivate if it get interrupted by
        ** another higher priority task. On mobile devices, this is
        ** likely an incoming phone call.
        */
    
    SW_MSG_SUSPEND,
    SW_MSG_RESUME,
        /*
        ** Application is about to be suspended or resumed.
        ** Will usually be sent in combination with SW_MSG_ACTIVATE or
        ** SW_MSG_DEACTIVATE.
        **
        ** An application is suspended on some mobile platforms.
        ** It means that the application has been placed in the background
        ** and is no longer the active task. However, it remains in
        ** memory.
        **
        ** Applications should save all user data when entering a suspended
        ** state, as some platforms will terminate a suspended 
        ** application with no warning.
        */
    
    SW_MSG_PROCESS,
    SW_MSG_DISPLAY,
    SW_MSG_ORIENTATION,
    
    SW_MSG_TOUCH,
    SW_MSG_TOUCH_UP,
    SW_MSG_TOUCH_MOVE,
    
    SW_MSG_BACK,
    
    SW_MSG_PAN,
    SW_MSG_PAN_UP,
    
    SW_MSG_ROTATE,
    SW_MSG_SCALE,
    
	SW_MSG_DETACH = 31,
        /*
        ** Virtual Message Id
        ** SW_MSG_CALLBACK is invoked with SW_MSG_DETACH in the msg field,
        ** and the SW_MSG_ID the callback is being detached from
        ** in the nVal[0] field.
        **
        ** This will always be the last message a callback receives
        ** for a given message ID.
        ** When a callback is attached to multiple MSG_ID's,
        ** will receive messages until all attached message ids
        ** have been detached (you get one SW_MSG_DETACH message for each).
        */
    
	/*
	** User Defined messages must be within the range
	** MSG_ID_USER_START to MSG_ID_USER_END inclusive.
	*/
	SW_MSG_ID_USER_START = 32 , /* Reserved range of user msgid's */
	SW_MSG_ID_USER_END   = 255
    
};
typedef int SW_MSG_ID;

struct _SW_MSG_S
{
	SW_MSG_ID msg; /* Identifies the type of message. */
    
    int    nVal[ 5 ];
    float  fVal[ 5 ];
    void * phidden;
        /* 
        ** Possible message parameters.
        ** The values contained in each of these variables depends 
        ** on the message type. The descriptions of
        ** each of the messages details which of these will contain
        ** valid values. 
        */
};
typedef struct _SW_MSG_S SW_MSG_S;


#if defined( __cplusplus )
	extern "C" {
#endif

typedef int SW_MSG_CALLBACK(
                SW_MSG_S sMsgData
                /* 
                ** Message data from the currently received message 
                */
                );
typedef SW_MSG_CALLBACK * SW_MSG_CALLBACK_PF;
        /*
        ** Prototype, and typedef for MSG callback functions
        **
        ** SW_MSG_CALLBACK_PF is the correct type for your callback function 
        ** pointer. Your callback function must look the same as the 
        ** SW_MSG_CALLBACK prototype above.
        **
        ** Normally return 0.
        ** Return non-zero when you want to inhibit subsequent and lower
        ** priority callbacks from being invoked.
        */


/*
** EXPORTED FUNCTIONS
*/

int  swMsgInit( void );
void swMsgFree( void );
        /*
        ** Initialize the message queue system. This routine must be called
        ** once before either sending or attaching events.
        **
        ** Cleanup and remove the message system.
        ** All unused events still on the queue are removed
        ** and their associated memory freed.
        */
        
        
SW_MSG_S swMsgNull(
        SW_MSG_ID msg
                /*
                ** Puts this value in the iMsgId field of the returned structure
                ** Use SW_MSG_NONE for unitialized structure.
                */
        );
        /*
        ** Returns a SW_MSG_S structure initialized to Null values, 
        ** and the given SW_MSG_ID.
        */


void swMsgAttach(
        SW_MSG_ID * piMsgId,
                /*
                ** SW_MSG_NONE terminated array of message Id's
                ** to attach the callback function to.
                */
        SW_MSG_CALLBACK_PF pfuncCallback,
                /*
                ** callback function whose prototype matches
                ** MSG_CALLBACK above.
                */
        int priority
                /*
                ** Priority determines the order in which
                ** messages are processed. Lower priority callbacks
                ** are called before higher priority.
                */
        );
        /*
        ** Puts the callback function on the list(s) associated with
        ** each of the SW_MSG_ID's in piMsgId.
        **
        ** swMsgAttach() can safely be called from within a callback
        ** routine. New attachments are only actually added to a given
        ** SW_MSG_ID's list once processing of that list's
        ** callbacks is complete.
        */
        
        
void swMsgDetach(
        SW_MSG_ID * piMsgId,
                /*
                ** MSG_NONE terminated array of message Id's
                ** to attach the callback function to.
                */
        SW_MSG_CALLBACK_PF pfuncCallback
                /*
                ** callback function whose prototype matches
                ** MSG_CALLBACK above.
                */
        );
        /*
        ** Removes the callback function from the list associated with
        ** each of the SW_MSG_ID's in piMsgId.
        **
        ** swMsgDetach() can safely be called from within a callback
        ** routine even to remove itself from the current callback list.
        ** Removal from the current list does not actually happen
        ** until current list has been fully processed.
        */

    
void swMsgFlush( void );
        /*
        ** Discard all unread messages.
        ** Intended for exceptional condition processing.
        */
    
    
void swMsgSend( SW_MSG_S msg );
        /*
        ** Puts the given message on the event message queue.
        */
    

void swMsgDispatch( SW_MSG_S msg );
        /*
        ** Goes through the list of attached callback routines for the given 
        ** message and calls each one of them in succession, until one of the
        ** callback routines returns non zero.
        **
        ** Recursive calls to swMsgDispatch are not allowed, that
        ** means: DO NOT CALL this routine from any callback routine, or from 
        ** within any routine called by a callback routine.
        */
        
        
void swMsgProcess( void );
        /*
        ** Goes through the message queue and dispatches any message
        ** present. Will continue to process messages until the queue is empty.
        */
        

#if defined( __cplusplus )
	}
#endif
#endif /* __SW_MSG_H__  */
