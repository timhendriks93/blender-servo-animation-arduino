#include "AnimationData.h"
#include "Command.h"
#include "Servo.h"
#include "typedefs.h"
#include <Arduino.h>

#ifndef BlenderServoAnimation_ServoManager_H
#define BlenderServoAnimation_ServoManager_H

namespace BlenderServoAnimation {

class ServoManager {

public:
  ~ServoManager();

  void setPositionCallback(pcb positionCallback);
  void setDefaultThreshold(byte value);
  void setThreshold(byte servoId, byte value);
  void setOffset(byte servoId, int offset);
  void parseData(AnimationData *data, bool considerLineBreaks = true);
  void moveAllTowardsNeutral();

  bool servosAreAllNeutral();

private:
  Servo **servos = nullptr;

  Command command;

  pcb positionCallback = nullptr;

  byte servoAmount = 0;
  byte defaultThreshold = 0;

  void handleCommand();

  Servo *addServo(byte id);
  Servo *getServo(byte id);
};

} // namespace BlenderServoAnimation

#endif
