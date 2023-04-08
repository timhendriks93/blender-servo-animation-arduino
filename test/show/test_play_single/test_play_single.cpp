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

void test_play_single(void) {
  Animation animations[4] = {
      {FPS, FRAMES},
      {FPS, FRAMES},
      {FPS, 4},
      {FPS, FRAMES},
  };

  Show show;
  show.addAnimations(animations, 4);

  TEST_ASSERT_EQUAL(4, show.countAnimations());
  TEST_ASSERT_EQUAL(Show::MODE_DEFAULT, show.getMode());

  show.playSingle(2, 0);

  TEST_ASSERT_EQUAL(Show::MODE_PLAY_SINGLE, show.getMode());
  TEST_ASSERT_EQUAL(4, show.getCurrentAnimation()->getFrames());

  for (long i = 0; i < FRAME_MICROS * (long)6; i += FRAME_MICROS) {
    show.run(i);
  }

  TEST_ASSERT_EQUAL(Show::MODE_DEFAULT, show.getMode());
}

void test_without_animations(void) {
  Show show;
  show.playSingle(0, 0);
  TEST_ASSERT_EQUAL(Show::MODE_DEFAULT, show.getMode());
}

void test_prevented(void) {
  Animation animation(FPS, FRAMES);
  Serial_ mock;
  Show show;
  show.addAnimation(animation);

  When(OverloadedMethod(ArduinoFake(), random, long(long))).Return(0);

  show.loop(0);
  TEST_ASSERT_EQUAL(Show::MODE_LOOP, show.getMode());
  show.playSingle(0, 0);
  TEST_ASSERT_EQUAL(Show::MODE_LOOP, show.getMode());
  show.pause();
  show.play(0);
  TEST_ASSERT_EQUAL(Show::MODE_PLAY, show.getMode());
  show.playSingle(0, 0);
  TEST_ASSERT_EQUAL(Show::MODE_PLAY, show.getMode());
  show.pause();
  show.playRandom(0);
  TEST_ASSERT_EQUAL(Show::MODE_PLAY_RANDOM, show.getMode());
  show.playSingle(0, 0);
  TEST_ASSERT_EQUAL(Show::MODE_PLAY_RANDOM, show.getMode());
  show.stop();
  TEST_ASSERT_EQUAL(Show::MODE_STOP, show.getMode());
  show.playSingle(0, 0);
  TEST_ASSERT_EQUAL(Show::MODE_STOP, show.getMode());
  show.run(0);
  show.live(mock);
  TEST_ASSERT_EQUAL(Show::MODE_LIVE, show.getMode());
  show.playSingle(0, 0);
  TEST_ASSERT_EQUAL(Show::MODE_LIVE, show.getMode());
}

void test_allowed(void) {
  Animation animation(FPS, FRAMES);
  Show show;
  show.addAnimation(animation);

  TEST_ASSERT_EQUAL(Show::MODE_DEFAULT, show.getMode());
  show.playSingle(0, 0);
  TEST_ASSERT_EQUAL(Show::MODE_PLAY_SINGLE, show.getMode());
  show.pause();
  TEST_ASSERT_EQUAL(Show::MODE_PAUSE, show.getMode());
  show.playSingle(0, 0);
  TEST_ASSERT_EQUAL(Show::MODE_PLAY_SINGLE, show.getMode());
}

void test_prevent_sudden_index_change(void) {
  Show show;
  Animation animationA(FPS, FRAMES);
  Animation animationB(FPS, FRAMES);
  show.addAnimation(animationA);
  show.addAnimation(animationB);

  show.play(0);
  TEST_ASSERT_EQUAL(Show::MODE_PLAY, show.getMode());
  show.run(FRAME_MICROS);
  show.pause();
  TEST_ASSERT_EQUAL(Show::MODE_PAUSE, show.getMode());
  show.playSingle(1, 0);
  TEST_ASSERT_EQUAL(Show::MODE_PAUSE, show.getMode());
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_play_single);
  RUN_TEST(test_without_animations);
  RUN_TEST(test_prevented);
  RUN_TEST(test_allowed);
  RUN_TEST(test_prevent_sudden_index_change);
  UNITY_END();
}
