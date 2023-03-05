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

const int positionsA[5] PROGMEM = {350, 340, 330, 340, 330};
const int positionsB[5] PROGMEM = {250, 240, 230, 240, 230};

void test_stop(byte mode) {
  Animation animationA(0, FPS, FRAMES);
  Animation animationB(1, FPS, FRAMES);

  Serial_ mock;
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
  case Show::MODE_LIVE:
    show.live(mock);
    break;
  }

  TEST_ASSERT_EQUAL(mode, show.getMode());
  TEST_ASSERT_EQUAL(0, show.getCurrentAnimation()->getFrame());

  for (long i = 0; i < FRAME_MICROS * (long)3; i++) {
    show.run(i);
  }

  if (mode != Show::MODE_LIVE) {
    TEST_ASSERT_EQUAL(2, show.getCurrentAnimation()->getFrame());
  }

  show.stop();

  TEST_ASSERT_EQUAL(Show::MODE_STOP, show.getMode());

  for (long i = 0; i < FRAME_MICROS * (long)3; i++) {
    show.run(i);
  }

  TEST_ASSERT_EQUAL(Show::MODE_DEFAULT, show.getMode());
  TEST_ASSERT_EQUAL(0, show.getCurrentAnimation()->getFrame());
}

void test_stop_play(void) {
  test_stop(Show::MODE_PLAY);
}

void test_stop_loop(void) {
  test_stop(Show::MODE_LOOP);
}

void test_stop_play_single(void) {
  test_stop(Show::MODE_PLAY_SINGLE);
}

void test_stop_play_random(void) {
  When(OverloadedMethod(ArduinoFake(), random, long(long))).Return(1);
  test_stop(Show::MODE_PLAY_RANDOM);
}

void test_stop_live(void) {
  test_stop(Show::MODE_LIVE);
}

void test_prevented(void) {
  Animation animation(FPS, FRAMES);
  Show show;
  show.addAnimation(animation);

  TEST_ASSERT_EQUAL(Show::MODE_DEFAULT, show.getMode());
  show.stop();
  TEST_ASSERT_EQUAL(Show::MODE_DEFAULT, show.getMode());
}

void test_allowed(void) {
  Serial_ mock;
  Animation animation(FPS, FRAMES);
  Show show;
  show.addAnimation(animation);

  show.play(0);
  TEST_ASSERT_EQUAL(Show::MODE_PLAY, show.getMode());
  show.stop();
  TEST_ASSERT_EQUAL(Show::MODE_STOP, show.getMode());
  show.run(0);
  show.playSingle(0, 0);
  TEST_ASSERT_EQUAL(Show::MODE_PLAY_SINGLE, show.getMode());
  show.stop();
  TEST_ASSERT_EQUAL(Show::MODE_STOP, show.getMode());
  show.run(0);
  show.playRandom(0);
  TEST_ASSERT_EQUAL(Show::MODE_PLAY_RANDOM, show.getMode());
  show.stop();
  TEST_ASSERT_EQUAL(Show::MODE_STOP, show.getMode());
  show.run(0);
  show.loop(0);
  TEST_ASSERT_EQUAL(Show::MODE_LOOP, show.getMode());
  show.stop();
  TEST_ASSERT_EQUAL(Show::MODE_STOP, show.getMode());
  show.run(0);
  show.live(mock);
  TEST_ASSERT_EQUAL(Show::MODE_LIVE, show.getMode());
  show.stop();
  TEST_ASSERT_EQUAL(Show::MODE_STOP, show.getMode());
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_stop_play);
  RUN_TEST(test_stop_play_single);
  RUN_TEST(test_stop_play_random);
  RUN_TEST(test_stop_loop);
  RUN_TEST(test_stop_live);
  RUN_TEST(test_prevented);
  RUN_TEST(test_allowed);
  UNITY_END();
}
