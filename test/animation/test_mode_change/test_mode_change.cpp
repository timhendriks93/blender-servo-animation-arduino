#include "BlenderServoAnimation.h"
#include "../../SerialMock.h"
#include <unity.h>

using namespace BlenderServoAnimation;

#define FPS 60
#define FRAMES 5

struct modeLog {
  byte prevMode;
  byte newMode;
};

byte modeIndex = 0;
modeLog lastModes[10];

void setUp(void) {
  for (int i = 0; i < 10; i++) {
    lastModes[i].prevMode = 0;
    lastModes[i].newMode = 0;
  }

  modeIndex = 0;
}

void onModeChange(byte prevMode, byte newMode) {
  lastModes[modeIndex].prevMode = prevMode;
  lastModes[modeIndex].newMode = newMode;
  modeIndex++;
}

void test_different_mode(void) {
  Animation animation(FPS, FRAMES);

  animation.onModeChange(onModeChange);

  animation.play();
  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, lastModes[0].prevMode);
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY, lastModes[0].newMode);
  animation.pause();
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY, lastModes[1].prevMode);
  TEST_ASSERT_EQUAL(Animation::MODE_PAUSE, lastModes[1].newMode);
}

void test_same_mode(void) {
  Animation animation(FPS, FRAMES);

  animation.onModeChange(onModeChange);

  animation.loop();
  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, lastModes[0].prevMode);
  TEST_ASSERT_EQUAL(Animation::MODE_LOOP, lastModes[0].newMode);
  animation.loop();
  TEST_ASSERT_EQUAL(Animation::MODE_LOOP, lastModes[1].prevMode);
  TEST_ASSERT_EQUAL(Animation::MODE_LOOP, lastModes[1].newMode);
}

void test_all_modes(void) {
  SerialMock mock;
  Animation animation(FPS, FRAMES);

  animation.onModeChange(onModeChange);

  animation.play();
  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, lastModes[0].prevMode);
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY, lastModes[0].newMode);
  animation.pause();
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY, lastModes[1].prevMode);
  TEST_ASSERT_EQUAL(Animation::MODE_PAUSE, lastModes[1].newMode);
  animation.loop();
  TEST_ASSERT_EQUAL(Animation::MODE_PAUSE, lastModes[2].prevMode);
  TEST_ASSERT_EQUAL(Animation::MODE_LOOP, lastModes[2].newMode);
  animation.stop();
  TEST_ASSERT_EQUAL(Animation::MODE_LOOP, lastModes[3].prevMode);
  TEST_ASSERT_EQUAL(Animation::MODE_STOP, lastModes[3].newMode);
  animation.run();
  TEST_ASSERT_EQUAL(Animation::MODE_STOP, lastModes[4].prevMode);
  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, lastModes[4].newMode);
  animation.live(mock);
  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, lastModes[5].prevMode);
  TEST_ASSERT_EQUAL(Animation::MODE_LIVE, lastModes[5].newMode);
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_different_mode);
  RUN_TEST(test_same_mode);
  RUN_TEST(test_all_modes);
  UNITY_END();
}
