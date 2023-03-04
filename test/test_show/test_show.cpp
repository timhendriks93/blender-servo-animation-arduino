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

void test_play(void) {
  Animation animationA(0, FPS, FRAMES);
  Animation animationB(1, FPS, FRAMES);

  Show show;

  TEST_ASSERT_FALSE(show.hasAnimations());
  TEST_ASSERT_EQUAL(0, show.countAnimations());
  TEST_ASSERT_FALSE(show.hasAnimation(0));
  TEST_ASSERT_FALSE(show.hasAnimation(1));
  TEST_ASSERT_EQUAL(nullptr, show.getCurrentAnimation());

  show.addAnimation(animationB);
  show.addAnimation(animationA);

  TEST_ASSERT_TRUE(show.hasAnimations());
  TEST_ASSERT_EQUAL(2, show.countAnimations());
  TEST_ASSERT_TRUE(show.hasAnimation(0));
  TEST_ASSERT_TRUE(show.hasAnimation(1));

  TEST_ASSERT_EQUAL(Show::MODE_DEFAULT, show.getMode());

  show.play(0);

  TEST_ASSERT_EQUAL(Show::MODE_PLAY, show.getMode());
  TEST_ASSERT_NOT_EQUAL(nullptr, show.getCurrentAnimation());
  TEST_ASSERT_EQUAL(1, show.getCurrentAnimation()->getID());

  for (long i = 0; i < FRAME_MICROS * (long)6; i++) {
    show.run(i);
  }

  TEST_ASSERT_EQUAL(Show::MODE_PLAY, show.getMode());
  TEST_ASSERT_EQUAL(0, show.getCurrentAnimation()->getID());

  for (long i = 0; i < FRAME_MICROS * (long)5; i++) {
    show.run(i);
  }

  TEST_ASSERT_EQUAL(Show::MODE_DEFAULT, show.getMode());
}

void test_play_single(void) {
  Animation animations[4] = {
      {0, FPS, FRAMES},
      {1, FPS, FRAMES},
      {2, FPS, FRAMES},
      {3, FPS, FRAMES},
  };

  Show show;
  show.addAnimations(animations, 4);

  TEST_ASSERT_EQUAL(4, show.countAnimations());
  TEST_ASSERT_EQUAL(Show::MODE_DEFAULT, show.getMode());

  show.playSingle(2, 0);

  TEST_ASSERT_EQUAL(Show::MODE_PLAY_SINGLE, show.getMode());
  TEST_ASSERT_EQUAL(2, show.getCurrentAnimation()->getID());

  for (long i = 0; i < FRAME_MICROS * (long)6; i++) {
    show.run(i);
  }

  TEST_ASSERT_EQUAL(Show::MODE_DEFAULT, show.getMode());
}

void test_play_random(void) {
  Animation animations[3] = {
      {0, FPS, FRAMES},
      {1, FPS, FRAMES},
      {2, FPS, FRAMES},
  };

  Show show;
  show.addAnimations(animations, 3);

  TEST_ASSERT_EQUAL(3, show.countAnimations());
  TEST_ASSERT_EQUAL(Show::MODE_DEFAULT, show.getMode());

  When(OverloadedMethod(ArduinoFake(), random, long(long))).Return(1, 0, 2);

  show.playRandom(0);

  TEST_ASSERT_EQUAL(Show::MODE_PLAY_RANDOM, show.getMode());
  TEST_ASSERT_EQUAL(1, show.getCurrentAnimation()->getID());

  for (long i = 0; i < FRAME_MICROS * (long)6; i++) {
    show.run(i);
  }

  TEST_ASSERT_EQUAL(Show::MODE_PLAY_RANDOM, show.getMode());
  TEST_ASSERT_EQUAL(0, show.getCurrentAnimation()->getID());

  for (long i = 0; i < FRAME_MICROS * (long)5; i++) {
    show.run(i);
  }

  TEST_ASSERT_EQUAL(Show::MODE_PLAY_RANDOM, show.getMode());
  TEST_ASSERT_EQUAL(2, show.getCurrentAnimation()->getID());
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

void test_playback_without_animations(void) {
  Show show;
  show.play(0);
  TEST_ASSERT_EQUAL(Show::MODE_DEFAULT, show.getMode());
  show.playRandom(0);
  TEST_ASSERT_EQUAL(Show::MODE_DEFAULT, show.getMode());
  show.playSingle(0, 0);
  TEST_ASSERT_EQUAL(Show::MODE_DEFAULT, show.getMode());
  show.loop(0);
  TEST_ASSERT_EQUAL(Show::MODE_DEFAULT, show.getMode());
}

void test_pause(byte mode) {
  Animation animationA(0, FPS, FRAMES);
  Animation animationB(1, FPS, FRAMES);

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

  for (long i = 0; i < FRAME_MICROS * (long)3; i++) {
    show.run(i);
  }

  TEST_ASSERT_EQUAL(2, show.getCurrentAnimation()->getFrame());

  show.pause();

  for (long i = 0; i < FRAME_MICROS * (long)3; i++) {
    show.run(i);
  }

  TEST_ASSERT_EQUAL(Show::MODE_PAUSE, show.getMode());
  TEST_ASSERT_EQUAL(2, show.getCurrentAnimation()->getFrame());

  switch (mode) {
    case Show::MODE_PLAY:
      show.play(0);
      break;
    case Show::MODE_PLAY_SINGLE:
      show.playSingle(1, 0);
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

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_play);
  RUN_TEST(test_play_single);
  RUN_TEST(test_play_random);
  RUN_TEST(test_loop);
  RUN_TEST(test_playback_without_animations);
  RUN_TEST(test_pause_play);
  RUN_TEST(test_pause_loop);
  RUN_TEST(test_pause_play_single);
  RUN_TEST(test_pause_play_random);
  UNITY_END();
}
