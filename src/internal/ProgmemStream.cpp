#include "ProgmemStream.h"
#include <Arduino.h>

using namespace BlenderServoAnimation;

ProgmemStream::ProgmemStream(const byte *data, size_t size) {
  this->data = data;
  this->size = size;
}

int ProgmemStream::read() {
  if (this->position < this->size) {
    return pgm_read_byte(this->data + this->position++);
  } else {
    return -1;
  }
}

int ProgmemStream::available() {
  return size - position;
}

int ProgmemStream::peek() {
  if (position < size) {
    return pgm_read_byte(this->data + this->position);
  } else {
    return -1;
  }
}

size_t ProgmemStream::write(uint8_t value) {
  return 0;
}

void ProgmemStream::flush() {
  this->position = 0;
}
