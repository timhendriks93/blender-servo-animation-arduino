#include "../test/helper.h"
#include "BlenderServoAnimation.h"
#include <unity.h>

using namespace fakeit;

void setUp(void) {
  ArduinoFakeReset();
  resetPositionLog();
}

void test_loop(void) {
  BlenderServoAnimation animation;
  animation.onPositionChange(move);
  animation.addScene(PROGMEM_DATA, DATA_SIZE, FPS, FRAMES);
  animation.addScene(PROGMEM_DATA, DATA_SIZE, FPS, FRAMES);

  TEST_ASSERT_TRUE(animation.hasScenes());
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_DEFAULT, animation.getMode());

  animation.loop();

  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_LOOP, animation.getMode());
  TEST_ASSERT_EQUAL(0, animation.getPlayIndex());

  for (int i = 0; i < FRAME_MICROS * 5; i += FRAME_MICROS) {
    animation.run(i);
  }

  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_LOOP, animation.getMode());
  TEST_ASSERT_EQUAL(1, animation.getPlayIndex());

  for (int i = FRAME_MICROS * 5; i < FRAME_MICROS * 10; i += FRAME_MICROS) {
    animation.run(i);
  }

  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_LOOP, animation.getMode());
  TEST_ASSERT_EQUAL(0, animation.getPlayIndex());
  TEST_ASSERT_EQUAL(20, logIndex);
}

void test_without_scenes(void) {
  BlenderServoAnimation animation;
  animation.onPositionChange(move);
  animation.loop();
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_DEFAULT, animation.getMode());
}

void test_prevented(void) {
  StreamMock mock;
  BlenderServoAnimation animation;
  animation.onPositionChange(move);
  animation.addScene(PROGMEM_DATA, DATA_SIZE, FPS, FRAMES);

  When(OverloadedMethod(ArduinoFake(), random, long(long))).Return(0);

  animation.play();
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_PLAY, animation.getMode());
  animation.loop();
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_PLAY, animation.getMode());
  animation.pause();
  animation.playSingle(0);
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_PLAY_SINGLE, animation.getMode());
  animation.loop();
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_PLAY_SINGLE, animation.getMode());
  animation.pause();
  animation.playRandom();
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_PLAY_RANDOM, animation.getMode());
  animation.loop();
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_PLAY_RANDOM, animation.getMode());
  animation.stop();
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_STOP, animation.getMode());
  animation.loop();
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_STOP, animation.getMode());
  animation.run(10000);
  animation.live(mock);
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_LIVE, animation.getMode());
  animation.loop();
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_LIVE, animation.getMode());
}

void test_allowed(void) {
  BlenderServoAnimation animation;
  animation.onPositionChange(move);
  animation.addScene(PROGMEM_DATA, DATA_SIZE, FPS, FRAMES);

  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_DEFAULT, animation.getMode());
  animation.loop();
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_LOOP, animation.getMode());
  animation.pause();
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_PAUSE, animation.getMode());
  animation.loop();
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_LOOP, animation.getMode());
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_loop);
  RUN_TEST(test_without_scenes);
  RUN_TEST(test_prevented);
  RUN_TEST(test_allowed);
  UNITY_END();
}
