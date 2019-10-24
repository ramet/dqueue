#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>

#include "queue.h"

/* *********************************************************** */

struct queue_s {
  struct element_s* head;
  struct element_s* tail;
  unsigned int length;
  gfree_function free_func;
  pthread_mutex_t mutex;
};

/* *********************************************************** */

struct element_s {
  gpointer value;
  struct element_s* next;
  struct element_s* prev;
};

/* *********************************************************** */

typedef struct queue_s queue_t;
typedef struct element_s element_t;

/* *********************************************************** */

queue_t* queue_init(gfree_function f) {
  queue_t* q = malloc(sizeof(queue_t));
  assert(q);
  q->length = 0;
  q->tail = q->head = NULL;
  q->free_func = f;
  pthread_mutex_init(&q->mutex, NULL);
  return q;
}

/* *********************************************************** */

void queue_push_head(queue_t* q, gpointer v) {
  assert(q);
  element_t* e = malloc(sizeof(element_t));
  assert(e);
  e->value = v;
  e->prev = NULL;
  pthread_mutex_lock(&q->mutex);
  e->next = q->head;
  if (q->head) q->head->prev = e;
  q->head = e;
  if (!q->tail) q->tail = e;
  q->length++;
  pthread_mutex_unlock(&q->mutex);
}

/* *********************************************************** */

gpointer queue_pop_tail(queue_t* q) {
  assert(q);
  pthread_mutex_lock(&q->mutex);
  assert(q->length > 0);
  assert(q->tail);
  gpointer v = q->tail->value;
  element_t* prev = q->tail->prev;
  if (prev) prev->next = NULL;
  free(q->tail);
  q->tail = prev;
  q->length--;
  if (!q->tail) q->head = NULL;
  pthread_mutex_unlock(&q->mutex);
  return v;
}

/* *********************************************************** */

void queue_drop_tail(queue_t* q) {
  assert(q);
  pthread_mutex_lock(&q->mutex);
  assert(q->length > 0);
  assert(q->tail);
  element_t* prev = q->tail->prev;
  if (prev) prev->next = NULL;
  if (q->free_func != NULL)
      q->free_func(q->tail->value);
  free(q->tail);
  q->tail = prev;
  q->length--;
  if (!q->tail) q->head = NULL;
  pthread_mutex_unlock(&q->mutex);
}

/* *********************************************************** */

int queue_length(queue_t* q) {
  assert(q);
  int length;
  pthread_mutex_lock(&q->mutex);
  length = q->length;
  pthread_mutex_unlock(&q->mutex);
  return length;
}

/* *********************************************************** */

bool queue_is_empty(queue_t* q) {
  assert(q);
  bool empty;
  pthread_mutex_lock(&q->mutex);
  empty = (q->length == 0);
  pthread_mutex_unlock(&q->mutex);
  return empty;
}

/* *********************************************************** */

gpointer queue_peek_head(queue_t* q) {
  assert(q);
  gpointer v;
  pthread_mutex_lock(&q->mutex);
  assert(q->head);
  v = q->head->value;
  pthread_mutex_unlock(&q->mutex);
  return v;
}

/* *********************************************************** */

gpointer queue_peek_tail(queue_t* q) {
  assert(q);
  gpointer v;
  pthread_mutex_lock(&q->mutex);
  assert(q->tail);
  v = q->tail->value;
  pthread_mutex_unlock(&q->mutex);
  return v;
}

/* *********************************************************** */
/* queue_free method does not have to lock the mutex.
It is not correct to call queue_free if the queue might
still be in use. */

void queue_free(queue_t* q) {
  assert(q);
  element_t* e = q->head;
  while (e) {
    element_t* tmp = e;
    e = e->next;
    if (q->free_func != NULL)
      q->free_func(tmp->value);
    free(tmp);
  }
  pthread_mutex_destroy(&q->mutex);
  free(q);
}

/* *********************************************************** */
