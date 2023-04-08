#include "BlenderServoAnimation.h"
#include <unity.h>

using namespace BlenderServoAnimation;

#define FPS 60
#define FRAMES 5

int prevMode = -1;
int newMode = -1;

void setUp(void) {
  prevMode = -1;
  newMode = -1;
}

void onModeChange(byte prevArg, byte newArg) {
  prevMode = prevArg;
  newMode = newArg;
}

void test_different_mode(void) {
  Animation animation(FPS, FRAMES);
  animation.onModeChange(onModeChange);

  animation.play(0);
  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, prevMode);
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY, newMode);
  animation.pause();
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY, prevMode);
  TEST_ASSERT_EQUAL(Animation::MODE_PAUSE, newMode);
}

void test_same_mode(void) {
  Animation animation(FPS, FRAMES);
  animation.onModeChange(onModeChange);

  animation.loop(0);
  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, prevMode);
  TEST_ASSERT_EQUAL(Animation::MODE_LOOP, newMode);
  animation.loop(0);
  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, prevMode);
  TEST_ASSERT_EQUAL(Animation::MODE_LOOP, newMode);
}

void test_all_modes(void) {
  Serial_ mock;
  Animation animation(FPS, FRAMES);

  animation.onModeChange(onModeChange);

  animation.play(0);
  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, prevMode);
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY, newMode);
  animation.pause();
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY, prevMode);
  TEST_ASSERT_EQUAL(Animation::MODE_PAUSE, newMode);
  animation.loop(0);
  TEST_ASSERT_EQUAL(Animation::MODE_PAUSE, prevMode);
  TEST_ASSERT_EQUAL(Animation::MODE_LOOP, newMode);
  animation.stop();
  TEST_ASSERT_EQUAL(Animation::MODE_LOOP, prevMode);
  TEST_ASSERT_EQUAL(Animation::MODE_STOP, newMode);
  animation.run(0);
  TEST_ASSERT_EQUAL(Animation::MODE_STOP, prevMode);
  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, newMode);
  animation.live(mock);
  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, prevMode);
  TEST_ASSERT_EQUAL(Animation::MODE_LIVE, newMode);
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_different_mode);
  RUN_TEST(test_same_mode);
  RUN_TEST(test_all_modes);
  UNITY_END();
}
