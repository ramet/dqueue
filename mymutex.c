#include "mymutex.h"

int
mutex_init( mymutex *mtx )
{
    if ( !mtx ) {
        return 0;
    }
    atomic_init( &mtx->lock, UNLOCKED );  // Not thread safe
    mtx->owner = UNASSIGNED_OWNER;
    return 1;
}

int
mutex_lock( mymutex *mtx )
{
    int zero = UNLOCKED;
    while ( !atomic_compare_exchange_weak_explicit(
        &mtx->lock, &zero, LOCKED, memory_order_acq_rel, memory_order_relaxed ) ) {
        zero = UNLOCKED;
        thrd_yield();  // Use system calls for scheduling speed
    }
    // We have the lock now!!!!
    mtx->owner = thrd_current();
    return 1;
}

int
mutex_unlock( mymutex *mtx )
{
    if ( thrd_current() != mtx->owner ) {
        return 0;  // You can't unlock a mutex if you aren't the owner
    }
    int one = 1;
    mtx->owner       = UNASSIGNED_OWNER;
    // Critical section ends after this atomic
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
