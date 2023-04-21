#include <Arduino.h>

#ifndef BlenderServoAnimation_Command_H
#define BlenderServoAnimation_Command_H

namespace BlenderServoAnimation {

class Command {

private:
  static const byte START_MARKER = 0x3C;
  static const byte END_MARKER = 0x3E;

  static const byte LENGTH = 5;
  static const byte BITS = 8;

  static const byte INDEX_SERVO_ID = 1;
  static const byte INDEX_POSITION_SIG_BYTE = 2;
  static const byte INDEX_POSITION_BYTE = 3;

  byte receivedBytes[LENGTH];
  byte receivedIndex = 0;

  bool receiving = false;
  bool complete = false;

public:
  void read(Stream *liveStream);

  bool isValid();
  bool isComplete();

  byte getServoID();

  int getServoPosition();
};

} // namespace BlenderServoAnimation

#endif