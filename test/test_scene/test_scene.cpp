#include "../test/helper.h"
#include "internal/ProgmemStream.h"
#include "internal/Scene.h"
#include <unity.h>

using namespace BlenderServoAnimation;

void setUp(void) {
  resetPositionLog();
}

void test_play_with_frames(void) {
  ServoManager servoManager;
  servoManager.setPositionCallback(move);
  ProgmemStream stream(PROGMEM_DATA, DATA_SIZE);
  Scene scene(servoManager, stream, FPS, FRAMES);

  positionLog exp[10] = {
      {0, 375}, {1, 375}, {0, 376}, {1, 376}, {0, 377},
      {1, 379}, {0, 380}, {1, 383}, {0, 384}, {1, 388},
  };

  for (int i = 0; i < ANIMATION_MICROS; i += FRAME_MICROS) {
    scene.play(i);
  }

  TEST_ASSERT_EQUAL(10, logIndex);

  for (byte i = 0; i < 10; i++) {
    TEST_ASSERT_EQUAL(exp[i].servoId, positions[i].servoId);
    TEST_ASSERT_EQUAL(exp[i].position, positions[i].position);
  }
}

void test_play_without_frames(void) {
  ServoManager servoManager;
  servoManager.setPositionCallback(move);
  ProgmemStream stream(PROGMEM_DATA, DATA_SIZE);
  Scene scene(servoManager, stream);

  positionLog exp[10] = {
      {0, 375}, {1, 375}, {0, 376}, {1, 376}, {0, 377},
      {1, 379}, {0, 380}, {1, 383}, {0, 384}, {1, 388},
  };

  scene.play(0);

  TEST_ASSERT_EQUAL(10, logIndex);

  for (byte i = 0; i < 10; i++) {
    TEST_ASSERT_EQUAL(exp[i].servoId, positions[i].servoId);
    TEST_ASSERT_EQUAL(exp[i].position, positions[i].position);
  }
}

void test_stop(void) {
  ServoManager servoManager;
  servoManager.setPositionCallback(move);
  servoManager.setDefaultThreshold(20);
  ProgmemStream stream(PROGMEM_DATA, DATA_SIZE);
  Scene scene(servoManager, stream, FPS, FRAMES);

  positionLog exp[15] = {
      {0, 375},
      {1, 375},
      {0, 376},
      {1, 376},
      {0, 377},
      {1, 379},
      {0, 380},
      {1, 383},
      // stop
      {0, 378},
      {1, 381},
      {0, 376},
      {1, 379},
      {0, 375},
      {1, 377},
      {1, 375},
  };

  for (int i = 0; i < FRAME_MICROS * 4; i += FRAME_MICROS) {
    scene.play(i);
  }

  TEST_ASSERT_FALSE(scene.hasFinished());
  TEST_ASSERT_EQUAL(4, scene.getFrame());
  TEST_ASSERT_EQUAL(8, logIndex);

  for (int i = 0; i < STOP_MICROS * 10; i += STOP_MICROS) {
    scene.stop(i);
  }

  TEST_ASSERT_EQUAL(0, scene.getFrame());
  TEST_ASSERT_EQUAL(15, logIndex);

  for (byte i = 0; i < 15; i++) {
    TEST_ASSERT_EQUAL(exp[i].servoId, positions[i].servoId);
    TEST_ASSERT_EQUAL(exp[i].position, positions[i].position);
  }
}

void test_has_finished(void) {
  ServoManager servoManager;
  ProgmemStream stream(PROGMEM_DATA, DATA_SIZE);
  Scene scene(servoManager, stream, FPS, FRAMES);

  TEST_ASSERT_FALSE(scene.hasFinished());

  for (byte i = 0; i < FRAMES; i++) {
    scene.play(i * FRAME_MICROS);

    if (i == FRAMES - 1) {
      TEST_ASSERT_TRUE(scene.hasFinished());
    } else {
      TEST_ASSERT_FALSE(scene.hasFinished());
    }
  }
}

void test_get_frame(void) {
  ServoManager servoManager;
  ProgmemStream stream(PROGMEM_DATA, DATA_SIZE);
  Scene scene(servoManager, stream, FPS, FRAMES);

  TEST_ASSERT_EQUAL(0, scene.getFrame());

  for (int i = 0; i < FRAMES; i++) {
    scene.play(i * FRAME_MICROS);

    TEST_ASSERT_EQUAL(i + 1, scene.getFrame());
  }
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_play_with_frames);
  RUN_TEST(test_play_without_frames);
  RUN_TEST(test_stop);
  RUN_TEST(test_has_finished);
  RUN_TEST(test_get_frame);
  UNITY_END();
}