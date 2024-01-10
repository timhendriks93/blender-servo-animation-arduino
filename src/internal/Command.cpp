#include "Command.h"
#include <Arduino.h>

using namespace BlenderServoAnimation;

void Command::write(byte value) {
  if (value == START_MARKER) {
    this->reset();
  }

  if (this->index >= LENGTH) {
    return;
  }

  this->values[this->index] = value;
  this->index++;
}

bool Command::isValid() {
  return this->values[INDEX_START_MARKER] == START_MARKER &&
         this->values[INDEX_END_MARKER] == END_MARKER;
}

byte Command::getServoID() {
  return this->values[INDEX_SERVO_ID];
}

int Command::getServoPosition() {
  return this->values[INDEX_POSITION_BYTE] |
         this->values[INDEX_POSITION_SIG_BYTE] << BITS;
}

void Command::reset() {
  for (byte i = 0; i < LENGTH; i++) {
    this->values[i] = 0;
  }

  this->index = 0;
}