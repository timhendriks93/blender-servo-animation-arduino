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

bool Show::hasAnimation(byte index) {
  return this->animations[index] != nullptr;
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

void Show::setRandomAnimation() {
  byte randomIndex = 0;

  if (this->countAnimations() > 1) {
    randomIndex = random(this->addIndex);
  }

  this->playIndex = randomIndex;
  this->animation = this->animations[this->playIndex];
}

void Show::play(unsigned long currentMicros) {
  if (!this->hasAnimations() || !this->modeIsIn(2, MODE_DEFAULT, MODE_PAUSE)) {
    return;
  }

  if (!this->animation) {
    this->animation = this->animations[this->playIndex];
  }

  this->animation->play(currentMicros);
  this->changeMode(MODE_PLAY);
}

void Show::playSingle(byte index, unsigned long currentMicros) {
  Animation *animation = this->animations[index];

  if (animation == nullptr || !this->modeIsIn(2, MODE_DEFAULT, MODE_PAUSE) ||
      (this->mode == MODE_PAUSE && playIndex != index)) {
    return;
  }

  if (!this->animation || this->animation->getFrame() == 0) {
    this->playIndex = index;
    this->animation = animation;
  }

  this->animation->play(currentMicros);
  this->changeMode(MODE_PLAY_SINGLE);
}

void Show::playRandom(unsigned long currentMicros) {
  if (!this->hasAnimations() || !this->modeIsIn(2, MODE_DEFAULT, MODE_PAUSE)) {
    return;
  }

  if (!this->animation || this->animation->getFrame() == 0) {
    this->setRandomAnimation();
  }

  this->animation->play(currentMicros);
  this->changeMode(MODE_PLAY_RANDOM);
}

void Show::loop(unsigned long currentMicros) {
  if (!this->hasAnimations() || !this->modeIsIn(2, MODE_DEFAULT, MODE_PAUSE)) {
    return;
  }

  if (!this->animation) {
    this->animation = this->animations[this->playIndex];
  }

  this->animation->play(currentMicros);
  this->changeMode(MODE_LOOP);
}

void Show::pause() {
  if (!this->animation || !this->modeIsIn(4, MODE_PLAY, MODE_PLAY_SINGLE,
                                          MODE_PLAY_RANDOM, MODE_LOOP)) {
    return;
  }

  this->animation->pause();
  this->changeMode(MODE_PAUSE);
}

void Show::stop(byte stepDelay) {
  if (!this->animation || this->modeIsIn(2, MODE_DEFAULT, MODE_STOP)) {
    return;
  }

  this->animation->stop(stepDelay);
  this->changeMode(MODE_STOP);
}

void Show::live(Stream &serial) {
  if (!this->hasAnimations() || this->mode != MODE_DEFAULT) {
    return;
  }

  if (!this->animation) {
    this->animation = this->animations[this->playIndex];
  }

  this->animation->live(serial);
  this->changeMode(MODE_LIVE);
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

byte Show::getPlayIndex() {
  return this->playIndex;
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
    this->handleStopMode(currentMicros);
    break;
  }
}

void Show::handlePlayMode(unsigned long currentMicros) {
  if (!this->animation) {
    this->changeMode(MODE_DEFAULT);
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

void Show::handleStopMode(unsigned long currentMicros) {
  if (!this->animation) {
    this->changeMode(MODE_DEFAULT);
    return;
  }

  this->animation->run(currentMicros);

  if (this->animation->getMode() == Animation::MODE_DEFAULT) {
    this->changeMode(MODE_DEFAULT);
  }
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

bool Show::modeIsIn(byte modeAmount, ...) {
  bool match = false;

  va_list modes;
  va_start(modes, modeAmount);

  for (int i = 0; i < modeAmount; i++) {
    if (this->mode == va_arg(modes, int)) {
      match = true;
    }
  }

  va_end(modes);

  return match;
}
