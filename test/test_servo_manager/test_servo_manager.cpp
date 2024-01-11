#include "../test/helper.h"
#include "internal/ServoManager.h"
#include <unity.h>

using namespace BlenderServoAnimation;

void setUp(void) {
  resetPositionLog();
}

void test_handle_command(void) {
  Command command;
  ServoManager servoManager;
  servoManager.setPositionCallback(move);

  for (byte i = 0; i < 4; i++) {
    command.write(PROGMEM_DATA[i]);
  }

  servoManager.handleCommand(command);

  TEST_ASSERT_EQUAL(0, logIndex);

  command.write(PROGMEM_DATA[4]);
  servoManager.handleCommand(command);

  TEST_ASSERT_EQUAL(1, logIndex);
  TEST_ASSERT_EQUAL(0, positions[0].servoId);
  TEST_ASSERT_EQUAL(375, positions[0].position);
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_handle_command);
  UNITY_END();
}