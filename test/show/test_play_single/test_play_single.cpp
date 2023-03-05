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

void test_without_animations(void) {
  Show show;
  show.playSingle(0, 0);
  TEST_ASSERT_EQUAL(Show::MODE_DEFAULT, show.getMode());
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_play_single);
  RUN_TEST(test_without_animations);
  UNITY_END();
}
