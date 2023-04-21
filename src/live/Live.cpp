#include "Live.h"
#include <Arduino.h>

using namespace BlenderServoAnimation;

int Live::available() {
  return this->readIndex != this->writeIndex;
}

int Live::peek() {
  return this->buffer[this->readIndex];
}

int Live::read() {
  int value = this->buffer[this->readIndex++];

  if (this->readIndex >= BUFFER_SIZE) {
    this->readIndex = 0;
  }

  return value;
}

size_t Live::write(uint8_t value) {
  this->buffer[this->writeIndex++] = value;

  if (this->writeIndex >= BUFFER_SIZE) {
    this->writeIndex = 0;
  }

  return 1;
}

void Live::flush() {
  for (int i = 0; i < BUFFER_SIZE; i++) {
    this->buffer[i] = 0;
  }

  this->readIndex = 0;
  this->writeIndex = 0;
}
