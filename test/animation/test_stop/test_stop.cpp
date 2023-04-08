#include "../helper.h"
#include "BlenderServoAnimation.h"
#include <unity.h>

using namespace BlenderServoAnimation;

void setUp(void) {
  setUpHelper();
}

const int positions[5] PROGMEM = {350, 340, 330, 340, 330};
const int positionsB[5] PROGMEM = {250, 240, 230, 240, 230};

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

void test_call_twice(void) {
  Animation animation(FPS, FRAMES);
  animation.onModeChange(onModeChange);

  animation.play(0);
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY, animation.getMode());
  animation.stop();
  TEST_ASSERT_EQUAL(Animation::MODE_STOP, animation.getMode());
  animation.stop();
  TEST_ASSERT_EQUAL(Animation::MODE_STOP, animation.getMode());

  TEST_ASSERT_EQUAL(2, modeChangeCount);
}

void test_prevented(void) {
  Animation animation(FPS, FRAMES);

  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, animation.getMode());
  animation.stop();
  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, animation.getMode());
}

void test_allowed(void) {
  Serial_ mock;
  Animation animation(FPS, FRAMES);

  animation.play(0);
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY, animation.getMode());
  animation.stop();
  TEST_ASSERT_EQUAL(Animation::MODE_STOP, animation.getMode());
  animation.run(0);
  animation.loop(0);
  TEST_ASSERT_EQUAL(Animation::MODE_LOOP, animation.getMode());
  animation.stop();
  TEST_ASSERT_EQUAL(Animation::MODE_STOP, animation.getMode());
  animation.run(0);
  animation.play(0);
  animation.pause();
  TEST_ASSERT_EQUAL(Animation::MODE_PAUSE, animation.getMode());
  animation.stop();
  TEST_ASSERT_EQUAL(Animation::MODE_STOP, animation.getMode());
  animation.run(0);
  animation.live(mock);
  TEST_ASSERT_EQUAL(Animation::MODE_LIVE, animation.getMode());
  animation.stop();
  TEST_ASSERT_EQUAL(Animation::MODE_STOP, animation.getMode());
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_stop);
  RUN_TEST(test_call_twice);
  RUN_TEST(test_prevented);
  RUN_TEST(test_allowed);
  UNITY_END();
}
