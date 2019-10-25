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
    mtx_init( &q->mutex, mtx_plain );
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
    mtx_lock( &q->mutex );
    e->next = q->head;
    if ( q->head )
        q->head->prev = e;
    q->head = e;
    if ( !q->tail )
        q->tail = e;
    q->length++;
    mtx_unlock( &q->mutex );
}

/* *********************************************************** */

gpointer
queue_pop_tail( queue_t *q )
{
    assert( q );
    mtx_lock( &q->mutex );
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
    mtx_unlock( &q->mutex );
    return v;
}

/* *********************************************************** */

void
queue_drop_tail( queue_t *q )
{
    assert( q );
    mtx_lock( &q->mutex );
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
    mtx_unlock( &q->mutex );
}

/* *********************************************************** */

int
queue_length( queue_t *q )
{
    assert( q );
    int length;
    mtx_lock( &q->mutex );
    length = q->length;
    mtx_unlock( &q->mutex );
    return length;
}

/* *********************************************************** */

bool
queue_is_empty( queue_t *q )
{
    assert( q );
    bool empty;
    mtx_lock( &q->mutex );
    empty = ( q->length == 0 );
    mtx_unlock( &q->mutex );
    return empty;
}

/* *********************************************************** */

gpointer
queue_peek_head( queue_t *q )
{
    assert( q );
    gpointer v;
    mtx_lock( &q->mutex );
    assert( q->head );
    v = q->head->value;
    mtx_unlock( &q->mutex );
    return v;
}

/* *********************************************************** */

gpointer
queue_peek_tail( queue_t *q )
{
    assert( q );
    gpointer v;
    mtx_lock( &q->mutex );
    assert( q->tail );
    v = q->tail->value;
    mtx_unlock( &q->mutex );
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
    mtx_destroy( &q->mutex );
    free( q );
}

/* *********************************************************** */
