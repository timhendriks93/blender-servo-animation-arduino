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
  void parseStream(Stream *stream, bool considerLineBreaks = true);
  void moveAllTowardsNeutral();

  bool hasPositionCallback();
  bool servosAreAllNeutral();

private:
  static const int MAX_SERVO_COUNT = 256;

  Servo *servos[MAX_SERVO_COUNT] = {nullptr};

  pcb positionCallback = nullptr;

  byte defaultThreshold = 0;
  byte thresholds[MAX_SERVO_COUNT] = {0};

  void addServo(byte id);
  void handleCommand(Command command);
};

} // namespace BlenderServoAnimation

#endif
