#include "typedefs.h"
#include <Arduino.h>

#ifndef BlenderServoAnimation_Servo_H
#define BlenderServoAnimation_Servo_H

namespace BlenderServoAnimation {

class Servo {

public:
  Servo(byte id, pcb positionCallback, byte threshold = 0);

  void move(int position);
  void moveTowardsNeutral();
  void setThreshold(byte value);
  void setPositionCallback(pcb positionCallback);

  bool isNeutral();

private:
  const byte STEP_DIVIDER = 10;

  byte id = 0;
  byte threshold = 0;
  byte step = 0;

  int neutralPosition = -1;
  int currentPosition = -1;

  pcb positionCallback = nullptr;

  bool positionExceedsThreshold(int position);
};

} // namespace BlenderServoAnimation

#endif