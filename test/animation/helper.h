#include <Arduino.h>

#define FPS 60
#define FRAMES 5
#define FRAME_MICROS 16667

byte modeChangeCount = 0;

struct positionLog {
  int index;
  int positions[20];
};

positionLog lastPositions[16];

void setUpHelper(void) {
  for (int id = 0; id < 16; id++) {
    lastPositions[id].index = 0;

    for (int i = 0; i < 20; i++) {
      lastPositions[id].positions[i] = 0;
    }
  }

  modeChangeCount = 0;
}

void onModeChange(byte prevMode, byte newMode) {
  modeChangeCount++;
}

void move(byte servoID, int position) {
  int index = lastPositions[servoID].index;
  lastPositions[servoID].positions[index] = position;
  lastPositions[servoID].index++;
}