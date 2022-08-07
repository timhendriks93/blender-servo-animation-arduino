#include "../SerialMock.h"
#include "BlenderServoAnimation.h"
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

const int positionsA[5] PROGMEM = {350, 340, 330, 340, 330};
const int positionsB[5] PROGMEM = {250, 240, 230, 240, 230};

void test_live(void) {
  Animation animation;
  SerialMock mock;
  Servo servos[] = {
      Servo(0, positionsA, move),
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

  animation.run();

  TEST_ASSERT_EQUAL(0, lastPositions[0].positions[0]);
  TEST_ASSERT_EQUAL(350, lastPositions[1].positions[0]);

  for (int i = 10; i < 20; i++) {
    mock.write(values[i]);
  }

  animation.run();

  TEST_ASSERT_EQUAL(355, lastPositions[0].positions[0]);
  TEST_ASSERT_EQUAL(355, lastPositions[1].positions[1]);
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_live);
  UNITY_END();
}
