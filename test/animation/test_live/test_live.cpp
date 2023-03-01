#include "BlenderServoAnimation.h"
#include "../../SerialMock.h"
#include <unity.h>

using namespace BlenderServoAnimation;

#define FPS 60
#define FRAMES 5

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

const int positions[5] PROGMEM = {350, 340, 330, 340, 330};

void test_multiple_servos(void) {
  Animation animation;
  SerialMock mock;
  Servo servos[] = {
      Servo(0, positions, move),
      Servo(1, move),
  };
  animation.addServos(servos, 2);
  animation.live(mock);
  TEST_ASSERT_EQUAL(Animation::MODE_LIVE, animation.getMode());

  byte values[20] = {60, 0, 1, 94, 62, 60, 1, 1, 94, 62,
                     60, 0, 1, 99, 62, 60, 1, 1, 99, 62};

  for (int i = 0; i < 10; i++) {
    mock.write(values[i]);
  }

  animation.run(0);

  TEST_ASSERT_EQUAL(0, lastPositions[0].positions[0]);
  TEST_ASSERT_EQUAL(350, lastPositions[1].positions[0]);

  for (int i = 10; i < 20; i++) {
    mock.write(values[i]);
  }

  animation.run(0);

  TEST_ASSERT_EQUAL(355, lastPositions[0].positions[0]);
  TEST_ASSERT_EQUAL(355, lastPositions[1].positions[1]);
}

void test_skip(void) {
  Animation animation;
  SerialMock mock;
  Servo servo(0, move);
  animation.addServo(servo);
  animation.live(mock);
  TEST_ASSERT_EQUAL(Animation::MODE_LIVE, animation.getMode());

  byte values[15] = {60, 0, 1, 94, 62, 60, 0, 1, 99, 0, 60, 0, 1, 104, 62};

  for (int i = 0; i < 15; i++) {
    mock.write(values[i]);
  }

  animation.run(0);

  TEST_ASSERT_EQUAL(350, lastPositions[0].positions[0]);
  TEST_ASSERT_EQUAL(360, lastPositions[0].positions[1]);
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_multiple_servos);
  RUN_TEST(test_skip);
  UNITY_END();
}
