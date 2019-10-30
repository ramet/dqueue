#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gtest/gtest.h"

extern "C" {
#if defined(HAVE_STD_ATOMIC)
using namespace std;
#endif
#include "queue.h"
}

typedef int mytype_t;

void
myfree( gpointer p )
{
    free( p );
}

/* ********** TEST INIT & FREE ********** */

TEST( QueueTest, Init_Free )
{
    queue_t *q = queue_init( myfree );
    EXPECT_FALSE( NULL == q );
    queue_free( q );
}

/* ********** TEST PUSH ********** */

TEST( QueueTest, Push )
{
    queue_t *q = queue_init( myfree );
    for ( int i = 0; i < 100; i++ ) {
        mytype_t *ptr_in = (mytype_t *)malloc( sizeof( mytype_t ) );
        *ptr_in          = i;
        queue_push_head( q, ptr_in );
        mytype_t *ptr_head = (mytype_t *)queue_peek_head( q );
        EXPECT_EQ( *ptr_head, (mytype_t)i );
        mytype_t *ptr_tail = (mytype_t *)queue_peek_tail( q );
        EXPECT_EQ( *ptr_tail, (mytype_t)0 );
    }
    queue_free( q );
}

/* ********** TEST POP ********** */

TEST( QueueTest, Pop )
{
    queue_t *q = queue_init( myfree );
    for ( int i = 0; i < 100; i++ ) {
        mytype_t *ptr_in = (mytype_t *)malloc( sizeof( mytype_t ) );
        *ptr_in          = i;
        queue_push_head( q, ptr_in );
    }
    for ( int i = 0; i < 100; i++ ) {
        mytype_t *ptr_out = (mytype_t *)queue_pop_tail( q );
        EXPECT_EQ( *ptr_out, (mytype_t)i );
        myfree( ptr_out );
    }
    queue_free( q );
}

/* ********** TEST DROP ********** */

TEST( QueueTest, Drop )
{
    queue_t *q = queue_init( myfree );
    for ( int i = 0; i < 100; i++ ) {
        mytype_t *ptr_in = (mytype_t *)malloc( sizeof( mytype_t ) );
        *ptr_in          = i;
        queue_push_head( q, ptr_in );
    }
    for ( int i = 0; i < 100; i++ )
        queue_drop_tail( q );
    EXPECT_EQ( queue_length( q ), 0 );
    queue_free( q );
}

/* ********** TEST EMPTY ********** */

TEST( QueueTest, Length )
{
    queue_t *q = queue_init( myfree );
    EXPECT_EQ( queue_length( q ), 0 );
    for ( int i = 0; i < 10; i++ ) {
        mytype_t *ptr_in = (mytype_t *)malloc( sizeof( mytype_t ) );
        *ptr_in          = i;
        queue_push_head( q, ptr_in );
    }
    EXPECT_EQ( queue_length( q ), 10 );
    for ( int i = 0; i < 10; i++ )
        queue_drop_tail( q );
    EXPECT_EQ( queue_length( q ), 0 );
    queue_free( q );
}

/* ********** TEST LENGTH ********** */

TEST( QueueTest, Empty )
{
    queue_t *q = queue_init( myfree );
    for ( int i = 0; i < 10; i++ ) {
        mytype_t *ptr_in = (mytype_t *)malloc( sizeof( mytype_t ) );
        *ptr_in          = i;
        queue_push_head( q, ptr_in );
    }
    EXPECT_FALSE( queue_is_empty( q ) );
    for ( int i = 0; i < 10; i++ )
        queue_drop_tail( q );
    EXPECT_TRUE( queue_is_empty( q ) );
    queue_free( q );
}

/* ********** TEST THREADSAFE ********** */

#define THREADS 8

typedef struct {
    int      threadnumber;
    queue_t *q;
} thrd_context_t;

static thrd_context_t contexts[THREADS];
static thrd_t         threads[THREADS];

static int
thread_func( void *context )
{
    thrd_context_t *ctx = (thrd_context_t *)context;
    for ( int i = 0; i < 100; i++ ) {
        mytype_t *ptr_in = (mytype_t *)malloc( sizeof( mytype_t ) );
        *ptr_in          = i + ctx->threadnumber;
        queue_push_head( ctx->q, ptr_in );
        if ( i % 2 )
            queue_drop_tail( ctx->q );
    }
    return 0;
}

TEST( QueueTest, ThreadSafe )
{
    ::testing::FLAGS_gtest_death_test_style = "threadsafe";
    queue_t *q = queue_init( myfree );
    for ( int i = 0; i < THREADS; i++ ) {
        thrd_context_t *ctx = contexts + i;
        ctx->threadnumber     = i + 1;
        ctx->q                = q;
        thrd_create(&threads[i], &thread_func, ctx);
    }
    for ( int i = 0; i < THREADS; i++ ) {
        thrd_join( threads[i], NULL );
    }
    EXPECT_EQ( queue_length( q ), ( 100 * THREADS ) / 2 );
    queue_free( q );
}

/* ********** MAIN ROUTINE ********** */

int
main( int argc, char **argv )
{
    /* run google tests */
    ::testing::InitGoogleTest( &argc, argv );
    return RUN_ALL_TESTS();
}
