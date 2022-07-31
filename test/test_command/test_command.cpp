#include "../SerialMock.h"
#include "command/Command.h"
#include <unity.h>

using namespace BlenderServoAnimation;

void test_valid(void) {
  byte values[5] = {60, 3, 1, 119, 62};
  SerialMock mock;
  Command command = Command();

  for (int i = 0; i < 5; i++) {
    mock.write(values[i]);
    command.read(&mock);

    if (i == 4) {
      TEST_ASSERT_TRUE(command.isComplete());
      TEST_ASSERT_TRUE(command.isValid());
      TEST_ASSERT_EQUAL(3, command.getServoID());
      TEST_ASSERT_EQUAL(375, command.getServoPosition());
    } else {
      TEST_ASSERT_FALSE(command.isComplete());
      TEST_ASSERT_FALSE(command.isValid());
    }
  }
}

void test_invalid(void) {
  byte values[5] = {60, 3, 1, 119, 0};
  SerialMock mock;
  Command command = Command();

  for (int i = 0; i < 5; i++) {
    mock.write(values[i]);
    command.read(&mock);

    if (i == 4) {
      TEST_ASSERT_TRUE(command.isComplete());
    } else {
      TEST_ASSERT_FALSE(command.isComplete());
    }

    TEST_ASSERT_FALSE(command.isValid());
  }
}

void test_incomplete(void) {
  byte values[13] = {60, 3, 1, 60, 3, 1, 119, 62, 60, 3, 1, 144, 62};
  SerialMock mock;
  Command command = Command();

  for (int i = 0; i < 13; i++) {
    mock.write(values[i]);
    command.read(&mock);

    if (i == 12) {
      TEST_ASSERT_TRUE(command.isComplete());
      TEST_ASSERT_TRUE(command.isValid());
      TEST_ASSERT_EQUAL(3, command.getServoID());
      TEST_ASSERT_EQUAL(400, command.getServoPosition());
    } else if (i >= 4 && i < 8) {
      TEST_ASSERT_TRUE(command.isComplete());
      TEST_ASSERT_FALSE(command.isValid());
    } else {
      TEST_ASSERT_FALSE(command.isComplete());
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