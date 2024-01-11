#include "../test/helper.h"
#include "internal/Animation.h"
#include "internal/LiveStream.h"
#include "internal/ProgmemStream.h"
#include "internal/Scene.h"
#include <unity.h>

using namespace BlenderServoAnimation;
using namespace fakeit;

void setUp(void) {
  ArduinoFakeReset();
  resetPositionLog();
}

void test_stop(byte mode) {
  LiveStream stream;
  Animation animation;
  animation.onPositionChange(move);
  animation.addScene(stream, FPS, FRAMES);

  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, animation.getMode());

  switch (mode) {
  case Animation::MODE_PLAY:
    animation.play();
    break;
  case Animation::MODE_PLAY_SINGLE:
    animation.playSingle(0);
    break;
  case Animation::MODE_PLAY_RANDOM:
    animation.playRandom();
    break;
  case Animation::MODE_LOOP:
    animation.loop();
    break;
  }

  TEST_ASSERT_EQUAL(mode, animation.getMode());

  for (int i = 0; i < FRAME_MICROS * 2; i += FRAME_MICROS) {
    animation.run(i);
  }

  animation.stop();

  TEST_ASSERT_EQUAL(Animation::MODE_STOP, animation.getMode());

  for (int i = FRAME_MICROS * 2; i < FRAME_MICROS * 4; i += FRAME_MICROS) {
    animation.run(i);
  }

  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, animation.getMode());
}

void test_stop_play(void) {
  test_stop(Animation::MODE_PLAY);
}

void test_stop_loop(void) {
  test_stop(Animation::MODE_LOOP);
}

void test_stop_play_single(void) {
  test_stop(Animation::MODE_PLAY_SINGLE);
}

void test_stop_play_random(void) {
  When(OverloadedMethod(ArduinoFake(), random, long(long))).Return(1);
  test_stop(Animation::MODE_PLAY_RANDOM);
}

void test_prevented(void) {
  LiveStream stream;
  Animation animation;
  animation.onPositionChange(move);
  animation.addScene(stream, FPS, FRAMES);

  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, animation.getMode());
  animation.stop();
  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, animation.getMode());
}

void test_allowed(void) {
  LiveStream stream;
  Animation animation;
  animation.onPositionChange(move);
  animation.addScene(stream, FPS, FRAMES);

  animation.play();
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY, animation.getMode());
  animation.stop();
  TEST_ASSERT_EQUAL(Animation::MODE_STOP, animation.getMode());
  animation.run(10000);
  animation.playSingle(0);
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY_SINGLE, animation.getMode());
  animation.stop();
  TEST_ASSERT_EQUAL(Animation::MODE_STOP, animation.getMode());
  animation.run(10000);
  animation.playRandom();
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY_RANDOM, animation.getMode());
  animation.stop();
  TEST_ASSERT_EQUAL(Animation::MODE_STOP, animation.getMode());
  animation.run(10000);
  animation.loop();
  TEST_ASSERT_EQUAL(Animation::MODE_LOOP, animation.getMode());
  animation.stop();
  TEST_ASSERT_EQUAL(Animation::MODE_STOP, animation.getMode());
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_stop_play);
  // RUN_TEST(test_stop_play_single);
  // RUN_TEST(test_stop_play_random);
  // RUN_TEST(test_stop_loop);
  // RUN_TEST(test_prevented);
  // RUN_TEST(test_allowed);
  UNITY_END();
}
