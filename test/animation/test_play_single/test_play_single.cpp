#include "../test/helper.h"
#include "BlenderServoAnimation.h"
#include <unity.h>

using namespace fakeit;

void setUp(void) {
  ArduinoFakeReset();
  resetPositionLog();
}

void test_play_single(void) {
  BlenderServoAnimation animation;
  animation.onPositionChange(move);
  animation.addScene(PROGMEM_DATA, DATA_SIZE, FPS, FRAMES);
  animation.addScene(PROGMEM_DATA, DATA_SIZE, FPS, FRAMES);
  animation.addScene(PROGMEM_DATA, DATA_SIZE, FPS, FRAMES);

  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_DEFAULT, animation.getMode());

  animation.playSingle(2);

  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_PLAY_SINGLE, animation.getMode());
  TEST_ASSERT_EQUAL(2, animation.getPlayIndex());

  for (long i = 0; i < ANIMATION_MICROS; i += FRAME_MICROS) {
    animation.run(i);
  }

  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_DEFAULT, animation.getMode());
  TEST_ASSERT_EQUAL(10, logIndex);
}

void test_without_scenes(void) {
  BlenderServoAnimation animation;
  animation.onPositionChange(move);
  animation.playSingle(0);
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_DEFAULT, animation.getMode());
}

void test_prevented(void) {
  StreamMock mock;
  BlenderServoAnimation animation;
  animation.onPositionChange(move);
  animation.addScene(PROGMEM_DATA, DATA_SIZE, FPS, FRAMES);

  When(OverloadedMethod(ArduinoFake(), random, long(long))).Return(0);

  animation.loop();
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_LOOP, animation.getMode());
  animation.playSingle(0);
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_LOOP, animation.getMode());
  animation.pause();
  animation.play();
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_PLAY, animation.getMode());
  animation.playSingle(0);
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_PLAY, animation.getMode());
  animation.pause();
  animation.playRandom();
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_PLAY_RANDOM, animation.getMode());
  animation.playSingle(0);
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_PLAY_RANDOM, animation.getMode());
  animation.stop();
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_STOP, animation.getMode());
  animation.playSingle(0);
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_STOP, animation.getMode());
  animation.run(10000);
  animation.live(mock);
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_LIVE, animation.getMode());
  animation.playSingle(0);
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_LIVE, animation.getMode());
}

void test_allowed(void) {
  BlenderServoAnimation animation;
  animation.onPositionChange(move);
  animation.addScene(PROGMEM_DATA, DATA_SIZE, FPS, FRAMES);

  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_DEFAULT, animation.getMode());
  animation.playSingle(0);
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_PLAY_SINGLE, animation.getMode());
  animation.pause();
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_PAUSE, animation.getMode());
  animation.playSingle(0);
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_PLAY_SINGLE, animation.getMode());
}

void test_prevent_sudden_index_change(void) {
  BlenderServoAnimation animation;
  animation.onPositionChange(move);
  animation.addScene(PROGMEM_DATA, DATA_SIZE, FPS, FRAMES);
  animation.addScene(PROGMEM_DATA, DATA_SIZE, FPS, FRAMES);

  animation.play();
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_PLAY, animation.getMode());
  animation.run(FRAME_MICROS);
  animation.pause();
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_PAUSE, animation.getMode());
  animation.playSingle(1);
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_PAUSE, animation.getMode());
  TEST_ASSERT_EQUAL(2, logIndex);
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_play_single);
  RUN_TEST(test_without_scenes);
  RUN_TEST(test_prevented);
  RUN_TEST(test_allowed);
  RUN_TEST(test_prevent_sudden_index_change);
  UNITY_END();
}
