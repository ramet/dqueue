#include <stdatomic.h>
#ifdef __STDC_NO_THREADS__
#include "c11threads.h"
#else
#include <threads.h>
#endif

typedef struct mutex_ {
    atomic_int lock;
    thrd_t     owner;
} mymutex;

#define UNLOCKED 0
#define LOCKED 1
#define UNASSIGNED_OWNER 0

int mutex_init( mymutex *mtx );
int mutex_lock( mymutex *mtx );
int mutex_unlock( mymutex *mtx );
int mutex_destroy( mymutex *mtx );
