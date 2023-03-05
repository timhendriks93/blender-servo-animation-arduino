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

void test_without_animations(void) {
  Show show;
  show.play(0);
  TEST_ASSERT_EQUAL(Show::MODE_DEFAULT, show.getMode());
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_play);
  RUN_TEST(test_without_animations);
  UNITY_END();
}
