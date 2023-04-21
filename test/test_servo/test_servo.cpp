#include "Servo.h"
#include <unity.h>

using namespace BlenderServoAnimation;

struct positionLog {
  int index;
  int positions[20];
};

positionLog lastPositions[16];

void setUp(void) {
  for (int id = 0; id < 16; id++) {
    lastPositions[id].index = 0;

    for (int i = 0; i < 20; i++) {
      lastPositions[id].positions[i] = 0;
    }
  }
}

void move(byte servoID, int position) {
  int index = lastPositions[servoID].index;
  lastPositions[servoID].positions[index] = position;
  lastPositions[servoID].index++;
}

const int PROGMEM positions[5] = {350, 340, 330, 340, 350};

void test_move_by_frame(void) {
  Servo servo(2, positions, move);
  TEST_ASSERT_EQUAL(2, servo.getID());
  TEST_ASSERT_TRUE(servo.isNeutral());
  servo.moveByFrame(1);
  TEST_ASSERT_EQUAL(340, lastPositions[2].positions[0]);
  servo.moveByFrame(2);
  TEST_ASSERT_EQUAL(330, lastPositions[2].positions[1]);
  servo.moveByFrame(3);
  TEST_ASSERT_EQUAL(340, lastPositions[2].positions[2]);
}

void test_move_towards_neutral(void) {
  Servo servo(0, positions, move);
  TEST_ASSERT_TRUE(servo.isNeutral());
  servo.moveByFrame(1);
  TEST_ASSERT_EQUAL(340, lastPositions[0].positions[0]);
  TEST_ASSERT_FALSE(servo.isNeutral());

  for (int i = 0; i < 10; i++) {
    servo.moveTowardsNeutral();
    TEST_ASSERT_EQUAL(341 + i, lastPositions[0].positions[1 + i]);
  }

  TEST_ASSERT_TRUE(servo.isNeutral());
}

void test_threshold(void) {
  Servo servo(0, positions, move, 15);
  TEST_ASSERT_TRUE(servo.isNeutral());
  servo.moveByFrame(1);
  TEST_ASSERT_EQUAL(340, lastPositions[0].positions[0]);
  servo.moveByFrame(2);
  TEST_ASSERT_EQUAL(330, lastPositions[0].positions[1]);
  servo.moveByFrame(4);
  TEST_ASSERT_EQUAL(0, lastPositions[0].positions[2]);
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_move_by_frame);
  RUN_TEST(test_move_towards_neutral);
  RUN_TEST(test_threshold);
  UNITY_END();
}