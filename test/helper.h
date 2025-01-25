#include <Arduino.h>

#define FPS 30
#define FRAMES 5
#define FRAME_MICROS 33333
#define ANIMATION_MICROS 166665
#define STOP_MICROS 10000
#define LOG_SIZE 20
#define DATA_SIZE 55

struct positionLog {
  byte servoId = 0;
  int position = 0;
};

positionLog positions[LOG_SIZE];

byte logIndex = 0;

void resetPositionLog(void) {
  for (int i = 0; i < LOG_SIZE; i++) {
    positions[i].servoId = 0;
    positions[i].position = 0;
  }

  logIndex = 0;
}

void move(byte servoID, int position) {
  positions[logIndex].servoId = servoID;
  positions[logIndex].position = position;
  logIndex++;
}

const byte PROGMEM PROGMEM_DATA[DATA_SIZE] = {
    0x3c, 0x00, 0x01, 0x77, 0x3e, 0x3c, 0x01, 0x01, 0x77, 0x3e, 0x0a, 0x3c, 0x00, 0x01,
    0x78, 0x3e, 0x3c, 0x01, 0x01, 0x78, 0x3e, 0x0a, 0x3c, 0x00, 0x01, 0x79, 0x3e, 0x3c,
    0x01, 0x01, 0x7b, 0x3e, 0x0a, 0x3c, 0x00, 0x01, 0x7c, 0x3e, 0x3c, 0x01, 0x01, 0x7f,
    0x3e, 0x0a, 0x3c, 0x00, 0x01, 0x80, 0x3e, 0x3c, 0x01, 0x01, 0x84, 0x3e, 0x0a,
};

class StreamMock : public Stream {
public:
  int availableValue = 1;

  int available() {
    return this->availableValue;
  }

  int read() {
    return 123;
  }

  int peek() {
    return 1;
  }

  void flush() {
  }

  size_t write(uint8_t) {
    return 1;
  }
};
