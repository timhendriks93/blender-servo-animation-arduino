#include "../test/helper.h"
#include "internal/AnimationData.h"
#include "internal/ServoManager.h"
#include <unity.h>

using namespace BlenderServoAnimation;

void setUp(void) {
  resetPositionLog();
}

void test_parse_data(void) {
  AnimationData data(PROGMEM_DATA, DATA_SIZE);
  ServoManager servoManager;
  servoManager.setPositionCallback(move);

  TEST_ASSERT_EQUAL(0, logIndex);

  servoManager.parseData(&data);

  TEST_ASSERT_EQUAL(2, logIndex);
  TEST_ASSERT_EQUAL(0, positions[0].servoId);
  TEST_ASSERT_EQUAL(375, positions[0].position);
  TEST_ASSERT_EQUAL(1, positions[1].servoId);
  TEST_ASSERT_EQUAL(375, positions[1].position);
}

void test_parse_data_without_line_breaks(void) {
  AnimationData data(PROGMEM_DATA, DATA_SIZE);
  ServoManager servoManager;
  servoManager.setPositionCallback(move);

  TEST_ASSERT_EQUAL(0, logIndex);

  servoManager.parseData(&data, false);

  TEST_ASSERT_EQUAL(10, logIndex);
}

void test_move_all_towards_neutral(void) {
  AnimationData data(PROGMEM_DATA, DATA_SIZE);
  ServoManager servoManager;
  servoManager.setPositionCallback(move);
  servoManager.setDefaultThreshold(20);
  servoManager.setThreshold(0, 10);

  TEST_ASSERT_EQUAL(0, logIndex);

  servoManager.parseData(&data);
  servoManager.parseData(&data);
  servoManager.parseData(&data);

  TEST_ASSERT_EQUAL(6, logIndex);
  TEST_ASSERT_FALSE(servoManager.servosAreAllNeutral());

  servoManager.moveAllTowardsNeutral();

  TEST_ASSERT_EQUAL(8, logIndex);
  TEST_ASSERT_FALSE(servoManager.servosAreAllNeutral());

  servoManager.moveAllTowardsNeutral();

  TEST_ASSERT_EQUAL(10, logIndex);
  TEST_ASSERT_TRUE(servoManager.servosAreAllNeutral());
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_parse_data);
  RUN_TEST(test_parse_data_without_line_breaks);
  RUN_TEST(test_move_all_towards_neutral);
  UNITY_END();
}