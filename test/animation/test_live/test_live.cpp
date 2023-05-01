#include "../helper.h"
#include "BlenderServoAnimation.h"
#include <unity.h>

using namespace BlenderServoAnimation;

void setUp(void) {
  setUpHelper();
}

const int positions[5] PROGMEM = {350, 340, 330, 340, 330};

void test_live(void) {
  Animation animation;
  LiveStream mock;
  Servo servos[] = {
      Servo(0, positions, move),
      Servo(1, move),
  };
  animation.addServos(servos, 2);
  animation.live(mock);
  TEST_ASSERT_EQUAL(Animation::MODE_LIVE, animation.getMode());

  byte values[20] = {60, 0, 1, 94, 62, 60, 1, 1, 94, 62,
                     60, 0, 1, 99, 62, 60, 1, 1, 99, 62};

  for (int i = 0; i < 10; i++) {
    mock.write(values[i]);
  }

  animation.run(0);

  TEST_ASSERT_EQUAL(0, lastPositions[0].positions[0]);
  TEST_ASSERT_EQUAL(350, lastPositions[1].positions[0]);

  for (int i = 10; i < 20; i++) {
    mock.write(values[i]);
  }

  animation.run(0);

  TEST_ASSERT_EQUAL(355, lastPositions[0].positions[0]);
  TEST_ASSERT_EQUAL(355, lastPositions[1].positions[1]);
}

void test_skip_incomplete_command(void) {
  Animation animation;
  LiveStream mock;
  Servo servo(0, move);
  animation.addServo(servo);
  animation.live(mock);
  TEST_ASSERT_EQUAL(Animation::MODE_LIVE, animation.getMode());

  byte values[15] = {60, 0, 1, 94, 62, 60, 0, 1, 99, 0, 60, 0, 1, 104, 62};

  for (int i = 0; i < 15; i++) {
    mock.write(values[i]);
  }

  animation.run(0);

  TEST_ASSERT_EQUAL(350, lastPositions[0].positions[0]);
  TEST_ASSERT_EQUAL(360, lastPositions[0].positions[1]);
}

void test_call_twice(void) {
  Serial_ mock;
  Animation animation(FPS, FRAMES);
  animation.onModeChange(onModeChange);

  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, animation.getMode());
  animation.live(mock);
  TEST_ASSERT_EQUAL(Animation::MODE_LIVE, animation.getMode());
  animation.live(mock);
  TEST_ASSERT_EQUAL(Animation::MODE_LIVE, animation.getMode());

  TEST_ASSERT_EQUAL(1, modeChangeCount);
}

void test_prevented(void) {
  Serial_ mock;
  Animation animation(FPS, FRAMES);

  animation.play(0);
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY, animation.getMode());
  animation.live(mock);
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY, animation.getMode());
  animation.pause();
  TEST_ASSERT_EQUAL(Animation::MODE_PAUSE, animation.getMode());
  animation.live(mock);
  TEST_ASSERT_EQUAL(Animation::MODE_PAUSE, animation.getMode());
  animation.loop(0);
  TEST_ASSERT_EQUAL(Animation::MODE_LOOP, animation.getMode());
  animation.live(mock);
  TEST_ASSERT_EQUAL(Animation::MODE_LOOP, animation.getMode());
  animation.stop(0);
  TEST_ASSERT_EQUAL(Animation::MODE_STOP, animation.getMode());
  animation.live(mock);
  TEST_ASSERT_EQUAL(Animation::MODE_STOP, animation.getMode());
}

void test_allowed(void) {
  Serial_ mock;
  Animation animation(FPS, FRAMES);

  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, animation.getMode());
  animation.live(mock);
  TEST_ASSERT_EQUAL(Animation::MODE_LIVE, animation.getMode());
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_live);
  RUN_TEST(test_skip_incomplete_command);
  RUN_TEST(test_call_twice);
  RUN_TEST(test_prevented);
  RUN_TEST(test_allowed);
  UNITY_END();
}
