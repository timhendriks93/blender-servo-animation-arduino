#include "../helper.h"
#include "BlenderServoAnimation.h"
#include <unity.h>

using namespace BlenderServoAnimation;

void setUp(void) {
  setUpHelper();
}

const int positions[5] PROGMEM = {350, 340, 330, 340, 330};
const int positionsB[5] PROGMEM = {250, 240, 230, 240, 230};

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

void test_pause_play(void) {
  test_pause(false);
}

void test_pause_loop(void) {
  test_pause(true);
}

void test_call_twice(void) {
  Animation animation(FPS, FRAMES);
  animation.onModeChange(onModeChange);

  animation.play(0);
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY, animation.getMode());
  animation.pause();
  TEST_ASSERT_EQUAL(Animation::MODE_PAUSE, animation.getMode());
  animation.pause();
  TEST_ASSERT_EQUAL(Animation::MODE_PAUSE, animation.getMode());

  TEST_ASSERT_EQUAL(2, modeChangeCount);
}

void test_prevented(void) {
  Serial_ mock;
  Animation animation(FPS, FRAMES);

  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, animation.getMode());
  animation.pause();
  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, animation.getMode());
  animation.play(0);
  animation.stop(0);
  TEST_ASSERT_EQUAL(Animation::MODE_STOP, animation.getMode());
  animation.pause();
  TEST_ASSERT_EQUAL(Animation::MODE_STOP, animation.getMode());
  animation.run(10000);
  animation.live(mock);
  TEST_ASSERT_EQUAL(Animation::MODE_LIVE, animation.getMode());
  animation.pause();
  TEST_ASSERT_EQUAL(Animation::MODE_LIVE, animation.getMode());
}

void test_allowed(void) {
  Animation animation(FPS, FRAMES);

  animation.play(0);
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY, animation.getMode());
  animation.pause();
  TEST_ASSERT_EQUAL(Animation::MODE_PAUSE, animation.getMode());
  animation.loop(0);
  TEST_ASSERT_EQUAL(Animation::MODE_LOOP, animation.getMode());
  animation.pause();
  TEST_ASSERT_EQUAL(Animation::MODE_PAUSE, animation.getMode());
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_pause_play);
  RUN_TEST(test_pause_loop);
  RUN_TEST(test_call_twice);
  RUN_TEST(test_prevented);
  RUN_TEST(test_allowed);
  UNITY_END();
}
