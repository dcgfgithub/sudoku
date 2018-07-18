/*
** The memory for the nodes used in the list are maintained inside a large
** block of contiguous memory called the node pool. Each node in the pool
** has enough memory to store the node header as well as the optional 
** user data. Using a node pool reduces the number of memory allocations
** required to maintain the list.
**
** After the list is initalized, a node pool is created sufficiently large
** enough to hold an inital number of nodes. All of these nodes are then
** added to a free node list. As new nodes are added to the list, 
** they are taken from the free node list. As nodes are removed from the 
** list, they are returned to the free node list.
**
** Pointers between different nodes are stored as indexes into the node
** pool. This simplifies the implementation somewhat in that as the node
** pool grows in size, the existing node pointers can remain unchanged.
** Using actual memory pointers would require that they be updated 
** if the location of the node pool changes when grown. 
**
** Each node in the node pool belongs either to the doubly linked list, or 
** is part of the free node list. The GTList structure maintains both
** of these lists using three variables, uHead, uTail and uFree which 
** point to the first node in the list, the last node in the list and the
** start of the free node list respectively. Nodes in the free list 
** make use of the uNext member to point to the next node in the free
** list. In addition, the uCurr field points to the current node.
*/
#include <assert.h>
#include <memory.h>
#include <stdlib.h>

#include "sw_dbg.h"
#include "sw_list.h"


/*
** DEFINITIONS
*/

#define SW_LIST_MIN_PAYLOAD   sizeof(void *)
#define SW_LIST_MIN_CAPACITY  4
#define SW_LIST_GROW_FACTOR   2

#define SW_POOL_GETNODE(pool,idx) \
    ((SW_LIST_NODE_S *)((char *)(pool->pnodes) + (pool->node_size*(idx))))

#define SW_NODE_GETPAYLOAD(pnode) \
    ((void *)((char *)(pnode) + sizeof(SW_LIST_NODE_S)));


/*
** LOCAL FUNCTIONS
*/


static void
nodePoolAddFreeNodes(
    SW_LIST_POOL_S * ppool,
    unsigned short   new_capactiy
    )
{
    SW_LIST_NODE_S * pnode;
    unsigned short i;
    
    for( i = ppool->capacity; i < new_capactiy; i++ )
    {
        pnode = SW_POOL_GETNODE( ppool, i );
        pnode->next  = ppool->free_node;
        pnode->prev  = 0;
        ppool->free_node = i;
    }
    ppool->capacity = new_capactiy;
}


static void
nodePoolGrow(
    SW_LIST_POOL_S * ppool,
    unsigned short   min_capacity
    )
{
    /*
    ** Increase the capacity of the node pool so that it can accomodate
    ** at least the number of nodes requested. If the pool is already
    ** large enough, no action is taken.
    **
    ** Node space is allocated using realloc. Because node pointers
    ** use indices to nodes in the node pool as opposed to actual
    ** pointers, adjustments are not necessary. The newly allocated
    ** nodes are added to the free node list.
    */
    void *       pnew_nodes = NULL;
    unsigned int new_capacity = 0;
    
    if( ppool->grow_factor == 0 )
        return; /* Pool is not allowed to grow */
    
    if( min_capacity <= ppool->capacity )
        return;  /* Pool is already big enough */
    
    new_capacity = ppool->capacity;
    
    if( 0 == new_capacity )
        new_capacity = SW_LIST_MIN_CAPACITY;
    
    while( new_capacity < min_capacity )
        new_capacity *= ppool->grow_factor;

    pnew_nodes = realloc( ppool->pnodes, (new_capacity * ppool->node_size) );
    
    if( NULL != pnew_nodes )
    {
        ppool->pnodes = pnew_nodes;
        nodePoolAddFreeNodes( ppool, new_capacity );
    }
}


static unsigned short
nodePoolAlloc(
    SW_LIST_POOL_S * ppool
    )
{
    /*
    ** Get the next free node from the node pool and
    ** return its index. Returns 0 if there are no
    ** more available nodes.
    **
    ** Attempts to grow the pool if there are no more free
    ** nodes available.
    */

    SW_LIST_NODE_S * pnode;
    unsigned short node;
    
    if( 0 == ppool->free_node )
    {
        nodePoolGrow( ppool, ppool->capacity+1 );
    }
    if( 0 == ppool->free_node )
    {
        return 0;
    }
    node  = ppool->free_node;
    pnode = SW_POOL_GETNODE( ppool, node );
    ppool->free_node = pnode->next;
    
    return node;
}


/*
** EXPORTED FUNCTIONS
*/

void
swListPoolInit(
    SW_LIST_POOL_S * p_nodepool_s,
    void *           preallocated,
    unsigned short   capacity,
    unsigned short   payload_size
    )
{
    /*
    ** Initialize a node pool structure.
    ** Assumes structure has been properly zeroed prior to calling.
    */
    
    assert( p_nodepool_s != NULL );
    assert( payload_size > 0 );
    
    p_nodepool_s->pnodes      = preallocated;
    p_nodepool_s->node_size   = sizeof(SW_LIST_NODE_S) + payload_size;
    p_nodepool_s->capacity    = 0;
    p_nodepool_s->free_node   = 0;
    p_nodepool_s->retain      = 0;
    
    if( preallocated == NULL )
    {
        /*
        ** Node pool is responsible for managing its own memory.
        ** Allocate memory large enough for initial capacity.
        **
        ** Use predefined grow factor.
        ** Allowing the user to specify their own grow factor
        ** did not seem useful enough.
        */
        p_nodepool_s->grow_factor = SW_LIST_GROW_FACTOR;
        nodePoolGrow( p_nodepool_s, capacity );
    }
    else
    {
        /*
        ** Node pool is not responsible for managing memory.
        **
        ** Setting grow factor to 0 not only prevents the
        ** pool from growing, but also signals that the memory
        ** should not be deallocated when the pool is freed.
        */
        p_nodepool_s->grow_factor = 0;
        nodePoolAddFreeNodes( p_nodepool_s, capacity );
    }
}


int
swListPoolFree(
    SW_LIST_POOL_S * p_nodepool_s
    )
{
    /*
    ** Frees all memory allocated to the pool.
    ** Operation fails if the retain counter is not zero.
    **
    ** Returns the adjusted retain count.
    ** Should always return 0 if successful.
    */
    assert( NULL != p_nodepool_s );
    
    if( p_nodepool_s->retain > 0 )
        return p_nodepool_s->retain;
    
    if( NULL != p_nodepool_s->pnodes && 0 != p_nodepool_s->grow_factor )
    {
      free( p_nodepool_s->pnodes );
    }

    p_nodepool_s->pnodes    = NULL;
    p_nodepool_s->node_size = 0;
    p_nodepool_s->capacity  = 0;
    
    return 0;
}


void
swListInit(
    SW_LIST_S      * plist,
    SW_LIST_POOL_S * pnodepool
    )
{
    /*
    ** Initializes a new list structure.
    ** Requires a pre-initialized node pool structure.
    */
    
    assert( NULL != plist );
    assert( NULL != pnodepool );
    
    plist->ppool = pnodepool;
    plist->head  = 0;
    plist->tail  = 0;
    plist->curr  = 0;
    
    pnodepool->retain++;
}


void
swListFree(
    SW_LIST_S * plist
    )
{
    /*
    ** Removes all nodes from the list and returns them to the 
    ** free store. Frees association from node pool.
    */
    assert( plist != NULL );
    assert( plist->ppool != NULL );
    assert( plist->ppool->retain > 0 );
    
    swListClear( plist );
    
    plist->ppool->retain--;
    plist->ppool = NULL;
}


void *
swListFirst(
    SW_LIST_S * plist
    )
{
    /*
    ** Sets the current node to be the first node in the list.
    ** Returns a pointer to the start of the payload data.
    ** Returns NULL if the list is empty.
    */
    SW_LIST_NODE_S * pnode;

    assert( NULL != plist );
    assert( NULL != plist->ppool );
    
    if( 0 == plist->head )
        return NULL;

    pnode = SW_POOL_GETNODE( plist->ppool, plist->head );
    plist->curr = plist->head;
    
    return SW_NODE_GETPAYLOAD( pnode );
}


void *
swListLast(
    SW_LIST_S * plist
    )
{
    /*
    ** Set the current node to be the last node in the list.
    ** Returns a pointer to the start of the payload data.
    ** Returns NULL if the list is empty.
    */
    SW_LIST_NODE_S * pnode;

    assert( NULL != plist );
    assert( NULL != plist->ppool );
    
    if( 0 == plist->tail )
        return NULL;

    pnode = SW_POOL_GETNODE( plist->ppool, plist->tail );
    plist->curr = plist->tail;
    
    return SW_NODE_GETPAYLOAD( pnode );
}


void *
swListNext(
    SW_LIST_S * plist
    )
{
    /*
    ** Moves the current node pointer to the node immediately after
    ** the current node. Returns a pointer to the start
    ** of the nodes payload.
    **
    ** If the current node is the last node in the list, it remains
    ** unchanged, and NULL is returned. If the list is empty,
    ** NULL is returned.
    */

    SW_LIST_NODE_S * pnode;
    void * payload = NULL;
    
    assert( NULL != plist );
    assert( NULL != plist->ppool );
    
    if( 0 == plist->curr )
        return NULL;

    pnode = SW_POOL_GETNODE( plist->ppool, plist->curr );
    if( 0 != pnode->next )
    {
        plist->curr = pnode->next;
        pnode = SW_POOL_GETNODE( plist->ppool, plist->curr );
        payload =  SW_NODE_GETPAYLOAD( pnode );
    }
    return payload;
}


void *
swListPrev(
    SW_LIST_S * plist
    )
{
    /*
    ** Moves the current node pointer to the node immediately before
    ** the current node. Returns a pointer to the start
    ** of the new node payload.
    **
    ** If the current node is the first node in the list, it remains
    ** unchanged, and NULL is returned. If the list is empty,
    ** NULL is returned.
    */

    SW_LIST_NODE_S * pnode;
    void * payload = NULL;
    
    assert( NULL != plist );
    assert( NULL != plist->ppool );
    
    if( 0 == plist->curr )
        return NULL;
    
    pnode = SW_POOL_GETNODE( plist->ppool, plist->curr );
    if( 0 != pnode->prev )
    {
        plist->curr = pnode->prev;
        pnode = SW_POOL_GETNODE( plist->ppool, plist->curr );
        payload =  SW_NODE_GETPAYLOAD( pnode );
    }
    return payload;
}


void *
swListCurr(
    SW_LIST_S * plist
    )
{
    /*
    ** Returns a pointer to the payload data for the
    ** current node. Returns NULL if the list is empty.
    */
    SW_LIST_NODE_S * pnode;
    void * payload = NULL;
    
    assert( NULL != plist );
    assert( NULL != plist->ppool );
    
    if( 0 == plist->curr )
        return NULL;
    
    pnode = SW_POOL_GETNODE( plist->ppool, plist->curr );
    payload =  SW_NODE_GETPAYLOAD( pnode );
    
    return payload;
}


void *
swListAddBefore(
    SW_LIST_S * plist
    )
{
    /*
    ** Adds an node to the list immediately before the current node and
    ** changes the current node pointer to the new node.
    ** If there are no more free nodes available, will attempt to grow
    ** the list.
    **
    ** Sets the value of the passed in pointer to the start of the data
    ** for the new node. Sets this value to NULL if the node was not
    ** added.
    */

    SW_LIST_NODE_S * pnode;
    SW_LIST_NODE_S * pcurr;
    unsigned int     node;
    void *           payload = NULL;
    
    assert( NULL != plist );
    assert( NULL != plist->ppool );
    assert( NULL != plist->ppool->pnodes );
  
    node = nodePoolAlloc( plist->ppool );
    if( 0 == node )
    {
        return NULL;
    }
    pnode = SW_POOL_GETNODE( plist->ppool, node );

    if( 0 != plist->curr )
    {
        pcurr = SW_POOL_GETNODE( plist->ppool, plist->curr );
        pnode->prev = pcurr->prev;
        pnode->next = plist->curr;
        pcurr->prev = node;

        if( 0 != pnode->prev )
        {
            pcurr = SW_POOL_GETNODE( plist->ppool, pnode->prev );
            pcurr->next = node;
        }
        if( plist->curr == plist->head )
            plist->head = node;
    }
    else
    {
        pnode->prev = 0;
        pnode->next = 0;
        plist->head = node;
        plist->tail = node;
    }

    plist->curr = node;
    payload = SW_NODE_GETPAYLOAD( pnode );

    return payload;
}


void *
swListAddAfter(
    SW_LIST_S * plist
    )
{
    /*
    ** Adds an node to the list immediately after the current node and
    ** changes the current node to the new node.
    ** If there are no more free nodes available, will attempt to grow
    ** the list.
    **
    ** Returns a pointer to the start of the payload data
    ** for the new node. Returns NULL if the node could not be
    ** added.
    */

    SW_LIST_NODE_S * pnode;
    SW_LIST_NODE_S * pcurr;
    unsigned int     node;
    void *           payload = NULL;
    
    assert( NULL != plist );
    assert( NULL != plist->ppool );
    assert( NULL != plist->ppool->pnodes );
    
    node = nodePoolAlloc( plist->ppool );
    if( 0 == node )
    {
        return NULL;
    }
    pnode = SW_POOL_GETNODE( plist->ppool, node );

    if( 0 != plist->curr )
    {
        pcurr = SW_POOL_GETNODE( plist->ppool, plist->curr );
        pnode->next = pcurr->next;
        pnode->prev = plist->curr;
        pcurr->next = node;

        if( 0 != pnode->next )
        {
            pcurr = SW_POOL_GETNODE( plist->ppool, pnode->next );
            pcurr->prev = node;
        }
        if( plist->curr == plist->tail )
            plist->tail = node;
    }
    else
    {
        pnode->prev = 0;
        pnode->next = 0;
        plist->head = node;
        plist->tail = node;
    }

    plist->curr = node;
    payload = SW_NODE_GETPAYLOAD( pnode );

    return payload;
}


void *
swListRemoveBefore(
    SW_LIST_S * plist
    )
{
    /*
    ** Removes the current node from the list and returns it to
    ** the free store. Changes the current node to the one immediately
    ** before the node being removed.
    **
    ** If the node being removed is the first node in the list, the current
    ** node is set to the one immediately after the removed node.
    ** If the list becomes empty, the current node is set to NULL.
    **
    ** Returns a pointer to the     
    */

    SW_LIST_NODE_S * pnode;
    void *           payload = NULL;
    
    assert( NULL != plist );
    assert( NULL != plist->ppool );
    assert( NULL != plist->ppool->pnodes );
    
    if( 0 != plist->curr )
    {
        pnode = SW_POOL_GETNODE( plist->ppool, plist->curr );
        
        unsigned int prev = pnode->prev;
        unsigned int next = pnode->next;

        pnode->prev = 0;
        pnode->next = plist->ppool->free_node;
        plist->ppool->free_node = plist->curr;

        if( 0 != next )
        {
            pnode = SW_POOL_GETNODE( plist->ppool, next );
            pnode->prev = prev;
        }
        else
        {
            plist->tail = prev;
        }
        if( 0 != prev )
        {
            pnode = SW_POOL_GETNODE( plist->ppool, prev );
            pnode->next = next;
            plist->curr = prev;
        }
        else
        {
            plist->head = next;
            plist->curr = next;
        }
        if( 0 != plist->curr )
        {
            pnode = SW_POOL_GETNODE( plist->ppool, plist->curr );
            payload = SW_NODE_GETPAYLOAD( pnode );
        }
    }
    return payload;
}


void *
swListRemoveAfter(
    SW_LIST_S * plist
    )
{
    /*
    ** Removes the node at the current node pointer and returns it to
    ** the free store. Changes the current node to the one immediately
    ** after the node being removed.
    **
    ** If the node being removed is the last node in the list, the current
    ** node is set to the one immediately after the removed node.
    ** If the list becomes empty, the current node is set to NULL.
    **
    ** Sets the value of the passed in pointer to the start of the data
    ** for the new current node. If the list becomes empty, the value
    ** is set to NULL.
    */

    SW_LIST_NODE_S * pnode;
    void *           payload = NULL;
    
    assert( NULL != plist );
    assert( NULL != plist->ppool );
    assert( NULL != plist->ppool->pnodes );
    
    if( 0 != plist->curr )
    {
        pnode = SW_POOL_GETNODE( plist->ppool, plist->curr );
        
        unsigned int prev = pnode->prev;
        unsigned int next = pnode->next;

        pnode->prev = 0;
        pnode->next = plist->ppool->free_node;
        plist->ppool->free_node = plist->curr;

        if( 0 != prev )
        {
            pnode = SW_POOL_GETNODE( plist->ppool, prev );
            pnode->next = next;
        }
        else
        {
            plist->head = next;
        }
        
        if( 0 != next )
        {
            pnode = SW_POOL_GETNODE( plist->ppool, next );
            pnode->prev = prev;
            plist->curr = next;
        }
        else
        {
            plist->tail = prev;
            plist->curr = prev;
        }
        
        if( 0 != plist->curr )
        {
            pnode = SW_POOL_GETNODE( plist->ppool, plist->curr );
            payload = SW_NODE_GETPAYLOAD( pnode );
        }
    }
    return payload;
}


int
swListIsEmpty(
    SW_LIST_S * plist
    )
{
    /*
    ** Returns non zero if the list is empty, or has not been initialized
    ** yet, otherwise it returns 0
    */
    assert( plist != NULL );

    if( NULL == plist->ppool ||
        NULL == plist->ppool->pnodes )
    {
        return 1;
    }
    return( plist->curr == 0 );
}


void
swListClear(
    SW_LIST_S * plist
    )
{
    /*
    ** Removes all nodes from the list and returns them to the free
    ** store. Does not decrease the size of the free store.
    ** Does nothing if the list is already empty, or has not been
    ** initialized.
    */

    SW_LIST_NODE_S * pnode;
    unsigned short   node;
    unsigned short   next;
    
    assert( NULL != plist );
    
    node = plist->head;
    
    while( node != 0 )
    {
        pnode = SW_POOL_GETNODE( plist->ppool, node );
        next = pnode->next;
        
        pnode->prev = 0;
        pnode->next = plist->ppool->free_node;
        plist->ppool->free_node = node;
        
        node = next;
    }
    plist->curr = 0;
    plist->head = 0;
    plist->tail = 0;
}


void swListDumpPool( SW_LIST_POOL_S * ppool )
{
    assert( ppool != NULL );
    
    SW_LIST_NODE_S * pnode;
    unsigned short   node;
    
    SW_LOG( "Pool : " );
    
    node = ppool->free_node;
    
    while( node != 0 )
    {
        pnode = SW_POOL_GETNODE( ppool, node );
        SW_LOGF( "%d, ", node );
        node = pnode->next;
    }
    
    SW_LOG( "\n" );
}

void swListDumpList( SW_LIST_S * plist )
{
    SW_LIST_NODE_S * pnode;
    unsigned short   node;
    
    assert( NULL != plist );
    
    SW_LOG( "List : " );
    
    node = plist->head;
    
    while( node != 0 )
    {
        pnode = SW_POOL_GETNODE( plist->ppool, node );
        SW_LOGF( "%d, ", node );
        node = pnode->next;
    }
    
    SW_LOG( "\n" );    
}


