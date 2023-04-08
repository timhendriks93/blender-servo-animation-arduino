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

void test_play_random(void) {
  Animation animations[3] = {
      {FPS, 2},
      {FPS, 3},
      {FPS, 4},
  };

  Show show;
  show.addAnimations(animations, 3);

  TEST_ASSERT_EQUAL(3, show.countAnimations());
  TEST_ASSERT_EQUAL(Show::MODE_DEFAULT, show.getMode());

  When(OverloadedMethod(ArduinoFake(), random, long(long))).Return(1, 0, 2);

  show.playRandom(0);

  TEST_ASSERT_EQUAL(Show::MODE_PLAY_RANDOM, show.getMode());
  TEST_ASSERT_EQUAL(3, show.getCurrentAnimation()->getFrames());

  for (long i = 0; i < FRAME_MICROS * (long)4; i += FRAME_MICROS) {
    show.run(i);
  }

  TEST_ASSERT_EQUAL(Show::MODE_PLAY_RANDOM, show.getMode());
  TEST_ASSERT_EQUAL(2, show.getCurrentAnimation()->getFrames());

  for (long i = 0; i < FRAME_MICROS * (long)2; i += FRAME_MICROS) {
    show.run(i);
  }

  TEST_ASSERT_EQUAL(Show::MODE_PLAY_RANDOM, show.getMode());
  TEST_ASSERT_EQUAL(4, show.getCurrentAnimation()->getFrames());
}

void test_without_animations(void) {
  Show show;
  show.playRandom(0);
  TEST_ASSERT_EQUAL(Show::MODE_DEFAULT, show.getMode());
}

void test_prevented(void) {
  Animation animation(FPS, FRAMES);
  Serial_ mock;
  Show show;
  show.addAnimation(animation);

  show.loop(0);
  TEST_ASSERT_EQUAL(Show::MODE_LOOP, show.getMode());
  show.playRandom(0);
  TEST_ASSERT_EQUAL(Show::MODE_LOOP, show.getMode());
  show.pause();
  show.play(0);
  TEST_ASSERT_EQUAL(Show::MODE_PLAY, show.getMode());
  show.playRandom(0);
  TEST_ASSERT_EQUAL(Show::MODE_PLAY, show.getMode());
  show.pause();
  show.playSingle(0, 0);
  TEST_ASSERT_EQUAL(Show::MODE_PLAY_SINGLE, show.getMode());
  show.playRandom(0);
  TEST_ASSERT_EQUAL(Show::MODE_PLAY_SINGLE, show.getMode());
  show.stop();
  TEST_ASSERT_EQUAL(Show::MODE_STOP, show.getMode());
  show.playRandom(0);
  TEST_ASSERT_EQUAL(Show::MODE_STOP, show.getMode());
  show.run(0);
  show.live(mock);
  TEST_ASSERT_EQUAL(Show::MODE_LIVE, show.getMode());
  show.playRandom(0);
  TEST_ASSERT_EQUAL(Show::MODE_LIVE, show.getMode());
}

void test_allowed(void) {
  Animation animation(FPS, FRAMES);
  Show show;
  show.addAnimation(animation);

  TEST_ASSERT_EQUAL(Show::MODE_DEFAULT, show.getMode());
  show.playRandom(0);
  TEST_ASSERT_EQUAL(Show::MODE_PLAY_RANDOM, show.getMode());
  show.pause();
  TEST_ASSERT_EQUAL(Show::MODE_PAUSE, show.getMode());
  show.playRandom(0);
  TEST_ASSERT_EQUAL(Show::MODE_PLAY_RANDOM, show.getMode());
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_play_random);
  RUN_TEST(test_without_animations);
  RUN_TEST(test_prevented);
  RUN_TEST(test_allowed);
  UNITY_END();
}
