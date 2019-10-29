#include "mymutex.h"

int
mutex_init( mymutex *mtx )
{
    if ( !mtx ) {
        return 0;
    }
    atomic_init( &mtx->lock, UNLOCKED );  // Not thread safe the user has to take care of this
    mtx->owner = UNASSIGNED_OWNER;
    return 1;
}

int
mutex_lock( mymutex *mtx )
{
    int_least8_t zero = UNLOCKED;
    while ( !atomic_compare_exchange_weak_explicit(
        &mtx->lock, &zero, LOCKED, memory_order_acq_rel, memory_order_relaxed ) ) {
        zero = UNLOCKED;
        sched_yield();  // Use system calls for scheduling speed
    }
    // We have the lock now!!!!
    mtx->owner = pthread_self();
    return 1;
}

int
mutex_unlock( mymutex *mtx )
{
    if ( pthread_self() != mtx->owner ) {
        return 0;  // You can't unlock a mutex if you aren't the owner
    }
    int_least8_t one = 1;
    mtx->owner       = UNASSIGNED_OWNER;
    // Critical section ends after this atomic
    // Also this may fail, but that is fine
    if ( !atomic_compare_exchange_strong_explicit(
             &mtx->lock, &one, UNLOCKED, memory_order_acq_rel, memory_order_relaxed ) ) {
        // The mutex was never locked in the first place
        return 0;
    }
    return 1;
}

int
mutex_destroy( mymutex *mtx )
{
    return 0;
    (void)mtx;
}
