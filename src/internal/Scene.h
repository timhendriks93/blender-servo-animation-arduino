#include "ServoManager.h"
#include <Arduino.h>

#ifndef BlenderServoAnimation_Scene_H
#define BlenderServoAnimation_Scene_H

namespace BlenderServoAnimation {

class Scene {

public:
  Scene(ServoManager &servoManager, Stream &data, byte fps = 0, int frames = 0, bool hasProgmemStream = false);
  ~Scene();

  void play(unsigned long currentMicros);
  void stop(unsigned long currentMicros);

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

  bool hasProgmemStream = false;

  ServoManager* servoManager;

  Stream* data = nullptr;

  void parseCommands();

  bool isNewFrame(unsigned long currentMicros);

  int getMicrosDiff(unsigned long currentMicros);

};

} // namespace BlenderServoAnimation

#endif