#include "../test/helper.h"
#include "AnimationData.h"
#include <unity.h>

using BlenderServoAnimationLibrary::AnimationData;

const byte PROGMEM values[5] = {60, 3, 1, 119, 62};

void test_progmem_data(void) {
  AnimationData data(values, 5);
  TEST_ASSERT_TRUE(data.isAvailable());

  for (byte i = 0; i < 5; i++) {
    TEST_ASSERT_EQUAL(values[i], data.getNextByte());
  }

  TEST_ASSERT_FALSE(data.isAvailable());
}

void test_stream_data(void) {
  StreamMock stream;
  AnimationData data(&stream);
  TEST_ASSERT_TRUE(data.isAvailable());
  TEST_ASSERT_EQUAL(123, data.getNextByte());
}

void test_buffer_data(void) {
  AnimationData data;
  TEST_ASSERT_FALSE(data.isAvailable());

  for (byte i = 0; i < 5; i++) {
    data.writeByte(values[i]);
  }

  TEST_ASSERT_TRUE(data.isAvailable());

  for (byte i = 0; i < 5; i++) {
    TEST_ASSERT_EQUAL(values[i], data.getNextByte());
  }

  TEST_ASSERT_FALSE(data.isAvailable());
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_progmem_data);
  RUN_TEST(test_stream_data);
  RUN_TEST(test_buffer_data);
  UNITY_END();
}
