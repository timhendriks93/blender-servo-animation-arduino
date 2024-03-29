#include "Animation.h"
#include "Servo.h"
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
    this->handleStopMode(currentMicros);
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

void Animation::handleStopMode(unsigned long currentMicros) {
  bool allNeutral = true;

  if (currentMicros - this->lastMicros < 10000) {
    return;
  }

  this->lastMicros = currentMicros;

  for (int i = 0; i < MAX_SERVO_COUNT; i++) {
    Servo *servo = this->servos[i];

    if (servo && servo->hasPositions() && !servo->isNeutral()) {
      servo->moveTowardsNeutral();
      allNeutral = false;
    }
  }

  if (!allNeutral) {
    return;
  }

  this->changeMode(MODE_DEFAULT);
}

void Animation::handleLiveMode() {
  while (this->liveStream->available() > 0) {
    this->liveCommand.read(this->liveStream);

    if (!this->liveCommand.isComplete() || !this->liveCommand.isValid()) {
      continue;
    }

    byte id = this->liveCommand.getServoID();
    int position = this->liveCommand.getServoPosition();

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

void Animation::stop(unsigned long currentMicros) {
  if (this->modeIsIn(2, MODE_DEFAULT, MODE_STOP)) {
    return;
  }

  this->frame = 0;
  this->lastMicros = currentMicros;
  this->changeMode(MODE_STOP);
}

void Animation::live(Stream &liveStream) {
  if (this->mode != MODE_DEFAULT) {
    return;
  }

  this->liveStream = &liveStream;
  this->changeMode(MODE_LIVE);
}

byte Animation::getFPS() {
  return this->fps;
}

byte Animation::getMode() {
  return this->mode;
}

int Animation::getFrame() {
  return this->frame;
}

int Animation::getFrames() {
  return this->frames;
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
