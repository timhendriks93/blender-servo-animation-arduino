#include "../test/helper.h"
#include "internal/ProgmemStream.h"
#include "internal/ServoManager.h"
#include <unity.h>

using namespace BlenderServoAnimation;

void setUp(void) {
  resetPositionLog();
}

void test_parse_stream(void) {
  ProgmemStream stream(PROGMEM_DATA, DATA_SIZE);
  ServoManager servoManager;
  servoManager.setPositionCallback(move);

  TEST_ASSERT_EQUAL(0, logIndex);

  servoManager.parseStream(&stream);

  TEST_ASSERT_EQUAL(2, logIndex);
  TEST_ASSERT_EQUAL(0, positions[0].servoId);
  TEST_ASSERT_EQUAL(375, positions[0].position);
  TEST_ASSERT_EQUAL(1, positions[1].servoId);
  TEST_ASSERT_EQUAL(375, positions[1].position);
}

void test_parse_stream_without_line_breaks(void) {
  ProgmemStream stream(PROGMEM_DATA, DATA_SIZE);
  ServoManager servoManager;
  servoManager.setPositionCallback(move);

  TEST_ASSERT_EQUAL(0, logIndex);

  servoManager.parseStream(&stream, false);

  TEST_ASSERT_EQUAL(10, logIndex);
}

void test_move_all_towards_neutral(void) {
  ProgmemStream stream(PROGMEM_DATA, DATA_SIZE);
  ServoManager servoManager;
  servoManager.setPositionCallback(move);
  servoManager.setDefaultThreshold(20);
  servoManager.setThreshold(0, 10);

  TEST_ASSERT_EQUAL(0, logIndex);

  servoManager.parseStream(&stream);
  servoManager.parseStream(&stream);
  servoManager.parseStream(&stream);

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
  RUN_TEST(test_parse_stream);
  RUN_TEST(test_parse_stream_without_line_breaks);
  RUN_TEST(test_move_all_towards_neutral);
  UNITY_END();
}