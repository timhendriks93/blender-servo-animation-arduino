#include "Servo.h"
#include "typedefs.h"
#include <Arduino.h>

using BlenderServoAnimation::Servo;

Servo::Servo(byte id, pcb positionCallback, byte threshold) {
  this->id = id;
  this->positionCallback = positionCallback;
  this->setThreshold(threshold);
}

void Servo::move(int position, bool useOffset) {
  if (useOffset && this->offset != 0) {
    position += this->offset;
  }

  if (position == this->currentPosition ||
      this->positionExceedsThreshold(position)) {
    return;
  }

  this->positionCallback(this->id, position);
  this->currentPosition = position;

  if (this->neutralPosition == -1) {
    this->neutralPosition = this->currentPosition;
  }
}

void Servo::moveTowardsNeutral() {
  if (this->neutralPosition == -1) {
    return;
  }

  if (this->threshold == 0) {
    return this->move(this->neutralPosition, false);
  }

  int newPosition = this->currentPosition;

  if (abs(this->neutralPosition - newPosition) < this->step) {
    newPosition = this->neutralPosition;
  } else if (this->currentPosition > this->neutralPosition) {
    newPosition -= this->step;
  } else if (this->currentPosition < this->neutralPosition) {
    newPosition += this->step;
  }

  this->move(newPosition, false);
}

bool Servo::isNeutral() {
  return this->currentPosition == this->neutralPosition;
}

void Servo::setPositionCallback(pcb positionCallback) {
  this->positionCallback = positionCallback;
}

void Servo::setThreshold(byte value) {
  this->threshold = value;
  this->step = round(value / STEP_DIVIDER);
}

void Servo::setOffset(int offset) {
  this->offset = offset;
}

bool Servo::positionExceedsThreshold(int position) {
  if (this->currentPosition == -1) {
    return false;
  }

  int positionDiff = abs(position - this->currentPosition);

  return this->threshold && positionDiff > this->threshold;
}

byte Servo::getId() {
  return this->id;
}
