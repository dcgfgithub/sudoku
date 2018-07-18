#ifndef __SW_LIST__
#define __SW_LIST__
/*
** Module Prefix : swList
**
** PROLOGUE:
**
** Provides an implemenation of a doubly linked list data structure.
**
** Doubly linked lists are useful in situations where you are required to
** perform a large amount of insertion and deletion operations, as they
** can be done in constant time. Access to elements is done O(n), because
** you are required to iterate through the list to find the item you
** want. However, because the list is doubly linked, you can iterate
** both forward and backward.
**
** Each instance of a doubly linked list maintains its own pointer to a
** current node. Operations are available to change the current node, and
** this is how you iterate through the list. The current node always
** points to a valid node, with the only exception being if the list is
** empty, in which case it is NULL.
**
** Insertions and deletions to and from the list are done relative to
** the location of the current node.
**
** Each node in the list can maintain a user payload of any size.
** Operations that retrieve nodes always return a pointer to
** the start of the payload for that node.
*/

/* 
** DEFINITIONS
*/

struct _SW_LIST_NODE_S
{
    unsigned short next;
    unsigned short prev;
};
typedef struct _SW_LIST_NODE_S SW_LIST_NODE_S;

struct _SW_LIST_POOL_S
{
    SW_LIST_NODE_S * pnodes;
        /*
        ** Points to the start of the node pool.
        ** Each node will have enough space for the node structure
        ** as well as the user payload.
        **
        ** Nodes are identified by their ordinal in the pool.
        ** This makes it easy to resize the node pool if necessary
        ** without invalidating any existing nodes.
        **
        ** Node 0 is always reserved as a NULL node, and will
        ** never be used.
        */
    unsigned short node_size;     /* User defined payload size in bytes */
    unsigned short capacity;      /* Total capacity in number of nodes  */
    unsigned short grow_factor;
        /*
        ** Growth rate of node pool when all nodes are used.
        ** If set to 0, node pool is fixed size.
        */
    unsigned short free_node;
        /*
        ** Index to the first free node.
        ** If this value is 0, there are no more free nodes.
        */
    unsigned int retain;
        /*
        ** Retain counter.
        ** Counts the number of lists using the pool.
        */
};
typedef struct _SW_LIST_POOL_S SW_LIST_POOL_S;

struct _SW_LIST_S
{
    unsigned short head;
    unsigned short tail;
    unsigned short curr;
    
    SW_LIST_POOL_S * ppool;
            /*
            ** Each list maintains a pointer to a shared node pool.
            ** This allows for the creation of lightweight lists that
            ** can be used in list pools. (Collections of lists that all
            ** store the same data).
            */
};
typedef struct _SW_LIST_S SW_LIST_S;



/*
** EXPORTED FUNCTIONS
*/

#if defined(__cplusplus)
  extern "C" {
#endif

                  
void
swListPoolInit(
        SW_LIST_POOL_S * p_nodepool_s,  /* Uninitialized structure */
        void * preallocated,
                /*
                ** If this value is NULL, the pool will manage its own
                ** memory. It will allocate an initial block of nodes, and
                ** when those nodes are used up, it will attempt to
                ** allocate additional space at predetermined size intervals.
                **
                ** If this value is not NULL, it should point to the start
                ** of a preallocated array of structures that will be used as
                ** the list nodes. The first member of the structure MUST
                ** always be an instance of a SW_LIST_NODE_S structure.
                ** When all of the nodes are used up, the node pool does not
                ** grow, and any attempt to add new list nodes will fail.
                */
        unsigned short initial_capacity,
                /*
                ** If preallocated is NULL, this value is
                ** how many nodes you want to preallocate for the pool.
                ** Passing in 0 will leave the node pool empty, and memory
                ** will only be allocated when the first node is added.
                **
                ** If preallocated is not NULL, this is the total number
                ** of nodes contained in the preallocated block.
                */
        unsigned short payload_size
                /*
                ** The size in bytes of the user payload data.
                **
                ** If preallocated is NULL, this value can be 0. 
                ** In this case, a default payload size is used, which is
                ** sufficient to hold a void * variable.
                **
                ** If preallocated is not NULL, this value should not 
                ** include 
                */
        );
        /*
        ** Initializes a node pool structure.
        ** You have the option of allowing the node pool to manage its
        ** own memory, or you can provide your own block of memory to use
        ** to hold the nodes.
        */
      
      
    
int
swListPoolFree(
        SW_LIST_POOL_S * p_nodepool_s
        );
        /*
        ** Frees the node pool, releasing all its allocated memory.
        ** Returns 0 if the allocation was successful.
        **
        ** Returns non-zero if the node pool is still being used by one
        ** or more lists. You must free all lists that use this node pool
        ** before attempting to free it.
        **
        ** The number returned indicates how many lists are still using
        ** the node pool.
        */
      

void
swListInit(
        /*
        ** Initializes a new list structure.
        */
        SW_LIST_S      * p_list_s,
        SW_LIST_POOL_S * p_nodepool_s
        );
    

void
swListFree(
        /*
        ** Frees a list returning any nodes it has back to the node
        ** pool. Also releases its reference to the pool.
        */
        SW_LIST_S * plist
        );


void * swListFirst( SW_LIST_S * plist );
void * swListLast ( SW_LIST_S * plist );
void * swListNext ( SW_LIST_S * plist );
void * swListPrev ( SW_LIST_S * plist );
void * swListCurr ( SW_LIST_S * plist );
        /*
        ** List navigation functions.
        ** All of them ( with the exception of swListCurr ) can potentially
        ** change the value of the current node pointer.
        **
        ** All of them return a pointer to the start of the 
        ** user payload block.
        */


void *
swListAddBefore(
        SW_LIST_S * plist
        );
        /*
        ** Adds a node to the list immediately before the current node.
        ** The current node is changed to be the new node.
        **
        ** Returns a pointer to the user payload of the new node,
        ** or NULL if the operation failed.
        */
      
    

void *
swListAddAfter(
        SW_LIST_S * plist
        );
        /*
        ** Adds a node to the list immediately after the current node.
        ** The current node is changed to be the new node.
        **
        ** Returns a pointer to the start of the user payload block for the new node,
        ** or NULL if the operation failed.
        */
      
    

void *
swListRemoveBefore(
        SW_LIST_S * plist
        );
        /*
        ** Removes the current node and changes the current node to be the node
        ** immediately before it.
        **
        ** If the current node is the first node in the list, the current node is
        ** changed to be the node immediately after it.
        **        
        ** If there is only one node in the list, the current node is set to 0.
        */
    

void *
swListRemoveAfter(
        SW_LIST_S * plist
        );
        /*
        ** Removes the node at the current pointer position and moves the
        ** current pointer to the node immediately after the one being removed.
        **
        ** If the current pointer is at the last element in the list, it is changed
        ** to point to the node immediately before the one being removed.
        **
        ** If the node being removed is the only node in the list, the current
        ** node is set to NULL.
        */
    

int
swListIsEmpty(
        SW_LIST_S * plist
        );
        /*
        ** Returns non-zero if the list is empty.
        */
    

void
swListClear(
        SW_LIST_S * plist
        );
        /*
        ** Removes all nodes from the list and returns them to
        ** the node pool.
        */
    

void swListDumpPool( SW_LIST_POOL_S * ppool );
void swListDumpList( SW_LIST_S * plist );
      
      
#if defined( __cplusplus )
    }
#endif

#endif /* __SW_LIST__ */
