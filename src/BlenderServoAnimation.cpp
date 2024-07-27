#include "BlenderServoAnimation.h"
#include "AnimationData.h"
#include <Arduino.h>

using BlenderServoAnimationLibrary::Scene;

BlenderServoAnimation::~BlenderServoAnimation() {
  if (this->scenes) {
    delete[] this->scenes;
  }

  if (this->liveStream != nullptr) {
    delete this->liveStream;
  }

  if (this->playedIndexes != nullptr) {
    delete[] this->playedIndexes;
  }
}

bool BlenderServoAnimation::hasFinished() {
  return this->playIndex + 1 >= this->addIndex;
}

bool BlenderServoAnimation::hasScenes() {
  return this->addIndex > 0;
}

void BlenderServoAnimation::addScene(const byte *data, int size, byte fps, int frames) {
  AnimationData *animationData = new AnimationData(data, size);
  Scene *scene = new Scene(&this->servoManager, animationData, fps, frames);
  this->registerScene(scene);
}

void BlenderServoAnimation::addScene(Stream &stream, byte fps, int frames) {
  AnimationData *animationData = new AnimationData(&stream);
  Scene *scene = new Scene(&this->servoManager, animationData, fps, frames);
  this->registerScene(scene);
}

void BlenderServoAnimation::registerScene(Scene *scene) {
  Scene **newScenes = new Scene *[this->addIndex + 1];
  bool *newPlayedIndexes = new bool[this->addIndex + 1];

  for (int i = 0; i < this->addIndex; i++) {
    newScenes[i] = this->scenes[i];
    newPlayedIndexes[i] = this->playedIndexes[i];
  }

  newScenes[this->addIndex] = scene;
  newPlayedIndexes[this->addIndex] = false;

  delete[] this->scenes;
  delete[] this->playedIndexes;

  this->scenes = newScenes;
  this->playedIndexes = newPlayedIndexes;
  this->addIndex++;
}

void BlenderServoAnimation::setDefaultServoThreshold(byte value) {
  this->servoManager.setDefaultThreshold(value);
}

void BlenderServoAnimation::setServoThreshold(byte id, byte value) {
  this->servoManager.setThreshold(id, value);
}

void BlenderServoAnimation::setServoOffset(byte id, int offset) {
  this->servoManager.setOffset(id, offset);
}

void BlenderServoAnimation::setScene(byte index) {
  if (!this->scenes || index >= this->addIndex) {
    return;
  }

  byte prevIndex = this->playIndex;

  this->playIndex = index;
  this->playedIndexes[index] = true;
  this->scene = this->scenes[index];
  this->scene->reset();

  if (this->allScenesPlayed()) {
    this->resetPlayedScenes();
  }

  if (this->sceneCallback) {
    this->sceneCallback(prevIndex, index);
  }
}

void BlenderServoAnimation::setRandomScene() {
  byte randomIndex = 0;

  if (this->addIndex > 1) {
    do {
      randomIndex = random(this->addIndex);
    } while (this->playedIndexes[randomIndex]);
  }

  this->setScene(randomIndex);
}

void BlenderServoAnimation::resetScene() {
  byte prevIndex = this->playIndex;

  this->scene = nullptr;
  this->playIndex = 0;

  if (this->sceneCallback) {
    this->sceneCallback(prevIndex, 0);
  }
}

void BlenderServoAnimation::resetPlayedScenes() {
  for (int i = 0; i < this->addIndex; i++) {
    this->playedIndexes[i] = false;
  }
}

void BlenderServoAnimation::play() {
  if (!this->hasScenes() || !this->modeIsIn(2, MODE_DEFAULT, MODE_PAUSE)) {
    return;
  }

  if (!this->scene) {
    this->setScene(this->playIndex);
  }

  this->changeMode(MODE_PLAY);
}

void BlenderServoAnimation::playSingle(byte index) {
  bool indexExists = this->scenes && index < this->addIndex;
  bool modeIsAllowed = this->modeIsIn(2, MODE_DEFAULT, MODE_PAUSE);
  bool pausedOtherScene = this->mode == MODE_PAUSE && this->playIndex != index;

  if (!indexExists || !modeIsAllowed || pausedOtherScene) {
    return;
  }

  if (!this->scene || this->scene->getFrame() == 0) {
    this->setScene(index);
  }

  this->changeMode(MODE_PLAY_SINGLE);
}

void BlenderServoAnimation::playRandom() {
  if (!this->hasScenes() || !this->modeIsIn(2, MODE_DEFAULT, MODE_PAUSE)) {
    return;
  }

  if (!this->scene || this->scene->getFrame() == 0) {
    this->setRandomScene();
  }

  this->changeMode(MODE_PLAY_RANDOM);
}

void BlenderServoAnimation::loop() {
  if (!this->hasScenes() || !this->modeIsIn(2, MODE_DEFAULT, MODE_PAUSE)) {
    return;
  }

  if (!this->scene) {
    this->setScene(this->playIndex);
  }

  this->changeMode(MODE_LOOP);
}

void BlenderServoAnimation::pause() {
  if (!this->scene || this->modeIsIn(4, MODE_DEFAULT, MODE_PAUSE, MODE_STOP, MODE_LIVE)) {
    return;
  }

  this->changeMode(MODE_PAUSE);
}

void BlenderServoAnimation::stop() {
  if (this->modeIsIn(2, MODE_DEFAULT, MODE_STOP)) {
    return;
  }

  this->changeMode(MODE_STOP);
}

void BlenderServoAnimation::live() {
  if (this->mode != MODE_DEFAULT) {
    return;
  }

  if (this->liveStream != nullptr) {
    delete this->liveStream;
  }

  this->liveStream = new AnimationData();
  this->changeMode(MODE_LIVE);
}

void BlenderServoAnimation::live(Stream &stream) {
  if (this->mode != MODE_DEFAULT) {
    return;
  }

  if (this->liveStream != nullptr) {
    delete this->liveStream;
  }

  this->liveStream = new AnimationData(&stream);
  this->changeMode(MODE_LIVE);
}

void BlenderServoAnimation::writeLiveStream(byte value) {
  if (this->liveStream != nullptr) {
    this->liveStream->writeByte(value);
  }
}

byte BlenderServoAnimation::getMode() {
  return this->mode;
}

byte BlenderServoAnimation::getPlayIndex() {
  return this->playIndex;
}

bool BlenderServoAnimation::scenePlayed(int index) {
  return this->playedIndexes[index];
}

bool BlenderServoAnimation::allScenesPlayed() {
  for (int i = 0; i < this->addIndex; i++) {
    if (!this->playedIndexes[i]) {
      return false;
    }
  }

  return true;
}

void BlenderServoAnimation::run(unsigned long currentMicros) {
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
  case MODE_LIVE:
    this->handleLiveMode();
    break;
  }
}

void BlenderServoAnimation::handlePlayMode(unsigned long currentMicros) {
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
      this->resetScene();
    } else {
      this->setScene(this->playIndex + 1);
    }
    break;
  case MODE_PLAY_SINGLE:
    this->resetScene();
    break;
  case MODE_PLAY_RANDOM:
    this->setRandomScene();
    this->changeMode(MODE_PLAY_RANDOM);
    break;
  case MODE_LOOP:
    if (this->hasFinished()) {
      this->setScene(0);
    } else {
      this->setScene(this->playIndex + 1);
    }
    this->changeMode(MODE_LOOP);
    break;
  }

  if (!this->scene) {
    this->changeMode(MODE_DEFAULT);
  }
}

void BlenderServoAnimation::handleStopMode(unsigned long currentMicros) {
  if (!this->scene) {
    this->changeMode(MODE_DEFAULT);
    return;
  }

  this->scene->stop(currentMicros);

  if (this->servoManager.servosAreAllNeutral()) {
    this->changeMode(MODE_DEFAULT);
  }
}

void BlenderServoAnimation::handleLiveMode() {
  this->servoManager.parseData(this->liveStream, false);
}

Scene *BlenderServoAnimation::getCurrentScene() {
  return this->scene;
}

void BlenderServoAnimation::onPositionChange(pcb positionCallback) {
  this->servoManager.setPositionCallback(positionCallback);
}

void BlenderServoAnimation::onModeChange(mcb modeCallback) {
  this->modeCallback = modeCallback;
}

void BlenderServoAnimation::onSceneChange(scb sceneCallback) {
  this->sceneCallback = sceneCallback;
}

void BlenderServoAnimation::changeMode(byte mode) {
  byte prevMode = this->mode;
  this->mode = mode;

  if (prevMode == MODE_LIVE) {
    delete this->liveStream;
    this->liveStream = nullptr;
  }

  if (this->modeCallback) {
    this->modeCallback(prevMode, mode);
  }
}

bool BlenderServoAnimation::modeIsIn(byte modeAmount, ...) {
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
