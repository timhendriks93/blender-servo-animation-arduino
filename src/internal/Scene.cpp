#include "Scene.h"
#include "Command.h"
#include "ProgmemStream.h"
#include "Servo.h"
#include <Arduino.h>

using namespace BlenderServoAnimation;

Scene::Scene(ServoManager &servoManager, byte fps, int frames) {
  this->servoManager = &servoManager;
  this->fps = fps;
  this->frames = frames;
  this->frameMicros = round((float)Scene::SECOND_IN_MICROS / (float)fps);
  this->diffPerSecond = Scene::SECOND_IN_MICROS - (this->frameMicros * fps);
}

Scene::~Scene() {
  if (this->progmemData) {
    delete this->progmemData;
  }
}

void Scene::setData(Stream *data) {
  this->data = data;
}

void Scene::setProgmemData(ProgmemStream *data) {
  this->progmemData = data;
}

void Scene::play(unsigned long currentMicros) {
  Stream *data = this->getAnimationData();

  if (this->frames == 0) {
    this->servoManager->parseStream(data);
    return;
  }

  if (!this->isNewFrame(currentMicros)) {
    return;
  }

  this->lastMicros = currentMicros;

  if (this->frame >= this->frames) {
    this->frame = 0;
  } else {
    this->frame++;
  }

  if (this->frame % this->fps == 0) {
    this->lastMicros += this->diffPerSecond;
  }

  this->servoManager->parseStream(data);
}

void Scene::stop(unsigned long currentMicros) {
  if (this->getMicrosDiff(currentMicros) < STOP_DELAY_IN_MICROS) {
    return;
  }

  this->lastMicros = currentMicros;
  this->servoManager->moveAllTowardsNeutral();

  if (this->servoManager->servosAreAllNeutral()) {
    this->frame = 0;
  }
}

unsigned int Scene::getMicrosDiff(unsigned long currentMicros) {
  if (currentMicros >= this->lastMicros) {
    return currentMicros - this->lastMicros;
  }

  return MAX_MICROS - this->lastMicros + currentMicros;
}

bool Scene::isNewFrame(unsigned long currentMicros) {
  return this->lastMicros == 0 ||
         this->getMicrosDiff(currentMicros) >= this->frameMicros;
}

bool Scene::hasFinished() {
  return this->frame == this->frames;
}

byte Scene::getFPS() {
  return this->fps;
}

int Scene::getFrame() {
  return this->frame;
}

int Scene::getFrames() {
  return this->frames;
}

Stream *Scene::getAnimationData() {
  if (this->progmemData) {
    return this->progmemData;
  }

  return this->data;
}
