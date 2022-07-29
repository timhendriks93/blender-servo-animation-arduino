#include "command/Command.h"
#include "servo/Servo.h"
#include <Arduino.h>

#ifndef BlenderServoAnimation_Animation_H
#define BlenderServoAnimation_Animation_H

namespace BlenderServoAnimation {
class Animation {
private:
  static const int MAX_SERVO_COUNT = 256;
  static const int SECOND_IN_MILLIS = 1000;

  byte frameMillis = 0;
  byte stopStepDelay = 20;
  byte mode = MODE_DEFAULT;

  int frame = 0;
  int frames = 0;

  unsigned long currentMillis;
  unsigned long lastMillis;

  Servo *servos[MAX_SERVO_COUNT] = {};
  Stream *serial;
  Command command;

  void handlePlayMode();

  void handleStopMode();

  void handleLiveMode();

public:
  static const byte MODE_DEFAULT = 0;
  static const byte MODE_PAUSE = 1;
  static const byte MODE_PLAY = 2;
  static const byte MODE_STOP = 3;
  static const byte MODE_LIVE = 4;

  Animation();

  Animation(byte fps, int frames);

  void addServo(Servo &servo);

  void addServos(Servo servos[], byte servoAmount);

  void run(unsigned long currentMillis = millis());

  void play();

  void pause();

  void stop(byte stepDelay = 20);

  void live(Stream &serial);

  byte getMode();

  int getFrame();
};
} // namespace BlenderServoAnimation

#endif