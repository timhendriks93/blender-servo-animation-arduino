#include "LiveCommand.h"
#include "Servo.h"
#include <Arduino.h>
#include <stdarg.h>

#ifndef BlenderServoAnimation_Animation_H
#define BlenderServoAnimation_Animation_H

namespace BlenderServoAnimation {

class Animation {

  typedef void (*mcb)(byte, byte);

private:
  static const int MAX_SERVO_COUNT = 256;
  static const long SECOND_IN_MICROS = 1000000;

  byte fps = 0;
  byte stopStepDelay = 20;
  byte mode = MODE_DEFAULT;

  int diffPerSecond = 0;

  unsigned int frame = 0;
  unsigned int frames = 0;
  unsigned int frameMicros = 0;

  unsigned long lastMicros;

  Servo *servos[MAX_SERVO_COUNT] = {};
  Stream *liveStream;
  LiveCommand liveCommand;
  mcb modeCallback = nullptr;

  void changeMode(byte mode);
  void handlePlayMode(unsigned long currentMicros);
  void handleStopMode();
  void handleLiveMode();

public:
  static const byte MODE_DEFAULT = 0;
  static const byte MODE_PLAY = 1;
  static const byte MODE_PAUSE = 2;
  static const byte MODE_LOOP = 3;
  static const byte MODE_STOP = 4;
  static const byte MODE_LIVE = 5;

  Animation();
  Animation(byte fps, int frames);

  void addServo(Servo &servo);
  void addServos(Servo servos[], byte servoAmount);
  void onModeChange(mcb modeCallback);
  void run(unsigned long currentMicros = micros());
  void play(unsigned long currentMicros = micros());
  void pause();
  void loop(unsigned long currentMicros = micros());
  void stop(byte stepDelay = 20);
  void live(Stream &liveStream);

  byte getFPS();
  byte getMode();

  int getFrame();
  int getFrames();

  bool modeIsIn(byte modeAmount, ...);
};

} // namespace BlenderServoAnimation

#endif