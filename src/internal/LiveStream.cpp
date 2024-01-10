#include "LiveStream.h"
#include <Arduino.h>

using namespace BlenderServoAnimation;

int LiveStream::available() {
  if (this->writeIndex >= this->readIndex) {
    return this->writeIndex - this->readIndex;
  } else {
    return BUFFER_SIZE - this->readIndex + this->writeIndex;
  }
}

int LiveStream::peek() {
  if (this->available()) {
    return this->buffer[this->readIndex];
  } else {
    return -1;
  } 
}

int LiveStream::read() {
  int value = this->buffer[this->readIndex++];

  if (this->readIndex >= BUFFER_SIZE) {
    this->readIndex = 0;
  }

  return value;
}

size_t LiveStream::write(uint8_t value) {
  this->buffer[this->writeIndex++] = value;

  if (this->writeIndex >= BUFFER_SIZE) {
    this->writeIndex = 0;
  }

  return 1;
}

void LiveStream::flush() {
  for (int i = 0; i < BUFFER_SIZE; i++) {
    this->buffer[i] = 0;
  }

  this->readIndex = 0;
  this->writeIndex = 0;
}
