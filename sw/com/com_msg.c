
/*
** PREREQUISITES
*/
#include <stdlib.h>
#include <memory.h>
#include <assert.h>

#include "sw_cfg.h"
#include "sw_def.h"
#include "sw_list.h"
#include "sw_msg.h"


/*
** DEFINITIONS
*/

struct _SW_MSG_QUEUE_NODE_S
{
    SW_LIST_NODE_S node;  /* Must always be first member */
    SW_MSG_S       msg;   /* Payload */
};
typedef struct _SW_MSG_QUEUE_NODE_S SW_MSG_QUEUE_NODE_S;
        /*
        ** Structure that conforms to the requirements for custom
        ** list node. Used for the message queue, which will be
        ** implemented as a list, as its operations can be
        ** used to mirror queue operations.
        */

static SW_MSG_QUEUE_NODE_S sw_msg_queue_node_as[SW_MSG_MAX_QUEUE_NODES];
static SW_LIST_POOL_S      sw_msg_queue_pool_s = {0};
static SW_LIST_S           sw_msg_queue_s = {0};
        /*
        ** Statically allocated queue node pool.
        */


struct _SW_MSG_CALLBACK_DATA_S
{
    SW_MSG_CALLBACK_PF pfCallback;
    int                priority;
};
typedef struct _SW_MSG_CALLBACK_DATA_S SW_MSG_CALLBACK_DATA_S;

struct _SW_MSG_CALLBACK_NODE_S
{
    SW_LIST_NODE_S         node;  /* Must always be first member */
    SW_MSG_CALLBACK_DATA_S data;
};
typedef struct _SW_MSG_CALLBACK_NODE_S SW_MSG_CALLBACK_NODE_S;
        /*
        ** Structure conforming to requirements for custom list node
        ** used for callback lists and the pending list.
        */

static SW_MSG_CALLBACK_NODE_S sw_msg_callback_node_as[SW_MSG_MAX_CALLBACK_NODES];
static SW_LIST_POOL_S         sw_msg_callback_pool_s = {0};

static SW_LIST_S sw_msg_callback_list_a[256] =
{
    /*
    ** Each type of msg_id is associated with its
    ** own callback list. Attached messages are added to
    ** the appropriate list.
    */
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}
};


SW_LIST_S sw_msg_pending_attach_list = {0};
SW_LIST_S sw_msg_pending_detach_list = {0};
        /*
        ** List of pending attach and detach requests
        ** During callback processing for a message,
        ** all attach and detach requests are put on these
        ** lists.  Afterwards, Attach and Detach requests
        ** for the message are performed before the next message.
        ** They are performed in the order they were received.
        */

SW_MSG_ID sw_msg_pending_msg_id = SW_MSG_NONE ;
SW_MSG_ID sw_msg_pending_ai[]   = { SW_MSG_NONE, SW_MSG_NONE } ; /* To have it as SW_MSG_NONE terminated array */


/*
** EXPORTED FUNCTIONS
*/

int swMsgInit( void )
{
	int i;
	/* 
	** Initialize the event queue system. This routine must be called
	** once before either sending or receiving events.
	*/
    swListPoolInit( &sw_msg_queue_pool_s,
                    &sw_msg_queue_node_as, SW_MSG_MAX_QUEUE_NODES,
                    sizeof(SW_MSG_S) );
    
    swListPoolInit( &sw_msg_callback_pool_s,
                    &sw_msg_callback_node_as, SW_MSG_MAX_CALLBACK_NODES,
                    sizeof(SW_MSG_CALLBACK_DATA_S) );
    
    for( i = 0; i < SW_ARRSIZE(sw_msg_callback_list_a); i++ )
    {
        swListInit( &sw_msg_callback_list_a[i],
                    &sw_msg_callback_pool_s );
    }
    swListInit( &sw_msg_pending_attach_list, &sw_msg_callback_pool_s );
    swListInit( &sw_msg_pending_detach_list, &sw_msg_callback_pool_s );
    swListInit( &sw_msg_queue_s, &sw_msg_queue_pool_s );
    
	return 1;
}


void swMsgFree( void )
{
	int i;
	/* 
	** Cleanup and remove the queue system. 
	** All unused events still on the queue are removed
	** and their associated memory freed.
	*/
	SW_MSG_CALLBACK_PF       pfuncCallBack ;
	SW_MSG_CALLBACK_DATA_S * psCallback;
	SW_MSG_S                 msg;
	/*
	** For each MSG_ID's list cleanup the list.
	** Each callback on each list must be sent a MSG_DETACH message.
	*/
	for( i=1; i<SW_ARRSIZE(sw_msg_callback_list_a); i++ )
    {
		/* i=0 corresponds to MSG_NONE. Reserved (not a list) */
        /*
        ** For each list entry from last to first.
        ** Invoke the callback routine with the detach message.
        */
        msg = swMsgNull( SW_MSG_DETACH ) ;
        msg.nVal[0] = i;
        psCallback = swListLast( &sw_msg_callback_list_a[i] ) ;
        while( psCallback )
        {
            pfuncCallBack = psCallback->pfCallback;
            (void)pfuncCallBack( msg ) ;
            psCallback = swListPrev( &sw_msg_callback_list_a[i] ) ;
        }
        /*
        ** Give all the list's resource back to the system.
        */
        swListFree( &sw_msg_callback_list_a[i] );
    }
    swListFree( &sw_msg_pending_attach_list );
    swListFree( &sw_msg_pending_detach_list );
    swListFree( &sw_msg_queue_s );
    
    swListPoolFree( &sw_msg_callback_pool_s );
    swListPoolFree( &sw_msg_queue_pool_s );
}


SW_MSG_S swMsgNull( SW_MSG_ID msg )
{
    /*
    ** Returns a SW_MSG_S initialized to Nul values, and the given SW_MSG_ID.
    */
	SW_MSG_S sMsg;
	memset( &sMsg, 0, sizeof(sMsg) );
	sMsg.msg = msg ;
	return sMsg;
}


void swMsgAttach(
    SW_MSG_ID *         piMsgId,
    SW_MSG_CALLBACK_PF  pfuncCallback,
    int                 priority
    )
{
	int i;
	/*
	** Puts the callback function on the list(s) associated with
	** each of the SW_MSG_ID's in piMsgId.
	**
	** swMsgAttach() can safely be called from within a callback
	** routine. New attachments are only actually added to a given
	** SW_MSG_ID's list once processing of that list's callbacks is complete.
	**
	*/
    SW_MSG_CALLBACK_DATA_S * psCallbackData;
	SW_MSG_ID iMsgId = 0;
	/*
	** For each message ID in the array terminated by a MSG_NONE
	** Put the callback routine on the list for that id, at the 
	** position indicated by the given priority.
	*/
	assert( SW_MSG_NONE == 0 );
    
	for( i=0; piMsgId[i] != SW_MSG_NONE; i++ )
    {
        iMsgId = piMsgId[i];
		assert( iMsgId < SW_ARRSIZE(sw_msg_callback_list_a) && iMsgId > 0 ) ;
        
		if( iMsgId == sw_msg_pending_msg_id )
        {
			/*
			** We are being called from a call back routine for this message.
			** Put the Attach request on the pending list.
			** Detach requests go on same list with a BSC_INT16_MAX iValue
			** as an indicator.
			*/
			psCallbackData = swListAddAfter( &sw_msg_pending_attach_list );
			psCallbackData->pfCallback = pfuncCallback;
            psCallbackData->priority   = priority;
			continue;
        }
        /*
        ** Advance to the first higher priority value in the list.
        */
        psCallbackData = swListFirst( &sw_msg_callback_list_a[iMsgId] );
        
        while( psCallbackData && psCallbackData->priority <= priority )
        {
            psCallbackData = swListNext( &sw_msg_callback_list_a[iMsgId] );
        }
        /*
        ** Put the given entry immediately before the higher priority one.
        */
        if( psCallbackData != NULL )
            psCallbackData = swListAddBefore( &sw_msg_callback_list_a[iMsgId] );
        else
            psCallbackData = swListAddAfter ( &sw_msg_callback_list_a[iMsgId] );
        
        assert( psCallbackData != NULL );
        
        psCallbackData->priority   = priority ;
        psCallbackData->pfCallback = pfuncCallback ;
    }
}


void swMsgDetach(
    SW_MSG_ID *        piMsgId,
    SW_MSG_CALLBACK_PF pfuncCallback
    )
{
	int i;
	/*
	** Removes the callback function from the list associated with
	** each of the MSG_ID's in  piMsgId.
	**
	** Calling detach has the effect of triggering a SW_MSG_DETACH
	** message virtually being sent to the callback routine,
	** once for each list detached from.
	**
	** MSG_Detach() can safely be called from within a callback 
	** routine even to remove itself from the current callback list.
	** Removal from the current list does not actually happen
	** until current list has been fully processed.
	*/
    SW_MSG_ID msg_id;
    SW_MSG_CALLBACK_DATA_S * psCallbackData;
    int iFoundOne;
	/*
	** For each message ID in the array terminated by a MSG_NONE
	** Find and remove the callback routine on that list.
	*/
	assert( SW_MSG_NONE == 0 );
    
	for( i=0; piMsgId[i] != SW_MSG_NONE; i++ )
    {
        msg_id = piMsgId[i];
		assert( msg_id < SW_ARRSIZE(sw_msg_callback_list_a) && msg_id > 0 ) ;
    
		if( msg_id == sw_msg_pending_msg_id )
        {
			/*
			** We are being called from a call back routine for this message.
			** Put the Detach request on the pending list with a BSC_INT16_MAX 
			** in the iValue as an indicator that this is a detach request
			*/
			psCallbackData = swListAddAfter( &sw_msg_pending_detach_list );
			psCallbackData->priority   = 0;
			psCallbackData->pfCallback = pfuncCallback;
			continue ;
        }
		/*
		** Remove every instance of that callback from the list
		** there should be only one, but removing them all will
		** get rid of any rogues.
		*/
		iFoundOne = 0;
		psCallbackData = swListFirst( &sw_msg_callback_list_a[msg_id] );
		while( psCallbackData )
        {
			if( psCallbackData->pfCallback == pfuncCallback )
            {
				iFoundOne = 1;
				psCallbackData = swListRemoveAfter( &sw_msg_callback_list_a[msg_id] );
            }
			else
				psCallbackData = swListNext( &sw_msg_callback_list_a[msg_id] );
        }
		if( iFoundOne )
        {
            /*
            ** Issue final SW_MSG_DETACH to callback to notify
            ** them not to expect any more.
            */
            SW_MSG_S msg = swMsgNull( SW_MSG_DETACH );
            msg.nVal[0] = msg_id;
            (void)pfuncCallback( msg ) ;
        }
    }
}


void swMsgFlush( void )
{
    /*
    ** Discard all unread messages.
    ** Intended for exceptional condition processing.
    */
    swListClear( &sw_msg_queue_s );
}


void swMsgSend( SW_MSG_S msg )
{
    /*
    ** Puts the given message on the event message queue.
    */
    SW_MSG_S * psmsg;
    
    swListLast( &sw_msg_queue_s );
    psmsg = swListAddAfter( &sw_msg_queue_s );
    assert( psmsg != NULL );
    *psmsg = msg;
}


void swMsgDispatch( SW_MSG_S msg )
{
	/*
	** Goes through the list of attached callback routines for the given
	** message and calls each one of them in succession, until one of the
	** callback routines returns non-zero (meaning ignore the rest).
	** Needs to accomodate possible calls to Attach and Detach from within
	** the callback routines.
	** To allow this, both Attach and Detach queue up requests which 
	** concern the currently processing MSG_ID instead of performing
	** them. So when this routine has finished processing a list of
	** callbacks, it must submit all the queued up requests to either
	** MSG_Attach or MSG_Detach.
	*/
	SW_MSG_CALLBACK_DATA_S * psCallbackData;
    int result;
    
    assert( msg.msg >= SW_MSG_NONE && msg.msg <= SW_MSG_ID_USER_END );
    assert( msg.msg != SW_MSG_DETACH );
    
	/*
	** Indicate the message ID being processed for the Attach and Detach 
	** routine's benefit.
	** Perform all the callbacks for the given message.
	*/
	sw_msg_pending_msg_id = msg.msg ; /* Attach and Detach Requests for this one will be queued up */

	psCallbackData = swListFirst( &sw_msg_callback_list_a[msg.msg] ) ;
	while( psCallbackData )
    {
        result = psCallbackData->pfCallback( msg );
        if( result == 0 )
            psCallbackData = swListNext( &sw_msg_callback_list_a[msg.msg] ) ;
        else
            psCallbackData = NULL;
    }
	/*
	** Clear the indicator which inhibits attach and detach then.
	** Perform attach and detach requests put on pending list during callback processing.
	*/
	sw_msg_pending_msg_id = SW_MSG_NONE ; /* This way attach and detach requests won't be requeued */
	sw_msg_pending_ai[0]  = msg.msg;
    
	psCallbackData = swListFirst( &sw_msg_pending_attach_list );
	while( psCallbackData != NULL )
    {
        swMsgAttach(
                sw_msg_pending_ai,
                psCallbackData->pfCallback,
                psCallbackData->priority
                );
        psCallbackData = swListRemoveAfter( &sw_msg_pending_attach_list );
    }
        
    psCallbackData = swListFirst( &sw_msg_pending_detach_list );
    while( psCallbackData != NULL )
    {
        swMsgDetach(
                sw_msg_pending_ai,
                psCallbackData->pfCallback
                );
        psCallbackData = swListRemoveAfter( &sw_msg_pending_detach_list );
    }
}


void swMsgProcess( void )
{
    /*
    ** Goes through the message queue and dispatches any message
    ** present. Will continue to process messages until the queue is empty.
    */
	SW_MSG_S * psmsg;
    
    psmsg = swListFirst( &sw_msg_queue_s );
    while( psmsg )
    {
        /*
        ** Because dispatching this message may cause additional
        ** messages to be queued, we cannot guarantee the integrity of 
        ** the message queue between messages. Therefore, remove the message
        ** before dispatching it, and then revert back to the head for
        ** the next message.
        */
        swListRemoveAfter( &sw_msg_queue_s );
        swMsgDispatch( *psmsg );
        psmsg = swListFirst( &sw_msg_queue_s );
    }
}



