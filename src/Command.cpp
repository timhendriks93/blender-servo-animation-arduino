#include "Command.h"
#include <Arduino.h>

using BlenderServoAnimationLibrary::Command;

void Command::write(byte value) {
  if (this->index >= LENGTH) {
    this->reset();
  }

  if (this->index == 0 && value != START_MARKER) {
    return;
  }

  this->values[this->index] = value;
  this->index++;
}

bool Command::isValid() {
  return this->values[INDEX_START_MARKER] == START_MARKER &&
         this->values[INDEX_END_MARKER] == END_MARKER;
}

bool Command::isReading() {
  return this->index > 0 && this->index < LENGTH;
}

byte Command::getServoID() {
  return this->values[INDEX_SERVO_ID];
}

int Command::getServoPosition() {
  return this->values[INDEX_POSITION_BYTE] | this->values[INDEX_POSITION_SIG_BYTE] << BITS;
}

void Command::reset() {
  for (byte i = 0; i < LENGTH; i++) {
    this->values[i] = 0;
  }

  this->index = 0;
}