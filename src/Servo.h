#include "typedefs.h"
#include <Arduino.h>

#ifndef BlenderServoAnimationLibrary_Servo_H
#define BlenderServoAnimationLibrary_Servo_H

namespace BlenderServoAnimationLibrary {

class Servo {

public:
  Servo(byte id, pcb positionCallback, byte threshold = 0);

  void move(int position, bool useOffset = true);
  void moveTowardsNeutral();
  void setThreshold(byte value);
  void setOffset(int offset);
  void setPositionCallback(pcb positionCallback);

  bool isNeutral();

  byte getId();

private:
  const byte STEP_DIVIDER = 10;

  byte id = 0;
  byte threshold = 0;
  byte step = 0;

  int neutralPosition = -1;
  int currentPosition = -1;
  int offset = 0;

  pcb positionCallback = nullptr;

  bool positionExceedsThreshold(int position);
};

} // namespace BlenderServoAnimationLibrary

#endif