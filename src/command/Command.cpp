#include "command/Command.h"
#include <Arduino.h>

using namespace BlenderServoAnimation;

void Command::read(Stream *liveStream) {
  byte receivedByte = liveStream->read();

  if (!this->receiving && receivedByte != START_MARKER) {
    return;
  }

  switch (this->receivedIndex) {
  case 0: // Start marker
    this->receiving = true;
    this->complete = false;
    break;
  case LENGTH - 1: // End marker
    this->receiving = false;
    this->complete = true;
    break;
  }

  this->receivedBytes[receivedIndex] = receivedByte;
  this->receivedIndex++;

  if (complete) {
    this->receivedIndex = 0;
  }
}

bool Command::isValid() {
  return this->receivedBytes[0] == START_MARKER &&
         this->receivedBytes[LENGTH - 1] == END_MARKER;
}

bool Command::isComplete() {
  return this->complete;
}

byte Command::getServoID() {
  return this->receivedBytes[INDEX_SERVO_ID];
}

int Command::getServoPosition() {
  return this->receivedBytes[INDEX_POSITION_BYTE] |
         this->receivedBytes[INDEX_POSITION_SIG_BYTE] << BITS;
}