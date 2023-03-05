#include "animation/Animation.h"
#include "servo/Servo.h"
#include <Arduino.h>

using namespace BlenderServoAnimation;

Animation::Animation() {
}

Animation::Animation(byte fps, int frames)
    : Animation::Animation(0, fps, frames) {
}

Animation::Animation(byte id, byte fps, int frames) {
  this->id = id;
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

void Animation::onModeChange(mcb modeCallback) {
  this->modeCallback = modeCallback;
}

void Animation::changeMode(byte mode) {
  byte prevMode = this->mode;
  this->mode = mode;

  if (this->modeCallback) {
    this->modeCallback(prevMode, mode);
  }
}

void Animation::run(unsigned long currentMicros) {
  switch (this->mode) {
  case MODE_PLAY:
  case MODE_LOOP:
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
      servo->moveByFrame(this->frame);
    }
  }

  if (this->frame == 0) {
    if (this->mode == MODE_LOOP) {
      this->changeMode(MODE_LOOP);
    } else {
      this->changeMode(MODE_DEFAULT);
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
    this->changeMode(MODE_DEFAULT);
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

void Animation::play(unsigned long currentMicros) {
  if (!this->modeIsIn(2, MODE_DEFAULT, MODE_PAUSE)) {
    return;
  }

  this->lastMicros = currentMicros;
  this->changeMode(MODE_PLAY);
}

void Animation::pause() {
  if (!this->modeIsIn(2, MODE_PLAY, MODE_LOOP)) {
    return;
  }

  this->changeMode(MODE_PAUSE);
}

void Animation::loop(unsigned long currentMicros) {
  if (!this->modeIsIn(2, MODE_DEFAULT, MODE_PAUSE)) {
    return;
  }

  this->lastMicros = currentMicros;
  this->changeMode(MODE_LOOP);
}

void Animation::stop(byte stepDelay) {
  if (this->modeIsIn(2, MODE_DEFAULT, MODE_STOP)) {
    return;
  }

  this->stopStepDelay = stepDelay;
  this->frame = 0;
  this->changeMode(MODE_STOP);
}

void Animation::live(Stream &serial) {
  if (this->mode != MODE_DEFAULT) {
    return;
  }

  this->serial = &serial;
  this->changeMode(MODE_LIVE);
}

byte Animation::getID() {
  return this->id;
}

byte Animation::getMode() {
  return this->mode;
}

int Animation::getFrame() {
  return this->frame;
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
