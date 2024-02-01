#include "AnimationData.h"
#include <Arduino.h>

using namespace BlenderServoAnimation;

AnimationData::AnimationData() {
}

AnimationData::AnimationData(const byte *data, int dataSize) {
  this->data = data;
  this->dataSize = dataSize;
}

AnimationData::AnimationData(Stream *stream) {
  this->stream = stream;
}

bool AnimationData::isAvailable() {
  if (this->stream) {
    return this->stream->available() > 0;
  } else if (this->data) {
    return this->dataSize - this->dataPosition > 0;
  } else {
    return this->readIndex != this->writeIndex;
  }
}

byte AnimationData::getNextByte() {
  if (this->stream) {
    return this->stream->read();
  } else if (this->data) {
    return this->readProgmem();
  } else {
    return this->readBuffer();
  }
}

byte AnimationData::readProgmem() {
  if (this->dataPosition < this->dataSize) {
    return pgm_read_byte(this->data + this->dataPosition++);
  } else {
    return -1;
  }
}

byte AnimationData::readBuffer() {
  byte value = this->buffer[this->readIndex++];

  if (this->readIndex >= BUFFER_SIZE) {
    this->readIndex = 0;
  }

  return value;
}

void AnimationData::writeByte(byte value) {
  this->buffer[this->writeIndex++] = value;

  if (this->writeIndex >= BUFFER_SIZE) {
    this->writeIndex = 0;
  }
}

void AnimationData::reset() {
  if (this->data) {
    this->dataPosition = 0;
  } else {
    this->readIndex = 0;
    this->writeIndex = 0;
  }
}
