#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "queue.h"

/* *********************************************************** */

queue_t *
queue_init( gfree_function f )
{
    queue_t *q = malloc( sizeof( queue_t ) );
    assert( q );
    q->length = 0;
    q->tail = q->head = NULL;
    q->free_func      = f;
    mutex_init( &q->mutex );
    return q;
}

/* *********************************************************** */

void
queue_push_head( queue_t *q, gpointer v )
{
    assert( q );
    element_t *e = malloc( sizeof( element_t ) );
    assert( e );
    e->value = v;
    e->prev  = NULL;
    mutex_lock( &q->mutex );
    e->next = q->head;
    if ( q->head )
        q->head->prev = e;
    q->head = e;
    if ( !q->tail )
        q->tail = e;
    q->length++;
    mutex_unlock( &q->mutex );
}

/* *********************************************************** */

gpointer
queue_pop_tail( queue_t *q )
{
    assert( q );
    mutex_lock( &q->mutex );
    assert( q->length > 0 );
    assert( q->tail );
    gpointer   v    = q->tail->value;
    element_t *prev = q->tail->prev;
    if ( prev )
        prev->next = NULL;
    free( q->tail );
    q->tail = prev;
    q->length--;
    if ( !q->tail )
        q->head = NULL;
    mutex_unlock( &q->mutex );
    return v;
}

/* *********************************************************** */

void
queue_drop_tail( queue_t *q )
{
    assert( q );
    mutex_lock( &q->mutex );
    assert( q->length > 0 );
    assert( q->tail );
    element_t *prev = q->tail->prev;
    if ( prev )
        prev->next = NULL;
    if ( q->free_func != NULL )
        q->free_func( q->tail->value );
    free( q->tail );
    q->tail = prev;
    q->length--;
    if ( !q->tail )
        q->head = NULL;
    mutex_unlock( &q->mutex );
}

/* *********************************************************** */

int
queue_length( queue_t *q )
{
    assert( q );
    int length;
    mutex_lock( &q->mutex );
    length = q->length;
    mutex_unlock( &q->mutex );
    return length;
}

/* *********************************************************** */

bool
queue_is_empty( queue_t *q )
{
    assert( q );
    bool empty;
    mutex_lock( &q->mutex );
    empty = ( q->length == 0 );
    mutex_unlock( &q->mutex );
    return empty;
}

/* *********************************************************** */

gpointer
queue_peek_head( queue_t *q )
{
    assert( q );
    gpointer v;
    mutex_lock( &q->mutex );
    assert( q->head );
    v = q->head->value;
    mutex_unlock( &q->mutex );
    return v;
}

/* *********************************************************** */

gpointer
queue_peek_tail( queue_t *q )
{
    assert( q );
    gpointer v;
    mutex_lock( &q->mutex );
    assert( q->tail );
    v = q->tail->value;
    mutex_unlock( &q->mutex );
    return v;
}

/* *********************************************************** */

void
queue_free( queue_t *q )
{
    assert( q );
    element_t *e = q->head;
    while ( e ) {
        element_t *tmp = e;
        e              = e->next;
        if ( q->free_func != NULL )
            q->free_func( tmp->value );
        free( tmp );
    }
    mutex_destroy( &q->mutex );
    free( q );
}

/* *********************************************************** */
