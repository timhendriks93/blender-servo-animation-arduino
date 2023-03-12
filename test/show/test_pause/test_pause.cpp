#include "BlenderServoAnimation.h"
#include <unity.h>

using namespace BlenderServoAnimation;
using namespace fakeit;

#define FPS 60
#define FRAMES 5
#define FRAME_MICROS 16667

void setUp(void) {
  ArduinoFakeReset();
}

void test_pause(byte mode) {
  Animation animationA(FPS, FRAMES);
  Animation animationB(FPS, FRAMES);

  Show show;
  show.addAnimation(animationA);
  show.addAnimation(animationB);

  TEST_ASSERT_EQUAL(Show::MODE_DEFAULT, show.getMode());

  switch (mode) {
  case Show::MODE_PLAY:
    show.play(0);
    break;
  case Show::MODE_PLAY_SINGLE:
    show.playSingle(0, 0);
    break;
  case Show::MODE_PLAY_RANDOM:
    show.playRandom(0);
    break;
  case Show::MODE_LOOP:
    show.loop(0);
    break;
  }

  TEST_ASSERT_EQUAL(mode, show.getMode());
  TEST_ASSERT_EQUAL(0, show.getCurrentAnimation()->getFrame());

  for (long i = 0; i < FRAME_MICROS * (long)3; i += FRAME_MICROS) {
    show.run(i);
  }

  TEST_ASSERT_EQUAL(2, show.getCurrentAnimation()->getFrame());

  show.pause();

  for (long i = 0; i < FRAME_MICROS * (long)3; i += FRAME_MICROS) {
    show.run(i);
  }

  TEST_ASSERT_EQUAL(Show::MODE_PAUSE, show.getMode());
  TEST_ASSERT_EQUAL(2, show.getCurrentAnimation()->getFrame());

  switch (mode) {
  case Show::MODE_PLAY:
    show.play(0);
    break;
  case Show::MODE_PLAY_SINGLE:
    show.playSingle(0, 0);
    break;
  case Show::MODE_PLAY_RANDOM:
    show.playRandom(0);
    break;
  case Show::MODE_LOOP:
    show.loop(0);
    break;
  }

  show.run(FRAME_MICROS);

  TEST_ASSERT_EQUAL(mode, show.getMode());
  TEST_ASSERT_EQUAL(3, show.getCurrentAnimation()->getFrame());
}

void test_pause_play(void) {
  test_pause(Show::MODE_PLAY);
}

void test_pause_loop(void) {
  test_pause(Show::MODE_LOOP);
}

void test_pause_play_single(void) {
  test_pause(Show::MODE_PLAY_SINGLE);
}

void test_pause_play_random(void) {
  When(OverloadedMethod(ArduinoFake(), random, long(long))).Return(1, 0);
  test_pause(Show::MODE_PLAY_RANDOM);
}

void test_prevented(void) {
  Serial_ mock;
  Animation animation(FPS, FRAMES);
  Show show;
  show.addAnimation(animation);

  TEST_ASSERT_EQUAL(Show::MODE_DEFAULT, show.getMode());
  show.pause();
  TEST_ASSERT_EQUAL(Show::MODE_DEFAULT, show.getMode());
  show.play(0);
  show.stop();
  TEST_ASSERT_EQUAL(Show::MODE_STOP, show.getMode());
  show.pause();
  TEST_ASSERT_EQUAL(Show::MODE_STOP, show.getMode());
  show.run(0);
  show.live(mock);
  TEST_ASSERT_EQUAL(Show::MODE_LIVE, show.getMode());
  show.pause();
  TEST_ASSERT_EQUAL(Show::MODE_LIVE, show.getMode());
}

void test_allowed(void) {
  Animation animation(FPS, FRAMES);
  Show show;
  show.addAnimation(animation);

  show.play(0);
  TEST_ASSERT_EQUAL(Show::MODE_PLAY, show.getMode());
  show.pause();
  TEST_ASSERT_EQUAL(Show::MODE_PAUSE, show.getMode());
  show.loop(0);
  TEST_ASSERT_EQUAL(Show::MODE_LOOP, show.getMode());
  show.pause();
  TEST_ASSERT_EQUAL(Show::MODE_PAUSE, show.getMode());
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_pause_play);
  RUN_TEST(test_pause_loop);
  RUN_TEST(test_pause_play_single);
  RUN_TEST(test_pause_play_random);
  RUN_TEST(test_prevented);
  RUN_TEST(test_allowed);
  UNITY_END();
}
