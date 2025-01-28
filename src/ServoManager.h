#include "AnimationData.h"
#include "Command.h"
#include "CommonTypes.h"
#include "Servo.h"
#include <Arduino.h>

#ifndef BlenderServoAnimationLibrary_ServoManager_H
#define BlenderServoAnimationLibrary_ServoManager_H

namespace BlenderServoAnimationLibrary {

class ServoManager {

public:
  ~ServoManager();

  void setPositionCallback(PositionCallback callback);
  void setDefaultThreshold(byte value);
  void setThreshold(byte servoId, byte value);
  void setOffset(byte servoId, int offset);
  void parseData(AnimationData *data, bool considerLineBreaks = true);
  void moveAllTowardsNeutral();

  bool servosAreAllNeutral();

private:
  Servo **servos = nullptr;

  Command command;

  PositionCallback positionCallback = nullptr;

  byte servoAmount = 0;
  byte defaultThreshold = 0;

  void handleCommand();

  Servo *addServo(byte id);
  Servo *getServo(byte id);
};

} // namespace BlenderServoAnimationLibrary

#endif
