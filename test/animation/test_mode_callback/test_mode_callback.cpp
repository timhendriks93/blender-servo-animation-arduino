#include "../test/helper.h"
#include "BlenderServoAnimation.h"

#include <unity.h>

int prevMode = -1;
int newMode = -1;

void setUp(void) {
  prevMode = -1;
  newMode = -1;
}

void onModeChange(byte prevArg, byte newArg) {
  prevMode = prevArg;
  newMode = newArg;
}

void test_different_mode(void) {
  BlenderServoAnimation animation;
  animation.onPositionChange(move);
  animation.onModeChange(onModeChange);
  animation.addScene(PROGMEM_DATA, DATA_SIZE, FPS, FRAMES);
  animation.addScene(PROGMEM_DATA, DATA_SIZE, FPS, FRAMES);

  TEST_ASSERT_EQUAL(-1, prevMode);
  TEST_ASSERT_EQUAL(-1, newMode);
  animation.playSingle(1);
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_DEFAULT, prevMode);
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_PLAY_SINGLE, newMode);
  TEST_ASSERT_EQUAL(1, animation.getPlayIndex());
  animation.pause();
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_PLAY_SINGLE, prevMode);
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_PAUSE, newMode);
}

void test_same_mode(void) {
  BlenderServoAnimation animation;
  animation.onPositionChange(move);
  animation.onModeChange(onModeChange);
  animation.addScene(PROGMEM_DATA, DATA_SIZE, FPS, FRAMES);

  animation.loop();
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_DEFAULT, prevMode);
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_LOOP, newMode);
  animation.loop();
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_DEFAULT, prevMode);
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_LOOP, newMode);
}

void test_all_modes(void) {
  StreamMock mock;
  BlenderServoAnimation animation;
  animation.onPositionChange(move);
  animation.onModeChange(onModeChange);
  animation.addScene(PROGMEM_DATA, DATA_SIZE, FPS, FRAMES);

  animation.play();
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_DEFAULT, prevMode);
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_PLAY, newMode);
  TEST_ASSERT_EQUAL(0, animation.getPlayIndex());
  animation.pause();
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_PLAY, prevMode);
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_PAUSE, newMode);
  TEST_ASSERT_EQUAL(0, animation.getPlayIndex());
  animation.playRandom();
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_PAUSE, prevMode);
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_PLAY_RANDOM, newMode);
  TEST_ASSERT_EQUAL(0, animation.getPlayIndex());
  animation.pause();
  animation.playSingle(0);
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_PAUSE, prevMode);
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_PLAY_SINGLE, newMode);
  TEST_ASSERT_EQUAL(0, animation.getPlayIndex());
  animation.pause();
  animation.loop();
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_PAUSE, prevMode);
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_LOOP, newMode);
  TEST_ASSERT_EQUAL(0, animation.getPlayIndex());
  animation.stop();
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_LOOP, prevMode);
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_STOP, newMode);
  TEST_ASSERT_EQUAL(0, animation.getPlayIndex());
  animation.run(10000);
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_STOP, prevMode);
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_DEFAULT, newMode);
  TEST_ASSERT_EQUAL(0, animation.getPlayIndex());
  animation.live(mock);
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_DEFAULT, prevMode);
  TEST_ASSERT_EQUAL(BlenderServoAnimation::MODE_LIVE, newMode);
  TEST_ASSERT_EQUAL(0, animation.getPlayIndex());
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_different_mode);
  RUN_TEST(test_same_mode);
  RUN_TEST(test_all_modes);
  UNITY_END();
}
