#include "../SerialMock.h"
#include "BlenderServoAnimation.h"
#include <unity.h>

using namespace BlenderServoAnimation;

#define FPS 60
#define FRAMES 5
#define FRAME_MICROS 16667

struct positionLog {
  int index;
  int positions[20];
};

positionLog lastPositions[16];

struct modeLog {
  byte prevMode;
  byte newMode;
};

byte modeIndex = 0;
modeLog lastModes[10];

void setUp(void) {
  for (int id = 0; id < 16; id++) {
    lastPositions[id].index = 0;

    for (int i = 0; i < 20; i++) {
      lastPositions[id].positions[i] = 0;
    }
  }

  for (int i = 0; i < 10; i++) {
    lastModes[i].prevMode = 0;
    lastModes[i].newMode = 0;
  }

  modeIndex = 0;
}

void move(byte servoID, int position) {
  int index = lastPositions[servoID].index;
  lastPositions[servoID].positions[index] = position;
  lastPositions[servoID].index++;
}

void onModeChange(byte prevMode, byte newMode) {
  lastModes[modeIndex].prevMode = prevMode;
  lastModes[modeIndex].newMode = newMode;
  modeIndex++;
}

const int positionsA[5] PROGMEM = {350, 340, 330, 340, 330};
const int positionsB[5] PROGMEM = {250, 240, 230, 240, 230};

void test_play(void) {
  Animation animation(FPS, FRAMES);
  Servo servos[] = {
      Servo(1, positionsA, move),
      Servo(2, positionsB, move),
      Servo(3, move),
  };
  animation.addServos(servos, 3);
  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, animation.getMode());
  animation.play();
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY, animation.getMode());

  int expA[9] = {340, 330, 340, 330, 350, 340, 330, 340, 330};
  int expB[9] = {240, 230, 240, 230, 250, 240, 230, 240, 230};

  for (long i = 0; i < FRAME_MICROS * (long)9; i++) {
    animation.run(i);
  }

  for (int i = 0; i < 9; i++) {
    TEST_ASSERT_EQUAL(expA[i], lastPositions[1].positions[i]);
    TEST_ASSERT_EQUAL(expB[i], lastPositions[2].positions[i]);
    TEST_ASSERT_EQUAL(0, lastPositions[3].positions[i]);
  }
}

void test_pause(void) {
  Animation animation(FPS, FRAMES);
  Servo servo(2, positionsA, move);
  animation.addServo(servo);
  animation.play();
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY, animation.getMode());

  int exp[9] = {340, 330, 340, 330, 350, 340, 330, 340, 330};

  for (int i = 0; i < 4; i++) {
    animation.run(FRAME_MICROS * (long)(i + 1));
    TEST_ASSERT_EQUAL(exp[i], lastPositions[2].positions[i]);
  }

  animation.pause();
  TEST_ASSERT_EQUAL(Animation::MODE_PAUSE, animation.getMode());

  for (int i = 4; i < 8; i++) {
    animation.run(FRAME_MICROS * (long)(i + 1));
    TEST_ASSERT_EQUAL(0, lastPositions[2].positions[i]);
  }

  animation.play();
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY, animation.getMode());

  for (int i = 8; i < 14; i++) {
    animation.run(FRAME_MICROS * (long)(i + 1));
    TEST_ASSERT_EQUAL(exp[i - 5], lastPositions[2].positions[i - 5]);
  }
}

void test_stop(void) {
  Animation animation(FPS, FRAMES);
  Servo servos[] = {
      Servo(0, positionsA, move),
      Servo(1, positionsB, move),
      Servo(2, move),
  };
  animation.addServos(servos, 3);
  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, animation.getMode());
  animation.play();
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY, animation.getMode());
  animation.run(FRAME_MICROS);
  TEST_ASSERT_EQUAL(340, lastPositions[0].positions[0]);
  TEST_ASSERT_EQUAL(240, lastPositions[1].positions[0]);
  TEST_ASSERT_EQUAL(0, lastPositions[2].positions[0]);
  TEST_ASSERT_EQUAL(1, animation.getFrame());
  animation.stop(0);

  for (int i = 0; i < 10; i++) {
    animation.run(FRAME_MICROS * (long)(i + 2));
    TEST_ASSERT_EQUAL(341 + i, lastPositions[0].positions[i + 1]);
    TEST_ASSERT_EQUAL(241 + i, lastPositions[1].positions[i + 1]);
    TEST_ASSERT_EQUAL(0, lastPositions[2].positions[i + 1]);
    TEST_ASSERT_EQUAL(Animation::MODE_STOP, animation.getMode());
  }

  animation.run(FRAME_MICROS * (long)12);
  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, animation.getMode());
  TEST_ASSERT_EQUAL(0, animation.getFrame());
}

void test_live(void) {
  Animation animation;
  SerialMock mock;
  Servo servos[] = {
      Servo(0, positionsA, move),
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

  animation.run();

  TEST_ASSERT_EQUAL(0, lastPositions[0].positions[0]);
  TEST_ASSERT_EQUAL(350, lastPositions[1].positions[0]);

  for (int i = 10; i < 20; i++) {
    mock.write(values[i]);
  }

  animation.run();

  TEST_ASSERT_EQUAL(355, lastPositions[0].positions[0]);
  TEST_ASSERT_EQUAL(355, lastPositions[1].positions[1]);
}

void test_mode_change(void) {
  SerialMock mock;
  Animation animation(FPS, FRAMES);

  animation.onModeChange(onModeChange);

  animation.play();
  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, lastModes[0].prevMode);
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY, lastModes[0].newMode);
  animation.pause();
  TEST_ASSERT_EQUAL(Animation::MODE_PLAY, lastModes[1].prevMode);
  TEST_ASSERT_EQUAL(Animation::MODE_PAUSE, lastModes[1].newMode);
  animation.stop();
  TEST_ASSERT_EQUAL(Animation::MODE_PAUSE, lastModes[2].prevMode);
  TEST_ASSERT_EQUAL(Animation::MODE_STOP, lastModes[2].newMode);
  animation.run();
  TEST_ASSERT_EQUAL(Animation::MODE_STOP, lastModes[3].prevMode);
  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, lastModes[3].newMode);
  animation.live(mock);
  TEST_ASSERT_EQUAL(Animation::MODE_DEFAULT, lastModes[4].prevMode);
  TEST_ASSERT_EQUAL(Animation::MODE_LIVE, lastModes[4].newMode);
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_play);
  RUN_TEST(test_pause);
  RUN_TEST(test_stop);
  RUN_TEST(test_live);
  RUN_TEST(test_mode_change);
  UNITY_END();
}
