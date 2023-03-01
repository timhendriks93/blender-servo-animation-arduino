#include "show/Show.h"
#include "animation/Animation.h"
#include <Arduino.h>

using namespace BlenderServoAnimation;

byte Show::countAnimations() {
  return this->addIndex;
}

bool Show::hasAnimations() {
  return this->countAnimations() > 0;
}

bool Show::hasAnimation(byte id) {
  for (int i = 0; i < this->addIndex; i++) {
    if (this->animations[i] && this->animations[i]->getID() == id) {
      return true;
    }
  }

  return false;
}

void Show::addAnimation(Animation &animation) {
  this->animations[this->addIndex] = &animation;
  this->addIndex++;

  if (!this->animation) {
    this->animation = &animation;
  }
}

void Show::addAnimations(Animation animations[], byte animationAmount) {
  for (int i = 0; i < animationAmount; i++) {
    this->addAnimation(animations[i]);
  }
}

void Show::play(unsigned long currentMicros) {
  if (this->animation) {
    this->animation->play(currentMicros);
  }
}

void Show::reset() {
  this->addIndex = 0;
  this->playIndex = 0;

  for (int i = 0; i < this->addIndex; i++) {
    this->animations[i] = nullptr;
  }
}

byte Show::getMode() {
  if (this->animation) {
    return this->animation->getMode();
  }
  
  return Animation::MODE_DEFAULT;
}

void Show::run(unsigned long currentMicros) {
  if (!this->animation) {
    return;
  }

  bool lastFrame = false;

  if (this->animation->getFrames() - this->animation->getFrame() == 1) {
    lastFrame = true;
  }

  animation->run(currentMicros);

  if (!lastFrame) {
    return;
  }

  if (this->playIndex + 1 >= this->addIndex) {
    this->animation = nullptr;
    return;
  }

  this->playIndex++;
  this->animation = this->animations[this->playIndex];
  this->animation->play(currentMicros);
}

Animation* Show::getCurrentAnimation() {
  return this->animation;
}
