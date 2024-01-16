#include "../test/helper.h"
#include "internal/Animation.h"

#include <unity.h>

using namespace BlenderServoAnimation;

int prevSceneIndex = -1;
int nextSceneIndex = -1;

void setUp(void) {
  prevSceneIndex = -1;
  nextSceneIndex = -1;
}

void onSceneChange(byte prevArg, byte newArg) {
  prevSceneIndex = prevArg;
  nextSceneIndex = newArg;
}

void test_play(void) {
  Animation animation;
  animation.onSceneChange(onSceneChange);
  animation.addScene(PROGMEM_DATA, DATA_SIZE, FPS, FRAMES);
  animation.addScene(PROGMEM_DATA, DATA_SIZE, FPS, FRAMES);

  TEST_ASSERT_EQUAL(-1, prevSceneIndex);
  TEST_ASSERT_EQUAL(-1, nextSceneIndex);

  animation.play();

  TEST_ASSERT_EQUAL(0, prevSceneIndex);
  TEST_ASSERT_EQUAL(0, nextSceneIndex);

  for (int i = 0; i < ANIMATION_MICROS; i += FRAME_MICROS) {
    animation.run(i);
  }

  TEST_ASSERT_EQUAL(0, prevSceneIndex);
  TEST_ASSERT_EQUAL(1, nextSceneIndex);
}

void test_play_single(void) {
  Animation animation;
  animation.onSceneChange(onSceneChange);
  animation.addScene(PROGMEM_DATA, DATA_SIZE, FPS, FRAMES);
  animation.addScene(PROGMEM_DATA, DATA_SIZE, FPS, FRAMES);

  TEST_ASSERT_EQUAL(-1, prevSceneIndex);
  TEST_ASSERT_EQUAL(-1, nextSceneIndex);

  animation.playSingle(1);

  TEST_ASSERT_EQUAL(0, prevSceneIndex);
  TEST_ASSERT_EQUAL(1, nextSceneIndex);

  for (int i = 0; i < ANIMATION_MICROS; i += FRAME_MICROS) {
    animation.run(i);
  }

  TEST_ASSERT_EQUAL(1, prevSceneIndex);
  TEST_ASSERT_EQUAL(0, nextSceneIndex);
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_play);
  RUN_TEST(test_play_single);
  UNITY_END();
}
