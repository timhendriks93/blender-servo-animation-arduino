#include "Scene.h"
#include "Command.h"
#include "Servo.h"
#include <Arduino.h>

using BlenderServoAnimation::Scene;

Scene::Scene(ServoManager *servoManager, AnimationData *data, byte fps,
             int frames) {
  this->servoManager = servoManager;
  this->data = data;
  this->fps = fps;
  this->frames = frames;
  this->frameMicros = round((float)Scene::SECOND_IN_MICROS / (float)fps);
  this->diffPerSecond = Scene::SECOND_IN_MICROS - (this->frameMicros * fps);
}

Scene::~Scene() {
  if (this->data) {
    delete this->data;
  }
}

void Scene::play(unsigned long currentMicros) {
  if (this->frames == 0) {
    this->servoManager->parseData(this->data);
    return;
  }

  if (!this->isNewFrame(currentMicros)) {
    return;
  }

  this->lastMicros = currentMicros;
  this->frame++;

  if (this->frame % this->fps == 0) {
    this->lastMicros += this->diffPerSecond;
  }

  this->servoManager->parseData(this->data);
}

void Scene::stop(unsigned long currentMicros) {
  if (this->getMicrosDiff(currentMicros) < STOP_DELAY_IN_MICROS) {
    return;
  }

  this->lastMicros = currentMicros;
  this->servoManager->moveAllTowardsNeutral();

  if (this->servoManager->servosAreAllNeutral()) {
    this->reset();
  }
}

void Scene::reset() {
  this->frame = 0;
  this->data->reset();
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
