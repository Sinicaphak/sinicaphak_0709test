#include <gtest/gtest.h>
#include "main.h"

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
  log_info("test");

  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
}