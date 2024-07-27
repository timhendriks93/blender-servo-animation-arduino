#include "../test/helper.h"
#include "BlenderServoAnimation.h"
#include <unity.h>

using namespace fakeit;

void setUp(void) {
  ArduinoFakeReset();
}

void test_prevented(void) {
  BlenderServoAnimation animation;
  StreamMock mock;
  animation.addScene(PROGMEM_DATA, DATA_SIZE, FPS, FRAMES);

  When(OverloadedMethod(ArduinoFake(), random, long(long))).Return(0);

  animation.loop();
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_LOOP, animation.getMode());
  animation.live(mock);
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_LOOP, animation.getMode());
  animation.pause();
  animation.play();
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_PLAY, animation.getMode());
  animation.live(mock);
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_PLAY, animation.getMode());
  animation.pause();
  animation.playSingle(0);
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_PLAY_SINGLE, animation.getMode());
  animation.live(mock);
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_PLAY_SINGLE, animation.getMode());
  animation.pause();
  animation.playRandom();
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_PLAY_RANDOM, animation.getMode());
  animation.live(mock);
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_PLAY_RANDOM, animation.getMode());
  animation.pause();
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_PAUSE, animation.getMode());
  animation.live(mock);
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_PAUSE, animation.getMode());
  animation.stop();
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_STOP, animation.getMode());
  animation.live(mock);
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_STOP, animation.getMode());
}

void test_allowed(void) {
  StreamMock mock;
  BlenderServoAnimation animation;
  animation.addScene(PROGMEM_DATA, DATA_SIZE, FPS, FRAMES);

  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_DEFAULT, animation.getMode());
  animation.live(mock);
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_LIVE, animation.getMode());
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_prevented);
  RUN_TEST(test_allowed);
  UNITY_END();
}