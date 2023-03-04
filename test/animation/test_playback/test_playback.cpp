#include "BlenderServoAnimation.h"
#include <unity.h>

using namespace BlenderServoAnimation;

#define FPS 60
#define FRAMES 5
#define FRAME_MICROS 16667

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
const int positionsB[5] PROGMEM = {250, 240, 230, 240, 230};

void test_play(void) {
  Animation animation(FPS, FRAMES);
  Servo servos[] = {
      Servo(1, positions, move),
      Servo(2, positionsB, move),
      Servo(3, move),
  };
  animation.addServos(servos, 3);
  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, animation.getMode());
  animation.play(0);
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY, animation.getMode());

  int expA[5] = {340, 330, 340, 330, 350};
  int expB[5] = {240, 230, 240, 230, 250};

  for (long i = 0; i < FRAME_MICROS * (long)9; i++) {
    animation.run(i);
  }

  for (int i = 0; i < 5; i++) {
    TEST_ASSERT_EQUAL(expA[i], lastPositions[1].positions[i]);
    TEST_ASSERT_EQUAL(expB[i], lastPositions[2].positions[i]);
    TEST_ASSERT_EQUAL(0, lastPositions[3].positions[i]);
  }

  for (int i = 5; i < 9; i++) {
    TEST_ASSERT_EQUAL(0, lastPositions[1].positions[i]);
    TEST_ASSERT_EQUAL(0, lastPositions[2].positions[i]);
    TEST_ASSERT_EQUAL(0, lastPositions[3].positions[i]);
  }

  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, animation.getMode());
}

void test_pause(bool loop) {
  Animation animation(FPS, FRAMES);
  Servo servo(2, positions, move);
  animation.addServo(servo);

  if (loop) {
    animation.loop(0);
    TEST_ASSERT_EQUAL(Animation::MODE_LOOP, animation.getMode());
  } else {
    animation.play(0);
    TEST_ASSERT_EQUAL(Animation::MODE_PLAY, animation.getMode());
  }

  int exp[5] = {340, 330, 340, 330, 350};

  for (int i = 0; i < 2; i++) {
    animation.run(FRAME_MICROS * (long)(i + 1));
    TEST_ASSERT_EQUAL(exp[i], lastPositions[2].positions[i]);
  }

  animation.pause();
  TEST_ASSERT_EQUAL(Animation::MODE_PAUSE, animation.getMode());

  for (int i = 2; i < 5; i++) {
    animation.run(FRAME_MICROS * (long)(i + 1));
    TEST_ASSERT_EQUAL(0, lastPositions[2].positions[i]);
  }

  if (loop) {
    animation.loop(0);
    TEST_ASSERT_EQUAL(Animation::MODE_LOOP, animation.getMode());
  } else {
    animation.play(0);
    TEST_ASSERT_EQUAL(Animation::MODE_PLAY, animation.getMode());
  }

  for (int i = 5; i < 8; i++) {
    animation.run(FRAME_MICROS * (long)(i + 1));
    TEST_ASSERT_EQUAL(exp[i - 3], lastPositions[2].positions[i - 3]);
  }
}

void test_stop(void) {
  Animation animation(FPS, FRAMES);
  Servo servos[] = {
      Servo(0, positions, move),
      Servo(1, positionsB, move),
      Servo(2, move),
  };
  animation.addServos(servos, 3);
  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, animation.getMode());
  animation.play(0);
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY, animation.getMode());
  animation.run(FRAME_MICROS);
  TEST_ASSERT_EQUAL(340, lastPositions[0].positions[0]);
  TEST_ASSERT_EQUAL(240, lastPositions[1].positions[0]);
  TEST_ASSERT_EQUAL(0, lastPositions[2].positions[0]);
  TEST_ASSERT_EQUAL(1, animation.getFrame());
  animation.stop(0);

  for (int i = 0; i < 10; i++) {
    animation.run(FRAME_MICROS * (long)(i + 2));
    TEST_ASSERT_EQUAL(341 + i, lastPositions[0].positions[i + 1]);
    TEST_ASSERT_EQUAL(241 + i, lastPositions[1].positions[i + 1]);
    TEST_ASSERT_EQUAL(0, lastPositions[2].positions[i + 1]);
    TEST_ASSERT_EQUAL(Animation::MODE_STOP, animation.getMode());
  }

  animation.run(FRAME_MICROS * (long)12);
  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, animation.getMode());
  TEST_ASSERT_EQUAL(0, animation.getFrame());
}

void test_loop(void) {
  Animation animation(FPS, FRAMES);
  Servo servos[] = {
      Servo(1, positions, move),
      Servo(2, positionsB, move),
      Servo(3, move),
  };
  animation.addServos(servos, 3);
  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, animation.getMode());
  animation.loop(0);
  TEST_ASSERT_EQUAL(Animation::MODE_LOOP, animation.getMode());

  int expA[9] = {340, 330, 340, 330, 350, 340, 330, 340, 330};
  int expB[9] = {240, 230, 240, 230, 250, 240, 230, 240, 230};

  for (long i = 0; i < FRAME_MICROS * (long)9; i++) {
    animation.run(i);
  }

  for (int i = 0; i < 9; i++) {
    TEST_ASSERT_EQUAL(expA[i], lastPositions[1].positions[i]);
    TEST_ASSERT_EQUAL(expB[i], lastPositions[2].positions[i]);
    TEST_ASSERT_EQUAL(0, lastPositions[3].positions[i]);
  }

  TEST_ASSERT_EQUAL(Animation::MODE_LOOP, animation.getMode());
}

void test_pause_play(void) {
  test_pause(false);
}

void test_pause_loop(void) {
  test_pause(true);
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_play);
  RUN_TEST(test_pause_play);
  RUN_TEST(test_stop);
  RUN_TEST(test_loop);
  RUN_TEST(test_pause_loop);
  UNITY_END();
}
