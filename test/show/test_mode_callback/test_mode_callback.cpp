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
  Show show;
  Animation animationA(FPS, FRAMES);
  Animation animationB(FPS, FRAMES);
  show.addAnimation(animationA);
  show.addAnimation(animationB);
  show.onModeChange(onModeChange);

  TEST_ASSERT_EQUAL(-1, prevMode);
  TEST_ASSERT_EQUAL(-1, newMode);
  show.playSingle(1, 0);
  TEST_ASSERT_EQUAL(Show::MODE_DEFAULT, prevMode);
  TEST_ASSERT_EQUAL(Show::MODE_PLAY_SINGLE, newMode);
  TEST_ASSERT_EQUAL(1, show.getPlayIndex());
  show.pause();
  TEST_ASSERT_EQUAL(Show::MODE_PLAY_SINGLE, prevMode);
  TEST_ASSERT_EQUAL(Show::MODE_PAUSE, newMode);
}

void test_same_mode(void) {
  Show show;
  Animation animation(FPS, FRAMES);
  show.addAnimation(animation);
  show.onModeChange(onModeChange);

  show.loop(0);
  TEST_ASSERT_EQUAL(Show::MODE_DEFAULT, prevMode);
  TEST_ASSERT_EQUAL(Show::MODE_LOOP, newMode);
  show.loop(0);
  TEST_ASSERT_EQUAL(Show::MODE_DEFAULT, prevMode);
  TEST_ASSERT_EQUAL(Show::MODE_LOOP, newMode);
}

void test_all_modes(void) {
  Serial_ mock;
  Show show;
  Animation animation(FPS, FRAMES);
  show.addAnimation(animation);
  show.onModeChange(onModeChange);

  show.play(0);
  TEST_ASSERT_EQUAL(Show::MODE_DEFAULT, prevMode);
  TEST_ASSERT_EQUAL(Show::MODE_PLAY, newMode);
  TEST_ASSERT_EQUAL(0, show.getPlayIndex());
  show.pause();
  TEST_ASSERT_EQUAL(Show::MODE_PLAY, prevMode);
  TEST_ASSERT_EQUAL(Show::MODE_PAUSE, newMode);
  TEST_ASSERT_EQUAL(0, show.getPlayIndex());
  show.playRandom(0);
  TEST_ASSERT_EQUAL(Show::MODE_PAUSE, prevMode);
  TEST_ASSERT_EQUAL(Show::MODE_PLAY_RANDOM, newMode);
  TEST_ASSERT_EQUAL(0, show.getPlayIndex());
  show.pause();
  show.playSingle(0, 0);
  TEST_ASSERT_EQUAL(Show::MODE_PAUSE, prevMode);
  TEST_ASSERT_EQUAL(Show::MODE_PLAY_SINGLE, newMode);
  TEST_ASSERT_EQUAL(0, show.getPlayIndex());
  show.pause();
  show.loop(0);
  TEST_ASSERT_EQUAL(Show::MODE_PAUSE, prevMode);
  TEST_ASSERT_EQUAL(Show::MODE_LOOP, newMode);
  TEST_ASSERT_EQUAL(0, show.getPlayIndex());
  show.stop();
  TEST_ASSERT_EQUAL(Show::MODE_LOOP, prevMode);
  TEST_ASSERT_EQUAL(Show::MODE_STOP, newMode);
  TEST_ASSERT_EQUAL(0, show.getPlayIndex());
  show.run(0);
  TEST_ASSERT_EQUAL(Show::MODE_STOP, prevMode);
  TEST_ASSERT_EQUAL(Show::MODE_DEFAULT, newMode);
  TEST_ASSERT_EQUAL(0, show.getPlayIndex());
  show.live(mock);
  TEST_ASSERT_EQUAL(Show::MODE_DEFAULT, prevMode);
  TEST_ASSERT_EQUAL(Show::MODE_LIVE, newMode);
  TEST_ASSERT_EQUAL(0, show.getPlayIndex());
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_different_mode);
  RUN_TEST(test_same_mode);
  RUN_TEST(test_all_modes);
  UNITY_END();
}
