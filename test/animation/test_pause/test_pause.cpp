#include "../test/helper.h"
#include "internal/Animation.h"
#include "internal/LiveStream.h"
#include <unity.h>

using namespace BlenderServoAnimation;
using namespace fakeit;

void setUp(void) {
  ArduinoFakeReset();
  resetPositionLog();
}

void test_pause(byte mode) {
  Animation animation;
  animation.onPositionChange(move);
  animation.addScene(PROGMEM_DATA, DATA_SIZE, FPS, FRAMES);

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
  TEST_ASSERT_EQUAL(0, animation.getCurrentScene()->getFrame());

  for (int i = 0; i < FRAME_MICROS * 3; i += FRAME_MICROS) {
    animation.run(i);
  }

  TEST_ASSERT_EQUAL(3, animation.getCurrentScene()->getFrame());

  animation.pause();

  for (int i = FRAME_MICROS * 3; i < FRAME_MICROS * 6; i += FRAME_MICROS) {
    animation.run(i);
  }

  TEST_ASSERT_EQUAL(Animation::MODE_PAUSE, animation.getMode());
  TEST_ASSERT_EQUAL(3, animation.getCurrentScene()->getFrame());

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

  animation.run(FRAME_MICROS * 7);

  TEST_ASSERT_EQUAL(mode, animation.getMode());
  TEST_ASSERT_EQUAL(4, animation.getCurrentScene()->getFrame());
  TEST_ASSERT_EQUAL(8, logIndex);
}

void test_pause_play(void) {
  test_pause(Animation::MODE_PLAY);
}

void test_pause_loop(void) {
  test_pause(Animation::MODE_LOOP);
}

void test_pause_play_single(void) {
  test_pause(Animation::MODE_PLAY_SINGLE);
}

void test_pause_play_random(void) {
  When(OverloadedMethod(ArduinoFake(), random, long(long))).Return(1, 0);
  test_pause(Animation::MODE_PLAY_RANDOM);
}

void test_prevented(void) {
  LiveStream stream;
  Animation animation;
  animation.onPositionChange(move);
  animation.addScene(stream, FPS, FRAMES);

  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, animation.getMode());
  animation.pause();
  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, animation.getMode());
  animation.play();
  animation.stop();
  TEST_ASSERT_EQUAL(Animation::MODE_STOP, animation.getMode());
  animation.pause();
  TEST_ASSERT_EQUAL(Animation::MODE_STOP, animation.getMode());
}

void test_allowed(void) {
  LiveStream stream;
  Animation animation;
  animation.onPositionChange(move);
  animation.addScene(stream, FPS, FRAMES);

  animation.play();
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY, animation.getMode());
  animation.pause();
  TEST_ASSERT_EQUAL(Animation::MODE_PAUSE, animation.getMode());
  animation.loop();
  TEST_ASSERT_EQUAL(Animation::MODE_LOOP, animation.getMode());
  animation.pause();
  TEST_ASSERT_EQUAL(Animation::MODE_PAUSE, animation.getMode());
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_pause_play);
  RUN_TEST(test_pause_loop);
  RUN_TEST(test_pause_play_single);
  RUN_TEST(test_pause_play_random);
  RUN_TEST(test_prevented);
  RUN_TEST(test_allowed);
  UNITY_END();
}
