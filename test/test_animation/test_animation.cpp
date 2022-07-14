#include "../SerialMock.h"
#include "BlenderServoAnimation.h"
#include <unity.h>

using namespace BlenderServoAnimation;

#define FPS 60
#define FRAME_MILLIS 1000 / FPS

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

void test_play(void) {
  Animation animation(FPS, 5);
  Servo servos[] = {
      Servo(1, positionsA, move),
      Servo(2, positionsB, move),
  };
  animation.addServos(servos, 2);
  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, animation.getMode());
  animation.play();
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY, animation.getMode());

  int expA[9] = {340, 330, 340, 330, 350, 340, 330, 340, 330};
  int expB[9] = {240, 230, 240, 230, 250, 240, 230, 240, 230};

  for (int i = 0; i < 9; i++) {
    animation.run(FRAME_MILLIS * (i + 1));
    TEST_ASSERT_EQUAL(expA[i], lastPositions[1].positions[i]);
    TEST_ASSERT_EQUAL(expB[i], lastPositions[2].positions[i]);
    animation.run(FRAME_MILLIS * (i + 1.5));
    TEST_ASSERT_EQUAL(0, lastPositions[1].positions[i + 1]);
    TEST_ASSERT_EQUAL(0, lastPositions[2].positions[i + 1]);
  }
}

void test_pause(void) {
  Animation animation(FPS, 5);
  Servo servo(2, positionsA, move);
  animation.addServo(servo);
  animation.play();
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY, animation.getMode());

  int exp[9] = {340, 330, 340, 330, 350, 340, 330, 340, 330};

  for (int i = 0; i < 4; i++) {
    animation.run(FRAME_MILLIS * (i + 1));
    TEST_ASSERT_EQUAL(exp[i], lastPositions[2].positions[i]);
  }

  animation.pause();
  TEST_ASSERT_EQUAL(Animation::MODE_PAUSE, animation.getMode());

  for (int i = 4; i < 8; i++) {
    animation.run(FRAME_MILLIS * (i + 1));
    TEST_ASSERT_EQUAL(0, lastPositions[2].positions[i]);
  }

  animation.play();
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY, animation.getMode());

  for (int i = 8; i < 14; i++) {
    animation.run(FRAME_MILLIS * (i + 1));
    TEST_ASSERT_EQUAL(exp[i - 5], lastPositions[2].positions[i - 5]);
  }
}

void test_stop(void) {
  Animation animation(FPS, 5);
  Servo servo = Servo(3, positionsA, move);
  animation.addServo(servo);
  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, animation.getMode());
  animation.play();
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY, animation.getMode());
  animation.run(FRAME_MILLIS);
  TEST_ASSERT_EQUAL(340, lastPositions[3].positions[0]);
  TEST_ASSERT_EQUAL(1, animation.getFrame());
  animation.stop(0);

  for (int i = 0; i < 10; i++) {
    animation.run(FRAME_MILLIS * (i + 2));
    TEST_ASSERT_EQUAL(341 + i, lastPositions[3].positions[i + 1]);
    TEST_ASSERT_EQUAL(Animation::MODE_STOP, animation.getMode());
  }

  animation.run(FRAME_MILLIS * 12);
  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, animation.getMode());
  TEST_ASSERT_EQUAL(0, animation.getFrame());
}

void test_live(void) {
  Animation animation(FPS, 5);
  SerialMock mock;
  Servo servo = Servo(3, positionsA, move, 50);
  animation.addServo(servo);
  animation.live(mock);
  TEST_ASSERT_EQUAL(Animation::MODE_LIVE, animation.getMode());

  byte values[10] = {60, 3, 1, 119, 62, 60, 3, 1, 144, 62};

  for (int i = 0; i < 5; i++) {
    mock.write(values[i]);
  }

  animation.run();
  TEST_ASSERT_EQUAL(375, lastPositions[3].positions[0]);

  for (int i = 5; i < 10; i++) {
    mock.write(values[i]);
  }

  animation.run();
  TEST_ASSERT_EQUAL(400, lastPositions[3].positions[1]);
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_play);
  RUN_TEST(test_pause);
  RUN_TEST(test_stop);
  RUN_TEST(test_live);
  UNITY_END();
}
