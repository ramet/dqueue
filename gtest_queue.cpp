#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gtest/gtest.h"

extern "C" {
#include "queue.h"
}

/* ********** TEST INIT & FREE ********** */

TEST(QueueTest, Init_Free) {
  queue_t *q = queue_init();
  EXPECT_FALSE(NULL == q);
  queue_free(q);
}

/* ********** TEST PUSH ********** */

TEST(QueueTest, Push) {
  queue_t *q = queue_init();
  for (int i = 0; i < 100; i++) {
    queue_push_head(q, i);
    int head = queue_peek_head(q);
    EXPECT_EQ(head, i);
    int tail = queue_peek_tail(q);
    EXPECT_EQ(tail, 0);
  }
  queue_free(q);
}

/* ********** TEST POP ********** */

TEST(QueueTest, Pop) {
  queue_t *q = queue_init();
  for (int i = 0; i < 100; i++) queue_push_head(q, i);
  for (int i = 0; i < 100; i++) {
    int v = queue_pop_tail(q);
    EXPECT_EQ(v, i);
  }
  queue_free(q);
}

/* ********** TEST EMPTY ********** */

TEST(QueueTest, Length) {
  queue_t *q = queue_init();
  EXPECT_EQ(queue_length(q), 0);
  for (int i = 0; i < 10; i++) queue_push_head(q, i);
  EXPECT_EQ(queue_length(q), 10);
  for (int i = 0; i < 10; i++) queue_pop_tail(q);
  EXPECT_EQ(queue_length(q), 0);
  queue_free(q);
}

/* ********** TEST LENGTH ********** */

TEST(QueueTest, Empty) {
  queue_t *q = queue_init();
  for (int i = 0; i < 10; i++) queue_push_head(q, i);
  EXPECT_FALSE(queue_is_empty(q));
  for (int i = 0; i < 10; i++) queue_pop_tail(q);
  EXPECT_TRUE(queue_is_empty(q));
  queue_free(q);
}

/* ********** MAIN ROUTINE ********** */

int main(int argc, char **argv) {
  /* run google tests */
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
