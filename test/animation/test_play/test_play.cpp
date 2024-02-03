#include "../test/helper.h"
#include "internal/Animation.h"
#include <unity.h>

using namespace BlenderServoAnimation;
using namespace fakeit;

void setUp(void) {
  ArduinoFakeReset();
  resetPositionLog();
}

void test_play(void) {
  Animation animation;
  animation.onPositionChange(move);

  TEST_ASSERT_FALSE(animation.hasScenes());
  TEST_ASSERT_EQUAL(nullptr, animation.getCurrentScene());

  animation.addScene(PROGMEM_DATA, DATA_SIZE, FPS, FRAMES);

  TEST_ASSERT_TRUE(animation.hasScenes());
  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, animation.getMode());

  animation.play();

  TEST_ASSERT_EQUAL(Animation::MODE_PLAY, animation.getMode());
  TEST_ASSERT_NOT_EQUAL(nullptr, animation.getCurrentScene());
  TEST_ASSERT_EQUAL(5, animation.getCurrentScene()->getFrames());

  for (int i = 0; i < ANIMATION_MICROS; i += FRAME_MICROS) {
    animation.run(i);
  }

  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, animation.getMode());
  TEST_ASSERT_EQUAL(nullptr, animation.getCurrentScene());
  TEST_ASSERT_EQUAL(10, logIndex);
}

void test_without_scenes(void) {
  Animation animation;
  animation.onPositionChange(move);
  animation.play();
  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, animation.getMode());
}

void test_prevented(void) {
  StreamMock mock;
  Animation animation;
  animation.onPositionChange(move);
  animation.addScene(PROGMEM_DATA, DATA_SIZE, FPS, FRAMES);

  When(OverloadedMethod(ArduinoFake(), random, long(long))).Return(0);

  animation.loop();
  TEST_ASSERT_EQUAL(Animation::MODE_LOOP, animation.getMode());
  animation.play();
  TEST_ASSERT_EQUAL(Animation::MODE_LOOP, animation.getMode());
  animation.pause();
  animation.playSingle(0);
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY_SINGLE, animation.getMode());
  animation.play();
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY_SINGLE, animation.getMode());
  animation.pause();
  animation.playRandom();
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY_RANDOM, animation.getMode());
  animation.play();
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY_RANDOM, animation.getMode());
  animation.stop();
  TEST_ASSERT_EQUAL(Animation::MODE_STOP, animation.getMode());
  animation.play();
  TEST_ASSERT_EQUAL(Animation::MODE_STOP, animation.getMode());
  animation.run(10000);
  animation.live(mock);
  TEST_ASSERT_EQUAL(Animation::MODE_LIVE, animation.getMode());
  animation.play();
  TEST_ASSERT_EQUAL(Animation::MODE_LIVE, animation.getMode());
}

void test_allowed(void) {
  Animation animation;
  animation.onPositionChange(move);
  animation.addScene(PROGMEM_DATA, DATA_SIZE, FPS, FRAMES);

  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, animation.getMode());
  animation.play();
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY, animation.getMode());
  animation.pause();
  TEST_ASSERT_EQUAL(Animation::MODE_PAUSE, animation.getMode());
  animation.play();
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY, animation.getMode());
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_play);
  RUN_TEST(test_without_scenes);
  RUN_TEST(test_prevented);
  RUN_TEST(test_allowed);
  UNITY_END();
}
