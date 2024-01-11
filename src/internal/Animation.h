#include "Scene.h"
#include "ServoManager.h"
#include "typedefs.h"
#include <Arduino.h>
#include <stdarg.h>

#ifndef BlenderServoAnimation_Animation_H
#define BlenderServoAnimation_Animation_H

namespace BlenderServoAnimation {

class Animation {

public:
  static const byte MODE_DEFAULT = 0;
  static const byte MODE_PLAY = 1;
  static const byte MODE_PLAY_SINGLE = 2;
  static const byte MODE_PLAY_RANDOM = 3;
  static const byte MODE_PAUSE = 4;
  static const byte MODE_LOOP = 5;
  static const byte MODE_STOP = 6;

  ~Animation();

  byte getMode();
  byte getPlayIndex();

  int countScenes();

  void addScene(const byte *data, int dataLength, byte fps, int frames);
  void addScene(Stream &data, byte fps = 0, int frames = 0);
  void onPositionChange(pcb positionCallback);
  void onModeChange(mcb modeCallback);
  void onSceneChange(scb sceneCallback);
  void run(unsigned long currentMicros = micros());
  void play();
  void playSingle(byte index);
  void playRandom();
  void loop();
  void pause();
  void stop();
  void reset();
  void setDefaultServoThreshold(byte value);
  void setServoThreshold(byte id, byte value);

  bool hasFinished();
  bool hasScenes();
  bool hasScene(byte index);
  bool modeIsIn(byte modeAmount, ...);

  Scene *getCurrentScene();

private:
  static const int MAX_SCENE_COUNT = 256;

  ServoManager servoManager;

  Scene *scenes[MAX_SCENE_COUNT] = {nullptr};
  Scene *scene = nullptr;

  mcb modeCallback = nullptr;
  scb sceneCallback = nullptr;

  byte mode = MODE_DEFAULT;

  int addIndex = 0;
  int playIndex = 0;

  void changeMode(byte mode);
  void handlePlayMode(unsigned long currentMicros);
  void handleStopMode(unsigned long currentMicros);
  void setRandomScene();
};

} // namespace BlenderServoAnimation

#endif