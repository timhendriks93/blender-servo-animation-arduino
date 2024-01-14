#include "../test/helper.h"
#include "internal/Animation.h"
#include <unity.h>

using namespace BlenderServoAnimation;
using namespace fakeit;

void setUp(void) {
  ArduinoFakeReset();
}

void test_prevented(void) {
  Animation animation;
  Serial_ mock;
  animation.addScene(mock);

  When(OverloadedMethod(ArduinoFake(), random, long(long))).Return(0);

  animation.loop();
  TEST_ASSERT_EQUAL(Animation::MODE_LOOP, animation.getMode());
  animation.live(mock);
  TEST_ASSERT_EQUAL(Animation::MODE_LOOP, animation.getMode());
  animation.pause();
  animation.play();
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY, animation.getMode());
  animation.live(mock);
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY, animation.getMode());
  animation.pause();
  animation.playSingle(0);
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY_SINGLE, animation.getMode());
  animation.live(mock);
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY_SINGLE, animation.getMode());
  animation.pause();
  animation.playRandom();
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY_RANDOM, animation.getMode());
  animation.live(mock);
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY_RANDOM, animation.getMode());
  animation.pause();
  TEST_ASSERT_EQUAL(Animation::MODE_PAUSE, animation.getMode());
  animation.live(mock);
  TEST_ASSERT_EQUAL(Animation::MODE_PAUSE, animation.getMode());
  animation.stop();
  TEST_ASSERT_EQUAL(Animation::MODE_STOP, animation.getMode());
  animation.live(mock);
  TEST_ASSERT_EQUAL(Animation::MODE_STOP, animation.getMode());
}

void test_allowed(void) {
  Serial_ mock;
  Animation animation;
  animation.addScene(mock);

  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, animation.getMode());
  animation.live(mock);
  TEST_ASSERT_EQUAL(Animation::MODE_LIVE, animation.getMode());
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_prevented);
  RUN_TEST(test_allowed);
  UNITY_END();
}