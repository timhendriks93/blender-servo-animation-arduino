#include "BlenderServoAnimation.h"
#include "servo/Servo.h"
#include <Arduino.h>

using namespace BlenderServoAnimation;

Animation::Animation() {
}

Animation::Animation(byte fps, int frames) {
  this->frameMillis = Animation::SECOND_IN_MILLIS / fps;
  this->frames = frames;
}

void Animation::addServo(Servo servo) {
  byte id = servo.getID();
  this->servos[id] = &servo;
}

void Animation::addServos(Servo servos[], byte servoAmount) {
  for (int i = 0; i < servoAmount; i++) {
    byte id = servos[i].getID();
    this->servos[id] = &servos[i];
  }
}

void Animation::run(unsigned long currentMillis) {
  this->currentMillis = currentMillis;

  switch (mode) {
  case MODE_PLAY:
    this->handlePlayMode();
    break;
  case MODE_STOP:
    this->handleStopMode();
    break;
  case MODE_LIVE:
    this->handleLiveMode();
    break;
  }
}

void Animation::handlePlayMode() {
  bool isNewFrame = this->currentMillis - this->lastMillis >= this->frameMillis;

  if (!isNewFrame || this->frames == 0) {
    return;
  }

  this->lastMillis = this->currentMillis;
  this->frame++;

  if (this->frame >= this->frames) {
    this->frame = 0;
  }

  for (int i = 0; i < MAX_SERVO_COUNT; i++) {
    Servo *servo = this->servos[i];

    if (servo && servo->hasPositions()) {
      servo->moveByStep(this->frame);
    }
  }
}

void Animation::handleStopMode() {
  bool allNeutral = true;

  for (int i = 0; i < MAX_SERVO_COUNT; i++) {
    Servo *servo = this->servos[i];

    if (servo && servo->hasPositions() && !servo->isNeutral()) {
      servo->moveTowardsNeutral();
      allNeutral = false;
    }
  }

  if (allNeutral) {
    this->mode = Animation::MODE_DEFAULT;
    this->frame = 0;
    return;
  }

  if (this->stopStepDelay > 0) {
    delay(this->stopStepDelay);
  }
}

void Animation::handleLiveMode() {
  while (this->serial->available() > 0) {
    this->command.read(this->serial);

    if (!this->command.isComplete() || !this->command.isValid()) {
      continue;
    }

    byte id = this->command.getServoID();
    int position = this->command.getServoPosition();

    for (int i = 0; i < MAX_SERVO_COUNT; i++) {
      Servo *servo = this->servos[i];

      if (servo && servo->getID() == id) {
        servo->move(position);
        break;
      }
    }
  }
}

void Animation::play() {
  this->mode = Animation::MODE_PLAY;
  this->currentMillis = millis();
  this->lastMillis = this->currentMillis;
}

void Animation::pause() {
  this->mode = Animation::MODE_PAUSE;
}

void Animation::stop(byte stepDelay) {
  this->mode = Animation::MODE_STOP;
  this->stopStepDelay = stepDelay;
}

void Animation::live(Stream &serial) {
  this->mode = Animation::MODE_LIVE;
  this->serial = &serial;
}

byte Animation::getMode() {
  return this->mode;
}

int Animation::getFrame() {
  return this->frame;
}
