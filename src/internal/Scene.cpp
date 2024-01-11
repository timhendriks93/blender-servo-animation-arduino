#include "Scene.h"
#include "Command.h"
#include "ProgmemStream.h"
#include "Servo.h"
#include <Arduino.h>

using namespace BlenderServoAnimation;

Scene::Scene(ServoManager &servoManager, Stream &data, byte fps, int frames,
             bool hasProgmemStream) {
  this->servoManager = &servoManager;
  this->data = &data;
  this->fps = fps;
  this->frames = frames;
  this->frameMicros = round((float)Scene::SECOND_IN_MICROS / (float)fps);
  this->diffPerSecond = Scene::SECOND_IN_MICROS - (this->frameMicros * fps);
  this->hasProgmemStream = hasProgmemStream;
}

Scene::~Scene() {
  if (this->hasProgmemStream) {
    delete this->data;
  }
}

void Scene::play(unsigned long currentMicros) {
  if (this->frames == 0) {
    this->parseCommands();
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

  this->parseCommands();
}

void Scene::stop(unsigned long currentMicros) {
  if (this->getMicrosDiff(currentMicros) < STOP_DELAY_IN_MICROS) {
    return;
  }

  this->lastMicros = currentMicros;
  this->servoManager->moveAllServosToNeutral();

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

void Scene::parseCommands() {
  if (!this->servoManager->hasPositionCallback()) {
    return;
  }

  Command command;

  while (this->data->available() > 0) {
    byte value = this->data->read();

    if (this->frames > 0 && value == Command::LINE_BREAK) {
      break;
    }

    command.write(value);

    this->servoManager->handleCommand(command);
  }
}
