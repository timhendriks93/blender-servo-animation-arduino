#include "Animation.h"
#include "ProgmemStream.h"
#include <Arduino.h>

using namespace BlenderServoAnimation;

Animation::~Animation() {
  for (int i = 0; i < this->addIndex; i++) {
    if (this->scenes[i]) {
      delete this->scenes[i];
    }
  }
}

int Animation::countScenes() {
  return this->addIndex;
}

bool Animation::hasFinished() {
  return this->playIndex + 1 >= this->addIndex;
}

bool Animation::hasScenes() {
  return this->countScenes() > 0;
}

bool Animation::hasScene(byte index) {
  return this->scenes[index] != nullptr;
}

void Animation::addScene(const byte *data, int dataLength, byte fps,
                         int frames) {
  ProgmemStream *stream = new ProgmemStream(data, dataLength);
  this->addScene(*stream, fps, frames);
}

void Animation::addScene(Stream &data, byte fps, int frames) {
  this->scenes[this->addIndex] =
      new Scene(this->servoManager, data, fps, frames);
  this->addIndex++;
}

void Animation::setDefaultServoThreshold(byte value) {
  this->servoManager.setDefaultThreshold(value);
}

void Animation::setServoThreshold(byte id, byte value) {
  this->servoManager.setThreshold(id, value);
}

void Animation::setRandomScene() {
  byte randomIndex = 0;

  if (this->countScenes() > 1) {
    randomIndex = random(this->addIndex);
  }

  this->playIndex = randomIndex;
  this->scene = this->scenes[this->playIndex];
}

void Animation::play() {
  if (!this->hasScenes() || !this->modeIsIn(2, MODE_DEFAULT, MODE_PAUSE)) {
    return;
  }

  if (!this->scene) {
    this->scene = this->scenes[this->playIndex];
  }

  this->changeMode(MODE_PLAY);
}

void Animation::playSingle(byte index) {
  Scene *scene = this->scenes[index];

  if (scene == nullptr || !this->modeIsIn(2, MODE_DEFAULT, MODE_PAUSE) ||
      (this->mode == MODE_PAUSE && playIndex != index)) {
    return;
  }

  if (!this->scene || this->scene->getFrame() == 0) {
    this->playIndex = index;
    this->scene = scene;
  }

  this->changeMode(MODE_PLAY_SINGLE);
}

void Animation::playRandom() {
  if (!this->hasScenes() || !this->modeIsIn(2, MODE_DEFAULT, MODE_PAUSE)) {
    return;
  }

  if (!this->scene || this->scene->getFrame() == 0) {
    this->setRandomScene();
  }

  this->changeMode(MODE_PLAY_RANDOM);
}

void Animation::loop() {
  if (!this->hasScenes() || !this->modeIsIn(2, MODE_DEFAULT, MODE_PAUSE)) {
    return;
  }

  if (!this->scene) {
    this->scene = this->scenes[this->playIndex];
  }

  this->changeMode(MODE_LOOP);
}

void Animation::pause() {
  if (!this->scene || !this->modeIsIn(4, MODE_PLAY, MODE_PLAY_SINGLE,
                                      MODE_PLAY_RANDOM, MODE_LOOP)) {
    return;
  }

  this->changeMode(MODE_PAUSE);
}

void Animation::stop() {
  if (!this->scene || this->modeIsIn(2, MODE_DEFAULT, MODE_STOP)) {
    return;
  }

  this->changeMode(MODE_STOP);
}

void Animation::reset() {
  this->addIndex = 0;
  this->playIndex = 0;

  for (int i = 0; i < this->addIndex; i++) {
    this->scenes[i] = nullptr;
  }
}

byte Animation::getMode() {
  return this->mode;
}

byte Animation::getPlayIndex() {
  return this->playIndex;
}

void Animation::run(unsigned long currentMicros) {
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

void Animation::handlePlayMode(unsigned long currentMicros) {
  if (!this->scene) {
    this->changeMode(MODE_DEFAULT);
    return;
  }

  this->scene->play(currentMicros);

  if (!this->scene->hasFinished()) {
    return;
  }

  switch (this->mode) {
  case MODE_PLAY:
    if (this->hasFinished()) {
      this->scene = nullptr;
    } else {
      this->playIndex++;
      this->scene = this->scenes[this->playIndex];
    }
    break;
  case MODE_PLAY_SINGLE:
    this->scene = nullptr;
    break;
  case MODE_PLAY_RANDOM:
    this->setRandomScene();
    this->changeMode(MODE_PLAY_RANDOM);
    break;
  case MODE_LOOP:
    if (this->hasFinished()) {
      this->playIndex = 0;
    } else {
      this->playIndex++;
    }
    this->scene = this->scenes[this->playIndex];
    this->changeMode(MODE_LOOP);
    break;
  }

  if (!this->scene) {
    this->changeMode(MODE_DEFAULT);
    return;
  }
}

void Animation::handleStopMode(unsigned long currentMicros) {
  if (!this->scene) {
    this->changeMode(MODE_DEFAULT);
    return;
  }

  this->scene->stop(currentMicros);

  if (this->servoManager.servosAreAllNeutral()) {
    this->changeMode(MODE_DEFAULT);
  }
}

Scene *Animation::getCurrentScene() {
  return this->scene;
}

void Animation::onPositionChange(pcb positionCallback) {
  this->servoManager.setPositionCallback(positionCallback);
}

void Animation::onModeChange(mcb modeCallback) {
  this->modeCallback = modeCallback;
}

void Animation::onSceneChange(scb sceneCallback) {
  this->sceneCallback = sceneCallback;
}

void Animation::changeMode(byte mode) {
  byte prevMode = this->mode;
  this->mode = mode;

  if (this->modeCallback) {
    this->modeCallback(prevMode, mode);
  }
}

bool Animation::modeIsIn(byte modeAmount, ...) {
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
