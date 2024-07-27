#include "../test/helper.h"
#include "BlenderServoAnimation.h"
#include "Scene.h"
#include <unity.h>

using namespace fakeit;

void setUp(void) {
  ArduinoFakeReset();
  resetPositionLog();
}

void test_stop(byte mode) {
  StreamMock mock;
  mock.availableValue = 0;
  BlenderServoAnimation animation;
  animation.onPositionChange(move);
  animation.addScene(PROGMEM_DATA, DATA_SIZE, FPS, FRAMES);

  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_DEFAULT, animation.getMode());

  switch (mode) {
  case BlenderServoAnimation::MODE_PLAY:
    animation.play();
    break;
  case BlenderServoAnimation::MODE_PLAY_SINGLE:
    animation.playSingle(0);
    break;
  case BlenderServoAnimation::MODE_PLAY_RANDOM:
    animation.playRandom();
    break;
  case BlenderServoAnimation::MODE_LOOP:
    animation.loop();
    break;
  case BlenderServoAnimation::MODE_LIVE:
    animation.live(mock);
    break;
  }

  TEST_ASSERT_EQUAL(mode, animation.getMode());

  for (int i = 0; i < FRAME_MICROS * 2; i += FRAME_MICROS) {
    animation.run(i);
  }

  if (mode != BlenderServoAnimation::MODE_LIVE) {
    TEST_ASSERT_EQUAL(2, animation.getCurrentScene()->getFrame());
  }

  animation.stop();

  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_STOP, animation.getMode());

  for (int i = FRAME_MICROS * 2; i < FRAME_MICROS * 4; i += FRAME_MICROS) {
    animation.run(i);
  }

  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_DEFAULT, animation.getMode());
}

void test_stop_play(void) {
  test_stop(BlenderServoAnimation::MODE_PLAY);
}

void test_stop_loop(void) {
  test_stop(BlenderServoAnimation::MODE_LOOP);
}

void test_stop_play_single(void) {
  test_stop(BlenderServoAnimation::MODE_PLAY_SINGLE);
}

void test_stop_play_random(void) {
  When(OverloadedMethod(ArduinoFake(), random, long(long))).Return(1);
  test_stop(BlenderServoAnimation::MODE_PLAY_RANDOM);
}

void test_stop_live(void) {
  test_stop(BlenderServoAnimation::MODE_LIVE);
}

void test_prevented(void) {
  BlenderServoAnimation animation;
  animation.onPositionChange(move);
  animation.addScene(PROGMEM_DATA, DATA_SIZE, FPS, FRAMES);

  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_DEFAULT, animation.getMode());
  animation.stop();
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_DEFAULT, animation.getMode());
}

void test_allowed(void) {
  StreamMock mock;
  BlenderServoAnimation animation;
  animation.onPositionChange(move);
  animation.addScene(PROGMEM_DATA, DATA_SIZE, FPS, FRAMES);

  animation.play();
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_PLAY, animation.getMode());
  animation.stop();
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_STOP, animation.getMode());
  animation.run(10000);
  animation.playSingle(0);
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_PLAY_SINGLE, animation.getMode());
  animation.stop();
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_STOP, animation.getMode());
  animation.run(10000);
  animation.playRandom();
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_PLAY_RANDOM, animation.getMode());
  animation.stop();
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_STOP, animation.getMode());
  animation.run(10000);
  animation.loop();
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_LOOP, animation.getMode());
  animation.stop();
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_STOP, animation.getMode());
  animation.run(10000);
  animation.live(mock);
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_LIVE, animation.getMode());
  animation.stop();
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_STOP, animation.getMode());
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
