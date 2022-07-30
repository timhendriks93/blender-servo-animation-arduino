#include "BlenderServoAnimation.h"
#include "servo/Servo.h"
#include <Arduino.h>

using namespace BlenderServoAnimation;

Animation::Animation() {
}

Animation::Animation(byte fps, int frames) {
  this->fps = fps;
  this->frames = frames;
  this->frameMicros = round((float)Animation::SECOND_IN_MICROS / (float)fps);
  this->diffPerSecond = Animation::SECOND_IN_MICROS - (this->frameMicros * fps);
}

void Animation::addServo(Servo &servo) {
  byte id = servo.getID();
  this->servos[id] = &servo;
}

void Animation::addServos(Servo servos[], byte servoAmount) {
  for (int i = 0; i < servoAmount; i++) {
    this->addServo(servos[i]);
  }
}

void Animation::run(unsigned long currentMicros) {
  switch (mode) {
  case MODE_PLAY:
    this->handlePlayMode(currentMicros);
    break;
  case MODE_STOP:
    this->handleStopMode();
    break;
  case MODE_LIVE:
    this->handleLiveMode();
    break;
  }
}

void Animation::handlePlayMode(unsigned long currentMicros) {
  bool isNewFrame = currentMicros - this->lastMicros >= this->frameMicros;

  if (!isNewFrame || this->frames == 0) {
    return;
  }

  this->lastMicros = currentMicros;
  this->frame++;

  if (this->frame >= this->frames) {
    this->frame = 0;
  }

  if (this->frame % this->fps == 0) {
    this->lastMicros += this->diffPerSecond;
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
  this->lastMicros = micros();
}

void Animation::pause() {
  this->mode = Animation::MODE_PAUSE;
}

void Animation::stop(byte stepDelay) {
  this->mode = Animation::MODE_STOP;
  this->stopStepDelay = stepDelay;
  this->frame = 0;
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
