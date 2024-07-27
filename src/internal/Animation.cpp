#include "Animation.h"
#include "AnimationData.h"
#include <Arduino.h>

using BlenderServoAnimation::Animation;
using BlenderServoAnimation::Scene;

Animation::~Animation() {
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

bool Animation::hasFinished() {
  return this->playIndex + 1 >= this->addIndex;
}

bool Animation::hasScenes() {
  return this->addIndex > 0;
}

void Animation::addScene(const byte *data, int size, byte fps, int frames) {
  AnimationData *animationData = new AnimationData(data, size);
  Scene *scene = new Scene(&this->servoManager, animationData, fps, frames);
  this->registerScene(scene);
}

void Animation::addScene(Stream &stream, byte fps, int frames) {
  AnimationData *animationData = new AnimationData(&stream);
  Scene *scene = new Scene(&this->servoManager, animationData, fps, frames);
  this->registerScene(scene);
}

void Animation::registerScene(Scene *scene) {
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

void Animation::setDefaultServoThreshold(byte value) {
  this->servoManager.setDefaultThreshold(value);
}

void Animation::setServoThreshold(byte id, byte value) {
  this->servoManager.setThreshold(id, value);
}

void Animation::setServoOffset(byte id, int offset) {
  this->servoManager.setOffset(id, offset);
}

void Animation::setScene(byte index) {
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

void Animation::setRandomScene() {
  byte randomIndex = 0;

  if (this->addIndex > 1) {
    do {
      randomIndex = random(this->addIndex);
    } while (this->playedIndexes[randomIndex]);
  }

  this->setScene(randomIndex);
}

void Animation::resetScene() {
  byte prevIndex = this->playIndex;

  this->scene = nullptr;
  this->playIndex = 0;

  if (this->sceneCallback) {
    this->sceneCallback(prevIndex, 0);
  }
}

void Animation::resetPlayedScenes() {
  for (int i = 0; i < this->addIndex; i++) {
    this->playedIndexes[i] = false;
  }
}

void Animation::play() {
  if (!this->hasScenes() || !this->modeIsIn(2, MODE_DEFAULT, MODE_PAUSE)) {
    return;
  }

  if (!this->scene) {
    this->setScene(this->playIndex);
  }

  this->changeMode(MODE_PLAY);
}

void Animation::playSingle(byte index) {
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
    this->setScene(this->playIndex);
  }

  this->changeMode(MODE_LOOP);
}

void Animation::pause() {
  if (!this->scene ||
      this->modeIsIn(4, MODE_DEFAULT, MODE_PAUSE, MODE_STOP, MODE_LIVE)) {
    return;
  }

  this->changeMode(MODE_PAUSE);
}

void Animation::stop() {
  if (this->modeIsIn(2, MODE_DEFAULT, MODE_STOP)) {
    return;
  }

  this->changeMode(MODE_STOP);
}

void Animation::live() {
  if (this->mode != MODE_DEFAULT) {
    return;
  }

  if (this->liveStream != nullptr) {
    delete this->liveStream;
  }

  this->liveStream = new AnimationData();
  this->changeMode(MODE_LIVE);
}

void Animation::live(Stream &stream) {
  if (this->mode != MODE_DEFAULT) {
    return;
  }

  if (this->liveStream != nullptr) {
    delete this->liveStream;
  }

  this->liveStream = new AnimationData(&stream);
  this->changeMode(MODE_LIVE);
}

void Animation::writeLiveStream(byte value) {
  if (this->liveStream != nullptr) {
    this->liveStream->writeByte(value);
  }
}

byte Animation::getMode() {
  return this->mode;
}

byte Animation::getPlayIndex() {
  return this->playIndex;
}

bool Animation::scenePlayed(int index) {
  return this->playedIndexes[index];
}

bool Animation::allScenesPlayed() {
  for (int i = 0; i < this->addIndex; i++) {
    if (!this->playedIndexes[i]) {
      return false;
    }
  }

  return true;
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
  case MODE_LIVE:
    this->handleLiveMode();
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

void Animation::handleLiveMode() {
  this->servoManager.parseData(this->liveStream, false);
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

  if (prevMode == MODE_LIVE) {
    delete this->liveStream;
    this->liveStream = nullptr;
  }

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
