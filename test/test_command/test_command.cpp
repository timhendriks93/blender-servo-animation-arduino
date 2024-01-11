#include "internal/Command.h"
#include <unity.h>

using namespace BlenderServoAnimation;

void test_valid(void) {
  byte values[5] = {60, 3, 1, 119, 62};
  Command command;

  for (int i = 0; i < 5; i++) {
    command.write(values[i]);

    if (i == 4) {
      TEST_ASSERT_TRUE(command.isValid());
      TEST_ASSERT_EQUAL(3, command.getServoID());
      TEST_ASSERT_EQUAL(375, command.getServoPosition());
    } else {
      TEST_ASSERT_FALSE(command.isValid());
    }
  }
}

void test_invalid(void) {
  byte values[5] = {60, 3, 1, 119, 0};
  Command command;

  for (int i = 0; i < 5; i++) {
    command.write(values[i]);

    TEST_ASSERT_FALSE(command.isValid());
  }
}

void test_incomplete(void) {
  byte values[13] = {60, 3, 1, 60, 3, 1, 119, 62, 60, 3, 1, 144, 62};
  Command command;

  for (int i = 0; i < 13; i++) {
    command.write(values[i]);

    if (i == 7 || i == 12) {
      TEST_ASSERT_TRUE(command.isValid());
    } else {
      TEST_ASSERT_FALSE(command.isValid());
    }
  }
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_valid);
  RUN_TEST(test_invalid);
  RUN_TEST(test_incomplete);
  UNITY_END();
}