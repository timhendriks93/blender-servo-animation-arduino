#include "../test/helper.h"
#include "AnimationData.h"
#include "ServoManager.h"
#include <unity.h>

using namespace BlenderServoAnimationLibrary;

void setUp(void) {
  resetPositionLog();
}

void test_parse_data(void) {
  AnimationData data(PROGMEM_DATA, DATA_SIZE);
  ServoManager servoManager;
  servoManager.setPositionCallback(move);

  TEST_ASSERT_EQUAL(0, logIndex);

  servoManager.parseData(&data);
  servoManager.parseData(&data);

  TEST_ASSERT_EQUAL(4, logIndex);
  TEST_ASSERT_EQUAL(0, positions[0].servoId);
  TEST_ASSERT_EQUAL(375, positions[0].position);
  TEST_ASSERT_EQUAL(1, positions[1].servoId);
  TEST_ASSERT_EQUAL(375, positions[1].position);
  TEST_ASSERT_EQUAL(0, positions[2].servoId);
  TEST_ASSERT_EQUAL(376, positions[2].position);
  TEST_ASSERT_EQUAL(1, positions[3].servoId);
  TEST_ASSERT_EQUAL(376, positions[3].position);
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

void test_servo_id_10_is_not_mixed_up_with_line_break(void) {
  byte values[11] = {60, 10, 1, 119, 62, 10, 60, 11, 1, 119, 62};
  AnimationData data(values, 11);
  ServoManager servoManager;
  servoManager.setPositionCallback(move);

  TEST_ASSERT_EQUAL(0, logIndex);

  servoManager.parseData(&data);
  servoManager.parseData(&data);

  TEST_ASSERT_EQUAL(2, logIndex);
  TEST_ASSERT_EQUAL(10, positions[0].servoId);
  TEST_ASSERT_EQUAL(375, positions[0].position);
  TEST_ASSERT_EQUAL(11, positions[1].servoId);
  TEST_ASSERT_EQUAL(375, positions[1].position);
}

void test_start_and_end_characters_not_mixed_up_with_ids_and_position_values(void) {
  byte values[11] = {60, 60, 0, 60, 62, 10, 60, 62, 0, 62, 62};
  AnimationData data(values, 11);
  ServoManager servoManager;
  servoManager.setPositionCallback(move);

  TEST_ASSERT_EQUAL(0, logIndex);

  servoManager.parseData(&data);
  servoManager.parseData(&data);

  TEST_ASSERT_EQUAL(2, logIndex);
  TEST_ASSERT_EQUAL(60, positions[0].servoId);
  TEST_ASSERT_EQUAL(60, positions[0].position);
  TEST_ASSERT_EQUAL(62, positions[1].servoId);
  TEST_ASSERT_EQUAL(62, positions[1].position);
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_parse_data);
  RUN_TEST(test_parse_data_without_line_breaks);
  RUN_TEST(test_move_all_towards_neutral);
  RUN_TEST(test_servo_id_10_is_not_mixed_up_with_line_break);
  RUN_TEST(test_start_and_end_characters_not_mixed_up_with_ids_and_position_values);
  UNITY_END();
}