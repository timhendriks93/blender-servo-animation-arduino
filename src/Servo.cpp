#include "Servo.h"
#include <Arduino.h>

using namespace BlenderServoAnimation;

Servo::Servo(byte id, const int positions[], cb moveCallback, byte threshold) {
  this->id = id;
  this->positions = positions;
  this->moveCallback = moveCallback;
  this->threshold = threshold;
  this->neutralPosition = pgm_read_word_near(positions + 0);
  this->currentPosition = this->neutralPosition;
}

Servo::Servo(byte id, cb moveCallback, byte threshold) {
  this->id = id;
  this->moveCallback = moveCallback;
  this->threshold = threshold;
}

void Servo::move(int position, bool force) {
  if (position == this->currentPosition && force == false) {
    return;
  }

  int positionDiff = abs(position - this->currentPosition);
  bool exceedsThreshold = positionDiff > this->threshold;

  if (this->currentPosition > 0 && exceedsThreshold) {
    return;
  }

  this->moveCallback(this->id, position);
  this->currentPosition = position;
}

void Servo::moveByFrame(int frame) {
  int newPosition = pgm_read_word_near(this->positions + frame);
  this->move(newPosition);
}

void Servo::moveTowardsNeutral(bool inSteps) {
  int newPosition = this->currentPosition;

  if (inSteps == false) {
    this->move(newPosition, true);
    return;
  }

  if (this->currentPosition > this->neutralPosition) {
    newPosition--;
  } else if (this->currentPosition < this->neutralPosition) {
    newPosition++;
  }

  this->move(newPosition);
}

bool Servo::isNeutral() {
  return this->currentPosition == this->neutralPosition;
}

bool Servo::hasPositions() {
  return this->positions != nullptr;
}

byte Servo::getID() {
  return this->id;
}
