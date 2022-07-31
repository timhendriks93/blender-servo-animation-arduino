#include <Arduino.h>

#ifndef BlenderServoAnimation_Servo_H
#define BlenderServoAnimation_Servo_H

namespace BlenderServoAnimation {
class Servo {
  typedef void (*cb)(byte, int);

private:
  byte id;
  byte threshold;

  int neutralPosition = -1;
  int currentPosition = -1;

  const int *positions = nullptr;

  cb moveCallback;

public:
  Servo(byte id, const int positions[], cb moveCallback, byte threshold = 20);
  Servo(byte id, cb moveCallback, byte threshold = 20);
  void move(int position, bool force = false);
  void moveByStep(int step);
  void moveTowardsNeutral(bool inSteps = true);
  bool isNeutral();
  bool hasPositions();
  byte getID();
};
} // namespace BlenderServoAnimation

#endif