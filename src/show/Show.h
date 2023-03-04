#include "animation/Animation.h"
#include <Arduino.h>

#ifndef BlenderServoAnimation_Show_H
#define BlenderServoAnimation_Show_H

namespace BlenderServoAnimation {

class Show {

  typedef void (*mcb)(byte, byte);

private:
  static const int MAX_ANIMATION_COUNT = 256;

  Animation *animations[MAX_ANIMATION_COUNT] = {};
  Animation *animation = nullptr;

  mcb modeCallback = nullptr;

  byte mode = MODE_DEFAULT;
  byte addIndex = 0;
  byte playIndex = 0;

  void changeMode(byte mode);
  void handlePlayMode(unsigned long currentMicros);
  void handleStopMode();

  Animation* getAnimation(byte id);
  Animation* getRandomAnimation();

public:
  static const byte MODE_DEFAULT = 0;
  static const byte MODE_PLAY = 1;
  static const byte MODE_PLAY_SINGLE = 2;
  static const byte MODE_PLAY_RANDOM = 3;
  static const byte MODE_PAUSE = 4;
  static const byte MODE_LOOP = 5;
  static const byte MODE_STOP = 6;

  byte countAnimations();
  void addAnimation(Animation &animation);
  void addAnimations(Animation animations[], byte animationAmount);
  void onModeChange(mcb modeCallback);
  void run(unsigned long currentMicros = micros());
  void play(unsigned long currentMicros = micros());
  void playSingle(byte id, unsigned long currentMicros = micros());
  void playRandom(unsigned long currentMicros = micros());
  void loop(unsigned long currentMicros = micros());
  void pause();
  void stop(byte stepDelay = 20);
  void reset();
  byte getMode();
  bool hasAnimations();
  bool hasAnimation(byte id);
  Animation* getCurrentAnimation();
};

} // namespace BlenderServoAnimation

#endif