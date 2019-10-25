#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "gtest/gtest.h"

extern "C" {
#include "queue.h"
}

#define EXPECT_CRASH(statement) EXPECT_EXIT((statement,exit(0)),::testing::KilledBySignal(SIGSEGV),".*")
#define EXPECT_NO_CRASH(statement) EXPECT_EXIT((statement,exit(0)),::testing::ExitedWithCode(0),".*")

#define MYTYPE int
void myfree(gpointer p) { free(p); }

class QueueTest : public ::testing::Test {
 protected:
  void SetUp() override { q = queue_init(myfree); } // queue_init(NULL);
  void TearDown() override { EXPECT_NO_CRASH(queue_free(q)); }
  queue_t* q;
};

/* ********** TEST INIT & FREE ********** */

TEST_F(QueueTest, Init_Free) { EXPECT_FALSE(NULL == q); }

/* ********** TEST PUSH ********** */

TEST_F(QueueTest, Push) {
  for (int i = 0; i < 100; i++) {
    MYTYPE *ptr_in = (MYTYPE *) malloc(sizeof(MYTYPE));
    *ptr_in = i;
    queue_push_head(q, ptr_in);
    MYTYPE* ptr_head = (MYTYPE*) queue_peek_head(q);
    EXPECT_EQ(*ptr_head, (MYTYPE)i);
    MYTYPE* ptr_tail = (MYTYPE*) queue_peek_tail(q);
    EXPECT_EQ(*ptr_tail, (MYTYPE)0);
  }
}

/* ********** TEST POP ********** */

TEST_F(QueueTest, Pop) {
  for (int i = 0; i < 100; i++) {
    MYTYPE *ptr_in = (MYTYPE *) malloc(sizeof(MYTYPE));
    *ptr_in = i;
    queue_push_head(q, ptr_in);
  }
  for (int i = 0; i < 100; i++) {
    MYTYPE* ptr_out = (MYTYPE*) queue_pop_tail(q);
    EXPECT_EQ(*ptr_out, (MYTYPE)i);
    myfree(ptr_out);
  }
}

/* ********** TEST DROP ********** */

TEST_F(QueueTest, Drop) {
  for (int i = 0; i < 100; i++) {
    MYTYPE *ptr_in = (MYTYPE *) malloc(sizeof(MYTYPE));
    *ptr_in = i;
    queue_push_head(q, ptr_in);
  }
  for (int i = 0; i < 100; i++) queue_drop_tail(q);
  EXPECT_EQ(queue_length(q), 0);
}


/* ********** TEST EMPTY ********** */

TEST_F(QueueTest, Length) {
  EXPECT_EQ(queue_length(q), 0);
  for (int i = 0; i < 10; i++) {
    MYTYPE *ptr_in = (MYTYPE *) malloc(sizeof(MYTYPE));
    *ptr_in = i;
    queue_push_head(q, ptr_in);
  }
  EXPECT_EQ(queue_length(q), 10);
  for (int i = 0; i < 10; i++) myfree(queue_pop_tail(q));
  EXPECT_EQ(queue_length(q), 0);
}

/* ********** TEST LENGTH ********** */

TEST_F(QueueTest, Empty) {
  for (int i = 0; i < 10; i++) {
    MYTYPE *ptr_in = (MYTYPE *) malloc(sizeof(MYTYPE));
    *ptr_in = i;
    queue_push_head(q, ptr_in);
  }
  EXPECT_FALSE(queue_is_empty(q));
  for (int i = 0; i < 10; i++) myfree(queue_pop_tail(q));
  EXPECT_TRUE(queue_is_empty(q));
}

/* ********** TEST THREADSAFE ********** */

#define THREADS 8

typedef struct {
    int threadnumber;
    queue_t *q;
} thread_context_t;

static thread_context_t contexts[THREADS];
static pthread_t threads[THREADS];

static void* thread_func(void* context)
{
  thread_context_t* ctx = (thread_context_t*) context;
  for (int i = 0; i < 100; i++) {
    MYTYPE *ptr_in = (MYTYPE *) malloc(sizeof(MYTYPE));
    queue_push_head(ctx->q, ptr_in);
    if (!queue_is_empty(ctx->q)) queue_drop_tail(ctx->q);
  }
  return NULL;
}

TEST_F(QueueTest, ThreadSafe) {
  for (int i = 0; i < THREADS; i++) {
    thread_context_t* ctx = contexts + i;
    ctx->threadnumber = i + 1;
    ctx->q = q;
    pthread_create(&threads[i], NULL, &thread_func, ctx);
  }
  for(int i = 0; i < THREADS; i++) {
    pthread_join(threads[i], NULL);
  }
}

/* ********** MAIN ROUTINE ********** */

int main(int argc, char **argv) {
  /* run google tests */
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
