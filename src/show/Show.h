#include "animation/Animation.h"
#include <Arduino.h>

#ifndef BlenderServoAnimation_Show_H
#define BlenderServoAnimation_Show_H

namespace BlenderServoAnimation {

class Show {

private:
  static const int MAX_ANIMATION_COUNT = 256;
  Animation *animations[MAX_ANIMATION_COUNT] = {};
  Animation *animation = nullptr;
  byte addIndex = 0;
  byte playIndex = 0;

public:
  byte countAnimations();
  void addAnimation(Animation &animation);
  void addAnimations(Animation animations[], byte animationAmount);
  void run(unsigned long currentMicros = micros());
  void play(unsigned long currentMicros = micros());
  void reset();
  byte getMode();
  bool hasAnimations();
  bool hasAnimation(byte id);
  Animation* getCurrentAnimation();
};

} // namespace BlenderServoAnimation

#endif