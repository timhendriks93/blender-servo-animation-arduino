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

void test_without_animations(void) {
  Show show;
  show.playRandom(0);
  TEST_ASSERT_EQUAL(Show::MODE_DEFAULT, show.getMode());
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_play_random);
  RUN_TEST(test_without_animations);
  UNITY_END();
}
