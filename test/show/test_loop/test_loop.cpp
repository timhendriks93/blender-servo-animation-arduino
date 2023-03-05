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

void test_loop(void) {
  Animation animationA(0, FPS, FRAMES);
  Animation animationB(1, FPS, FRAMES);

  Show show;
  show.addAnimation(animationA);
  show.addAnimation(animationB);

  TEST_ASSERT_TRUE(show.hasAnimations());
  TEST_ASSERT_EQUAL(2, show.countAnimations());

  TEST_ASSERT_EQUAL(Show::MODE_DEFAULT, show.getMode());

  show.loop(0);

  TEST_ASSERT_EQUAL(Show::MODE_LOOP, show.getMode());
  TEST_ASSERT_EQUAL(0, show.getCurrentAnimation()->getID());

  for (long i = 0; i < FRAME_MICROS * (long)6; i++) {
    show.run(i);
  }

  TEST_ASSERT_EQUAL(Show::MODE_LOOP, show.getMode());
  TEST_ASSERT_EQUAL(1, show.getCurrentAnimation()->getID());

  for (long i = 0; i < FRAME_MICROS * (long)5; i++) {
    show.run(i);
  }

  TEST_ASSERT_EQUAL(Show::MODE_LOOP, show.getMode());
  TEST_ASSERT_EQUAL(0, show.getCurrentAnimation()->getID());
}

void test_without_animations(void) {
  Show show;
  show.loop(0);
  TEST_ASSERT_EQUAL(Show::MODE_DEFAULT, show.getMode());
}

void test_prevented(void) {
  Animation animation(0, FPS, FRAMES);
  Serial_ mock;
  Show show;
  show.addAnimation(animation);

  When(OverloadedMethod(ArduinoFake(), random, long(long))).Return(0);

  show.play(0);
  TEST_ASSERT_EQUAL(Show::MODE_PLAY, show.getMode());
  show.loop(0);
  TEST_ASSERT_EQUAL(Show::MODE_PLAY, show.getMode());
  show.pause();
  show.playSingle(0, 0);
  TEST_ASSERT_EQUAL(Show::MODE_PLAY_SINGLE, show.getMode());
  show.loop(0);
  TEST_ASSERT_EQUAL(Show::MODE_PLAY_SINGLE, show.getMode());
  show.pause();
  show.playRandom(0);
  TEST_ASSERT_EQUAL(Show::MODE_PLAY_RANDOM, show.getMode());
  show.loop(0);
  TEST_ASSERT_EQUAL(Show::MODE_PLAY_RANDOM, show.getMode());
  show.stop();
  TEST_ASSERT_EQUAL(Show::MODE_STOP, show.getMode());
  show.loop(0);
  TEST_ASSERT_EQUAL(Show::MODE_STOP, show.getMode());
  show.run(0);
  show.live(mock);
  TEST_ASSERT_EQUAL(Show::MODE_LIVE, show.getMode());
  show.loop(0);
  TEST_ASSERT_EQUAL(Show::MODE_LIVE, show.getMode());
}

void test_allowed(void) {
  Animation animation(FPS, FRAMES);
  Show show;
  show.addAnimation(animation);

  TEST_ASSERT_EQUAL(Show::MODE_DEFAULT, show.getMode());
  show.loop(0);
  TEST_ASSERT_EQUAL(Show::MODE_LOOP, show.getMode());
  show.pause();
  TEST_ASSERT_EQUAL(Show::MODE_PAUSE, show.getMode());
  show.loop(0);
  TEST_ASSERT_EQUAL(Show::MODE_LOOP, show.getMode());
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_loop);
  RUN_TEST(test_without_animations);
  RUN_TEST(test_prevented);
  RUN_TEST(test_allowed);
  UNITY_END();
}
