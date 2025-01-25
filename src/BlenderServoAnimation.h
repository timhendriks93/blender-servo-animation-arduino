#include "AnimationData.h"
#include "CommonTypes.h"
#include "Scene.h"
#include "ServoManager.h"
#include <Arduino.h>
#include <stdarg.h>

#ifndef BlenderServoAnimation_H
#define BlenderServoAnimation_H

using BlenderServoAnimationLibrary::AnimationData;
using BlenderServoAnimationLibrary::ModeCallback;
using BlenderServoAnimationLibrary::PositionCallback;
using BlenderServoAnimationLibrary::Scene;
using BlenderServoAnimationLibrary::SceneCallback;
using BlenderServoAnimationLibrary::ServoManager;

class BlenderServoAnimation {

public:
  static const byte MODE_DEFAULT = 0;
  static const byte MODE_PLAY = 1;
  static const byte MODE_PLAY_SINGLE = 2;
  static const byte MODE_PLAY_RANDOM = 3;
  static const byte MODE_PAUSE = 4;
  static const byte MODE_LOOP = 5;
  static const byte MODE_STOP = 6;
  static const byte MODE_LIVE = 7;

  ~BlenderServoAnimation();

  byte getMode();
  byte getPlayIndex();

  void addScene(const byte *data, int size, byte fps, int frames);
  void addScene(Stream &stream, byte fps, int frame);
  void onPositionChange(PositionCallback callback);
  void onModeChange(ModeCallback callback);
  void onSceneChange(SceneCallback callback);
  void run(unsigned long currentMicros = micros());
  void play();
  void playSingle(byte index);
  void playRandom();
  void loop();
  void pause();
  void stop();
  void live();
  void live(Stream &stream);
  void writeLiveStream(byte value);
  void setDefaultServoThreshold(byte value);
  void setServoThreshold(byte id, byte value);
  void setServoOffset(byte id, int offset);

  bool hasFinished();
  bool hasScenes();
  bool scenePlayed(int index);
  bool allScenesPlayed();

  Scene *getCurrentScene();

private:
  ServoManager servoManager;

  Scene **scenes = nullptr;
  Scene *scene = nullptr;

  AnimationData *liveStream = nullptr;

  bool *playedIndexes = nullptr;

  ModeCallback modeCallback = nullptr;
  SceneCallback sceneCallback = nullptr;

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
  void resetPlayedScenes();

  bool modeIsIn(byte modeAmount, ...);
};

#endif