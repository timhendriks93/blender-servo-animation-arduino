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
  return this->getAnimationIndex(id) > -1;
}

void Show::addAnimation(Animation &animation) {
  this->animations[this->addIndex] = &animation;
  this->addIndex++;
}

void Show::addAnimations(Animation animations[], byte animationAmount) {
  for (int i = 0; i < animationAmount; i++) {
    this->addAnimation(animations[i]);
  }
}

int Show::getAnimationIndex(byte id) {
  for (int i = 0; i < this->addIndex; i++) {
    Animation *animation = this->animations[i];

    if (animation && animation->getID() == id) {
      return i;
    }
  }

  return -1;
}

void Show::setRandomAnimation() {
  byte randomIndex = random(this->addIndex - 1);
  this->playIndex = randomIndex;
  this->animation = this->animations[this->playIndex];
}

void Show::play(unsigned long currentMicros) {
  if (!this->hasAnimations()) {
    return;
  }

  if (!this->animation) {
    this->animation = this->animations[this->playIndex];
  }

  this->animation->play(currentMicros);
  this->changeMode(MODE_PLAY);
}

void Show::playSingle(byte id, unsigned long currentMicros) {
  int animationIndex = this->getAnimationIndex(id);

  if (animationIndex < 0) {
    return;
  }

  if (!this->animation || this->animation->getFrame() == 0) {
    this->playIndex = animationIndex;
    this->animation = this->animations[this->playIndex];
  }

  this->animation->play(currentMicros);
  this->changeMode(MODE_PLAY_SINGLE);
}

void Show::playRandom(unsigned long currentMicros) {
  if (!this->hasAnimations()) {
    return;
  }

  if (!this->animation || this->animation->getFrame() == 0) {
    this->setRandomAnimation();
  }

  this->animation->play(currentMicros);
  this->changeMode(MODE_PLAY_RANDOM);
}

void Show::loop(unsigned long currentMicros) {
  if (!this->hasAnimations()) {
    return;
  }

  if (!this->animation) {
    this->animation = this->animations[this->playIndex];
  }

  this->animation->play(currentMicros);
  this->changeMode(MODE_LOOP);
}

void Show::pause() {
  if (!this->animation) {
    return;
  }

  this->animation->pause();
  this->changeMode(Show::MODE_PAUSE);
}

void Show::stop(byte stepDelay) {
  if (!this->animation) {
    return;
  }

  this->animation->stop(stepDelay);
  this->changeMode(MODE_STOP);
}

void Show::reset() {
  this->addIndex = 0;
  this->playIndex = 0;

  for (int i = 0; i < this->addIndex; i++) {
    this->animations[i] = nullptr;
  }
}

byte Show::getMode() {
  return this->mode;
}

void Show::run(unsigned long currentMicros) {
  switch (this->mode) {
  case MODE_PLAY:
  case MODE_PLAY_SINGLE:
  case MODE_PLAY_RANDOM:
  case MODE_LOOP:
    this->handlePlayMode(currentMicros);
    break;
  case MODE_STOP:
    this->handleStopMode();
    break;
  }
}

void Show::handlePlayMode(unsigned long currentMicros) {
  if (!this->animation) {
    return;
  }

  this->animation->run(currentMicros);

  if (this->animation->getMode() == Animation::MODE_PLAY) {
    return;
  }

  bool lastAnimation = this->playIndex + 1 >= this->addIndex;

  switch (this->mode) {
  case MODE_PLAY:
    if (lastAnimation) {
      this->animation = nullptr;
    } else {
      this->playIndex++;
      this->animation = this->animations[this->playIndex];
    }
    break;
  case MODE_PLAY_SINGLE:
    this->animation = nullptr;
    break;
  case MODE_PLAY_RANDOM:
    this->setRandomAnimation();
    this->changeMode(MODE_PLAY_RANDOM);
    break;
  case MODE_LOOP:
    if (lastAnimation) {
      this->playIndex = 0;
    } else {
      this->playIndex++;
    }
    this->animation = this->animations[this->playIndex];
    this->changeMode(MODE_LOOP);
    break;
  }

  if (!this->animation) {
    this->changeMode(MODE_DEFAULT);
    return;
  }

  this->animation->play(currentMicros);
}

void Show::handleStopMode() {
}

Animation *Show::getCurrentAnimation() {
  return this->animation;
}

void Show::onModeChange(mcb modeCallback) {
  this->modeCallback = modeCallback;
}

void Show::changeMode(byte mode) {
  byte prevMode = this->mode;
  this->mode = mode;

  if (this->modeCallback) {
    this->modeCallback(prevMode, mode);
  }
}
