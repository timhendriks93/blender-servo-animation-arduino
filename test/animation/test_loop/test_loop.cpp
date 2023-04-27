#include "../helper.h"
#include "BlenderServoAnimation.h"
#include <unity.h>

using namespace BlenderServoAnimation;

void setUp(void) {
  setUpHelper();
}

const int positionsA[5] PROGMEM = {350, 340, 330, 340, 330};
const int positionsB[5] PROGMEM = {250, 240, 230, 240, 230};

void test_loop(void) {
  Animation animation(FPS, FRAMES);
  Servo servos[] = {
      Servo(1, positionsA, move),
      Servo(2, positionsB, move),
      Servo(3, move),
  };
  animation.addServos(servos, 3);
  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, animation.getMode());
  animation.loop(0);
  TEST_ASSERT_EQUAL(Animation::MODE_LOOP, animation.getMode());

  int expA[9] = {340, 330, 340, 330, 350, 340, 330, 340, 330};
  int expB[9] = {240, 230, 240, 230, 250, 240, 230, 240, 230};

  for (long i = 0; i < FRAME_MICROS * (long)10; i += FRAME_MICROS) {
    animation.run(i);
  }

  for (int i = 0; i < 9; i++) {
    TEST_ASSERT_EQUAL(expA[i], lastPositions[1].positions[i]);
    TEST_ASSERT_EQUAL(expB[i], lastPositions[2].positions[i]);
    TEST_ASSERT_EQUAL(0, lastPositions[3].positions[i]);
  }

  TEST_ASSERT_EQUAL(Animation::MODE_LOOP, animation.getMode());
}

void test_call_twice(void) {
  Animation animation(FPS, FRAMES);
  animation.onModeChange(onModeChange);

  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, animation.getMode());
  animation.loop(0);
  TEST_ASSERT_EQUAL(Animation::MODE_LOOP, animation.getMode());
  animation.loop(0);
  TEST_ASSERT_EQUAL(Animation::MODE_LOOP, animation.getMode());

  TEST_ASSERT_EQUAL(1, modeChangeCount);
}

void test_prevented(void) {
  Serial_ mock;
  Animation animation(FPS, FRAMES);

  animation.play(0);
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY, animation.getMode());
  animation.loop(0);
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY, animation.getMode());
  animation.stop(0);
  TEST_ASSERT_EQUAL(Animation::MODE_STOP, animation.getMode());
  animation.loop(0);
  TEST_ASSERT_EQUAL(Animation::MODE_STOP, animation.getMode());
  animation.run(10000);
  animation.live(mock);
  TEST_ASSERT_EQUAL(Animation::MODE_LIVE, animation.getMode());
  animation.loop(0);
  TEST_ASSERT_EQUAL(Animation::MODE_LIVE, animation.getMode());
}

void test_allowed(void) {
  Animation animation(FPS, FRAMES);

  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, animation.getMode());
  animation.loop(0);
  TEST_ASSERT_EQUAL(Animation::MODE_LOOP, animation.getMode());
  animation.pause();
  TEST_ASSERT_EQUAL(Animation::MODE_PAUSE, animation.getMode());
  animation.loop(0);
  TEST_ASSERT_EQUAL(Animation::MODE_LOOP, animation.getMode());
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_loop);
  RUN_TEST(test_call_twice);
  RUN_TEST(test_prevented);
  RUN_TEST(test_allowed);
  UNITY_END();
}
