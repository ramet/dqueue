/**
 * @author aurelien.esnard@u-bordeaux.fr and pierre.ramet@u-bordeaux.fr
 * @brief Lightweight implementation of double-ended queue data structure
 * following GLib interface.
 * @details For futher details, please visit :
 * https://developer.gnome.org/glib/stable/glib-Double-ended-Queues.html
 **/

#ifndef QUEUE_H
#define QUEUE_H

#include "mymutex.h"

//@{

typedef void *      gpointer;
typedef const void *gconstpointer;
typedef void ( *gfree_function )( gpointer data );

/* *********************************************************** */
/*                          QUEUE                              */
/* *********************************************************** */

struct queue_s {
    struct element_s *head;
    struct element_s *tail;
    unsigned int      length;
    gfree_function    free_func;
    mymutex           mutex;
};

struct element_s {
    gpointer          value;
    struct element_s *next;
    struct element_s *prev;
};

typedef struct queue_s   queue_t;
typedef struct element_s element_t;

queue_t *queue_init( gfree_function free_func );
void     queue_push_head( queue_t *q, gpointer v );
gpointer queue_pop_tail( queue_t *q );
void     queue_drop_tail( queue_t *q );
int      queue_length( queue_t *q );
bool     queue_is_empty( queue_t *q );
gpointer queue_peek_head( queue_t *q );
gpointer queue_peek_tail( queue_t *q );
void     queue_free( queue_t *q );

//@}

#endif
