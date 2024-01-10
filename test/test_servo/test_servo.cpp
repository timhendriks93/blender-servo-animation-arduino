#include "internal/Servo.h"
#include "../test/helper.h"
#include <unity.h>

using namespace BlenderServoAnimation;

void setUp(void) {
  resetPositionLog();
}

void test_move(void) {
  byte servoId = 2;
  Servo servo(servoId, move);

  TEST_ASSERT_TRUE(servo.isNeutral());

  servo.move(340);

  TEST_ASSERT_EQUAL(servoId, positions[0].servoId);
  TEST_ASSERT_EQUAL(340, positions[0].position);

  servo.move(330);

  TEST_ASSERT_EQUAL(servoId, positions[1].servoId);
  TEST_ASSERT_EQUAL(330, positions[1].position);

  servo.move(340);

  TEST_ASSERT_EQUAL(servoId, positions[2].servoId);
  TEST_ASSERT_EQUAL(340, positions[2].position);

  TEST_ASSERT_EQUAL(3, logIndex);
}

void test_move_towards_neutral(void) {
  byte servoId = 123;
  Servo servo(servoId, move, 10);

  servo.move(350);

  TEST_ASSERT_EQUAL(servoId, positions[0].servoId);
  TEST_ASSERT_EQUAL(350, positions[0].position);
  TEST_ASSERT_TRUE(servo.isNeutral());

  servo.move(340);

  TEST_ASSERT_EQUAL(servoId, positions[1].servoId);
  TEST_ASSERT_EQUAL(340, positions[1].position);
  TEST_ASSERT_FALSE(servo.isNeutral());

  for (int i = 0; i < 10; i++) {
    servo.moveTowardsNeutral();

    TEST_ASSERT_EQUAL(servoId, positions[2 + i].servoId);
    TEST_ASSERT_EQUAL(341 + i, positions[2 + i].position);
  }

  TEST_ASSERT_TRUE(servo.isNeutral());
  TEST_ASSERT_EQUAL(12, logIndex);
}

void test_threshold(void) {
  byte servoId = 3;
  Servo servo(servoId, move, 15);

  TEST_ASSERT_TRUE(servo.isNeutral());

  servo.move(340);

  TEST_ASSERT_EQUAL(servoId, positions[0].servoId);
  TEST_ASSERT_EQUAL(340, positions[0].position);

  servo.move(330);

  TEST_ASSERT_EQUAL(servoId, positions[1].servoId);
  TEST_ASSERT_EQUAL(330, positions[1].position);

  servo.move(310);

  TEST_ASSERT_EQUAL(0, positions[2].servoId);
  TEST_ASSERT_EQUAL(0, positions[2].position);

  TEST_ASSERT_EQUAL(2, logIndex);
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_move);
  RUN_TEST(test_move_towards_neutral);
  RUN_TEST(test_threshold);
  UNITY_END();
}