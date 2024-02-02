#include "AnimationData.h"
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
  static const byte MODE_LIVE = 7;

  ~Animation();

  byte getMode();
  byte getPlayIndex();

  void addScene(const byte *data, int size, byte fps, int frames);
  void addScene(Stream &stream, byte fps, int frame);
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
  void live(Stream &stream);
  void live(AnimationData &data);
  void setDefaultServoThreshold(byte value);
  void setServoThreshold(byte id, byte value);

  bool hasFinished();
  bool hasScenes();

  Scene *getCurrentScene();

private:
  ServoManager servoManager;

  Scene **scenes = nullptr;
  Scene *scene = nullptr;

  AnimationData *liveStream = nullptr;

  bool isOneTimeLiveStream = false;

  mcb modeCallback = nullptr;
  scb sceneCallback = nullptr;

  byte mode = MODE_DEFAULT;

  int addIndex = 0;
  int playIndex = 0;

  void registerScene(Scene *scene);
  void changeMode(byte mode);
  void handlePlayMode(unsigned long currentMicros);
  void handleStopMode(unsigned long currentMicros);
  void handleLiveMode();
  void setScene(byte index);
  void setRandomScene();
  void resetScene();

  bool modeIsIn(byte modeAmount, ...);
};

} // namespace BlenderServoAnimation

#endif