#include "Animation.h"
#include <Arduino.h>
#include <stdarg.h>

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
  void handleStopMode(unsigned long currentMicros);
  void setRandomAnimation();

public:
  static const byte MODE_DEFAULT = 0;
  static const byte MODE_PLAY = 1;
  static const byte MODE_PLAY_SINGLE = 2;
  static const byte MODE_PLAY_RANDOM = 3;
  static const byte MODE_PAUSE = 4;
  static const byte MODE_LOOP = 5;
  static const byte MODE_STOP = 6;
  static const byte MODE_LIVE = 7;

  byte getMode();
  byte getPlayIndex();
  byte countAnimations();

  void addAnimation(Animation &animation);
  void addAnimations(Animation animations[], byte animationAmount);
  void onModeChange(mcb modeCallback);
  void run(unsigned long currentMicros = micros());
  void play(unsigned long currentMicros = micros());
  void playSingle(byte index, unsigned long currentMicros = micros());
  void playRandom(unsigned long currentMicros = micros());
  void loop(unsigned long currentMicros = micros());
  void pause();
  void stop(byte stepDelay = 20);
  void live(Stream &liveStream);
  void reset();

  bool hasAnimations();
  bool hasAnimation(byte index);
  bool modeIsIn(byte modeAmount, ...);

  Animation *getCurrentAnimation();
};

} // namespace BlenderServoAnimation

#endif