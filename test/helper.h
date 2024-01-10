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
  60, 0, 1, 119, 62, 60, 1, 1, 119, 62, 10,
  60, 0, 1, 120, 62, 60, 1, 1, 120, 62, 10,
  60, 0, 1, 121, 62, 60, 1, 1, 123, 62, 10,
  60, 0, 1, 124, 62, 60, 1, 1, 127, 62, 10,
  60, 0, 1, 128, 62, 60, 1, 1, 132, 62, 10,
};
