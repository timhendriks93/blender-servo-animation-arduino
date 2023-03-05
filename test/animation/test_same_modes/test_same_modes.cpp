#include "BlenderServoAnimation.h"
#include <unity.h>

using namespace BlenderServoAnimation;

#define FPS 60
#define FRAMES 5

byte modeChangeCount = 0;

void setUp(void) {
  modeChangeCount = 0;
}

void onModeChange(byte prevMode, byte newMode) {
  modeChangeCount++;
}

void test_play(void) {
  Animation animation(FPS, FRAMES);
  animation.onModeChange(onModeChange);

  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, animation.getMode());
  animation.play(0);
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY, animation.getMode());
  animation.play(0);
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY, animation.getMode());

  TEST_ASSERT_EQUAL(1, modeChangeCount);
}

void test_pause(void) {
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

void test_loop(void) {
  Animation animation(FPS, FRAMES);
  animation.onModeChange(onModeChange);

  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, animation.getMode());
  animation.loop(0);
  TEST_ASSERT_EQUAL(Animation::MODE_LOOP, animation.getMode());
  animation.loop(0);
  TEST_ASSERT_EQUAL(Animation::MODE_LOOP, animation.getMode());

  TEST_ASSERT_EQUAL(1, modeChangeCount);
}

void test_stop(void) {
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

void test_live(void) {
  Serial_ mock;
  Animation animation(FPS, FRAMES);
  animation.onModeChange(onModeChange);

  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, animation.getMode());
  animation.live(mock);
  TEST_ASSERT_EQUAL(Animation::MODE_LIVE, animation.getMode());
  animation.live(mock);
  TEST_ASSERT_EQUAL(Animation::MODE_LIVE, animation.getMode());

  TEST_ASSERT_EQUAL(1, modeChangeCount);
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_play);
  RUN_TEST(test_pause);
  RUN_TEST(test_loop);
  RUN_TEST(test_stop);
  RUN_TEST(test_live);
  UNITY_END();
}
