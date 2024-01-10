#include "internal/LiveStream.h"
#include <unity.h>

using namespace BlenderServoAnimation;

void test_read_write(void) {
  LiveStream stream;

  TEST_ASSERT_FALSE(stream.available());

  for (int i = 0; i < 200; i += 20) {
    for (int x = i; x < i + 20; x++) {
        TEST_ASSERT_EQUAL(1, stream.write(x));
    }

    TEST_ASSERT_EQUAL(20, stream.available());
    TEST_ASSERT_EQUAL(i, stream.peek());

    for (int x = i; x < i + 20; x++) {
        TEST_ASSERT_EQUAL(x, stream.read());
    }
  }

  TEST_ASSERT_EQUAL(0, stream.available());
  TEST_ASSERT_EQUAL(-1, stream.peek());
}

void test_flush(void) {
  byte values[5] = {60, 3, 1, 119, 62};
  LiveStream stream;

  for (int i = 0; i < 5; i++) {
    TEST_ASSERT_EQUAL(1, stream.write(values[i]));
  }

  stream.flush();

  for (int i = 0; i < 5; i++) {
    TEST_ASSERT_EQUAL(0, stream.read());
  }
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_read_write);
  RUN_TEST(test_flush);
  UNITY_END();
}