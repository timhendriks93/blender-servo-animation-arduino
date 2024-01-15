#include "ServoManager.h"
#include "Servo.h"
#include <Arduino.h>

using namespace BlenderServoAnimation;

ServoManager::~ServoManager() {
  for (int i = 0; i < MAX_SERVO_COUNT; i++) {
    if (this->servos[i]) {
      delete this->servos[i];
    }
  }
}

void ServoManager::setPositionCallback(pcb positionCallback) {
  this->positionCallback = positionCallback;

  for (int i = 0; i < MAX_SERVO_COUNT; i++) {
    if (this->servos[i]) {
      this->servos[i]->setPositionCallback(positionCallback);
    }
  }
}

void ServoManager::setDefaultThreshold(byte value) {
  this->defaultThreshold = value;
}

void ServoManager::setThreshold(byte servoId, byte value) {
  this->thresholds[servoId] = value;

  if (this->servos[servoId]) {
    this->servos[servoId]->setThreshold(value);
  }
}

void ServoManager::parseData(AnimationData *data, bool considerLineBreaks) {
  if (!data || !this->hasPositionCallback()) {
    return;
  }

  Command command;

  while (data->isAvailable()) {
    byte value = data->getNextByte();

    if (considerLineBreaks && value == Command::LINE_BREAK) {
      break;
    }

    command.write(value);

    this->handleCommand(command);
  }
}

void ServoManager::handleCommand(Command command) {
  if (!command.isValid()) {
    return;
  }

  byte id = command.getServoID();
  int position = command.getServoPosition();

  if (!this->servos[id]) {
    this->addServo(id);
  }

  this->servos[id]->move(position);
}

void ServoManager::moveAllTowardsNeutral() {
  for (int i = 0; i < MAX_SERVO_COUNT; i++) {
    Servo *servo = this->servos[i];

    if (servo && !servo->isNeutral()) {
      servo->moveTowardsNeutral();
    }
  }
}

bool ServoManager::hasPositionCallback() {
  return this->positionCallback != nullptr;
}

bool ServoManager::servosAreAllNeutral() {
  for (int i = 0; i < MAX_SERVO_COUNT; i++) {
    Servo *servo = this->servos[i];

    if (servo && !servo->isNeutral()) {
      return false;
    }
  }

  return true;
}

void ServoManager::addServo(byte id) {
  byte threshold = this->defaultThreshold;

  if (this->thresholds[id]) {
    threshold = this->thresholds[id];
  }

  this->servos[id] = new Servo(id, this->positionCallback, threshold);
}
