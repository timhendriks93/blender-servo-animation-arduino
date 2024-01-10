#include "internal/ProgmemStream.h"
#include <unity.h>

using namespace BlenderServoAnimation;

const byte PROGMEM values[5] = {60, 3, 1, 119, 62};

void test_read(void) {
  ProgmemStream stream(values, 5);

  TEST_ASSERT_EQUAL(5, stream.available());
  TEST_ASSERT_EQUAL(60, stream.peek());

  byte exp[5] = {60, 3, 1, 119, 62};

  for (int i = 0; i < 5; i++) {
    TEST_ASSERT_EQUAL(exp[i], stream.read());
  }

  TEST_ASSERT_EQUAL(0, stream.available());
  TEST_ASSERT_EQUAL(-1, stream.peek());
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_read);
  UNITY_END();
}