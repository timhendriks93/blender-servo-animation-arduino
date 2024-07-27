#include "AnimationData.h"
#include "ServoManager.h"
#include <Arduino.h>

#ifndef BlenderServoAnimationLibrary_Scene_H
#define BlenderServoAnimationLibrary_Scene_H

namespace BlenderServoAnimationLibrary {

class Scene {

public:
  Scene(ServoManager *servoManager, AnimationData *data, byte fps, int frames);
  ~Scene();

  void play(unsigned long currentMicros);
  void stop(unsigned long currentMicros);
  void reset();

  bool hasFinished();

  byte getFPS();

  int getFrame();
  int getFrames();

private:
  static const unsigned long MAX_MICROS = 4294967295;
  static const unsigned long SECOND_IN_MICROS = 1000000;
  static const unsigned int STOP_DELAY_IN_MICROS = 10000;

  byte fps = 0;

  int diffPerSecond = 0;

  unsigned int frame = 0;
  unsigned int frames = 0;
  unsigned int frameMicros = 0;

  unsigned long lastMicros = 0;

  ServoManager *servoManager = nullptr;

  AnimationData *data = nullptr;

  bool isNewFrame(unsigned long currentMicros);

  unsigned int getMicrosDiff(unsigned long currentMicros);
};

} // namespace BlenderServoAnimationLibrary

#endif