#include "ServoManager.h"
#include "Servo.h"
#include <Arduino.h>

using BlenderServoAnimationLibrary::Servo;
using BlenderServoAnimationLibrary::ServoManager;

ServoManager::~ServoManager() {
  if (this->servos) {
    delete[] this->servos;
  }
}

void ServoManager::setPositionCallback(pcb positionCallback) {
  this->positionCallback = positionCallback;

  for (byte i = 0; i < this->servoAmount; i++) {
    this->servos[i]->setPositionCallback(positionCallback);
  }
}

void ServoManager::setDefaultThreshold(byte value) {
  this->defaultThreshold = value;
}

void ServoManager::setThreshold(byte servoId, byte value) {
  Servo *servo = this->getServo(servoId);

  servo->setThreshold(value);
}

void ServoManager::setOffset(byte servoId, int offset) {
  Servo *servo = this->getServo(servoId);

  servo->setOffset(offset);
}

void ServoManager::parseData(AnimationData *data, bool considerLineBreaks) {
  if (!data || !this->positionCallback) {
    return;
  }

  while (data->isAvailable()) {
    byte value = data->getNextByte();

    if (considerLineBreaks && value == Command::LINE_BREAK) {
      break;
    }

    this->command.write(value);

    this->handleCommand();
  }
}

void ServoManager::handleCommand() {
  if (!this->command.isValid()) {
    return;
  }

  byte id = this->command.getServoID();
  int position = this->command.getServoPosition();
  Servo *servo = this->getServo(id);

  servo->move(position);
}

void ServoManager::moveAllTowardsNeutral() {
  for (byte i = 0; i < this->servoAmount; i++) {
    Servo *servo = this->servos[i];

    if (!servo->isNeutral()) {
      servo->moveTowardsNeutral();
    }
  }
}

bool ServoManager::servosAreAllNeutral() {
  for (byte i = 0; i < this->servoAmount; i++) {
    Servo *servo = this->servos[i];

    if (!servo->isNeutral()) {
      return false;
    }
  }

  return true;
}

Servo *ServoManager::addServo(byte id) {
  Servo *servo = new Servo(id, this->positionCallback, this->defaultThreshold);
  Servo **newServos = new Servo *[this->servoAmount + 1];

  for (byte i = 0; i < this->servoAmount; i++) {
    newServos[i] = this->servos[i];
  }

  newServos[this->servoAmount] = servo;

  delete[] this->servos;

  this->servos = newServos;
  this->servoAmount++;

  return servo;
}

Servo *ServoManager::getServo(byte id) {
  for (byte i = 0; i < this->servoAmount; i++) {
    Servo *servo = this->servos[i];

    if (servo->getId() == id) {
      return servo;
    }
  }

  return this->addServo(id);
}
